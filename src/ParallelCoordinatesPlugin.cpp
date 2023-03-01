#include "ParallelCoordinatesPlugin.h"
#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesSettings.h"

#include "DataHierarchyItem.h"
#include "event/Event.h"
#include "Dataset.h"
#include "PointData.h"

#include <actions/PluginTriggerAction.h>
#include <widgets/DropWidget.h>

#include <QtCore>

#include <QVariantList> 
#include <QVariantMap> 
#include <QtDebug>

#include <QtConcurrent> 

#include <numeric>      // iota, accumulate
#include <algorithm>    // std::equal, clamp, count_if

Q_PLUGIN_METADATA(IID "nl.tudelft.ParallelCoordinatesPlugin")

using namespace hdps;
using namespace hdps::util;

// =============================================================================
// View
// =============================================================================

ParallelCoordinatesPlugin::ParallelCoordinatesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),  _currentDataSet(nullptr), _pcpWidget(nullptr), _settingsWidget(nullptr), _dropWidget(nullptr),
    _minClampPercent(0), _maxClampPercent(100), _numDims(0), _numSelectedDims(0), _numPoints(0)
{ 
}

ParallelCoordinatesPlugin::~ParallelCoordinatesPlugin()
{
}

void ParallelCoordinatesPlugin::init()
{
    getWidget().setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    // set layout
    QVBoxLayout* layout = new QVBoxLayout();
    
    _settingsWidget = new PCPSettings(*this);
    _pcpWidget      = new PCPWidget(this);
    _dropWidget     = new DropWidget(_pcpWidget);

    _pcpWidget->setPage(":parcoords/parcoords.html", "qrc:/parcoords/");     // set html contents of webpage

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(_settingsWidget->createWidget(&getWidget()));
    layout->addWidget(_pcpWidget, 1);

    getWidget().setLayout(layout);

    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(&getWidget(), "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        // Gather information to generate appropriate drop regions
        const auto mimeText = mimeData->text();
        const auto tokens = mimeText.split("\n");

        if (tokens.count() < 2)
            return dropRegions;

        const auto datasetName = tokens[0];
        const auto datasetId = tokens[1];
        const auto dataType = DataType(tokens[2]);
        const auto dataTypes = DataTypes({ PointType });
        const auto candidateDataset = _core->requestDataset(datasetId);

        if (dataTypes.contains(dataType)) {

            if (datasetId == getCurrentDataSetGuid()) {
                dropRegions << new DropWidget::DropRegion(this, "Warning", "Data already loaded", "exclamation-circle", false);
            }
            else {
                dropRegions << new DropWidget::DropRegion(this, "Points", QString("Visualize %1 as parallel coordinates").arg(datasetName), "map-marker-alt", true, [this, candidateDataset]() {
                    loadData({ candidateDataset });
                    _dropWidget->setShowDropIndicator(false);
                    });

            }
        }
        else {
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", "exclamation-circle", false);
        }

        return dropRegions;
        });

    // load data after drop action
    connect(this, &ParallelCoordinatesPlugin::dataSetChanged, this, &ParallelCoordinatesPlugin::onDataInput);

    // update data when data set changed
    connect(&_currentDataSet, &Dataset<Points>::dataChanged, this, &ParallelCoordinatesPlugin::onDataInput);

    // Pass selection from js to core
    connect(_pcpWidget, &PCPWidget::newSelectionToQt, this, &ParallelCoordinatesPlugin::publishSelection);

    // Update the window title when the GUI name of the position dataset changes
    connect(&_currentDataSet, &Dataset<Points>::dataGuiNameChanged, this, &ParallelCoordinatesPlugin::updateWindowTitle);

    // Update the selection
    connect(&_currentDataSet, &Dataset<Points>::dataSelectionChanged, this, &ParallelCoordinatesPlugin::onDataSelectionChanged);

    updateWindowTitle();
}

void ParallelCoordinatesPlugin::loadData(const hdps::Datasets& datasets)
{
    // Exit if there is nothing to load
    if (datasets.isEmpty())
        return;

    // Load the first dataset, changes to _currentDataSet are connected with onDataInput
    _currentDataSet = datasets.first();
    _dropWidget->setShowDropIndicator(false);
    emit dataSetChanged();
}

QString ParallelCoordinatesPlugin::getCurrentDataSetName() const 
{ 
    if (_currentDataSet.isValid())
        return _currentDataSet->getGuiName();
    else
        return QString{};
}

QString ParallelCoordinatesPlugin::getCurrentDataSetGuid() const 
{ 
    if (_currentDataSet.isValid())
        return _currentDataSet->getGuid();
    else
        return QString{};
}

void ParallelCoordinatesPlugin::onDataSelectionChanged()
{
    // Get the selection set and respective IDs that changed
    const auto& selectionSet = _currentDataSet->getSelection<Points>();
    const auto& selectionIndices = selectionSet->indices;

    // send them to js side
    _pcpWidget->passSelectionToJS(selectionIndices);
    _pcpWidget->disableBrushHighlight();
}

void ParallelCoordinatesPlugin::onDataInput()
{
    if (!_currentDataSet.isValid())
        return;

    getWidget().setWindowTitle(_currentDataSet->getGuiName());

    // get data set from core
    // Get indices of selected points
    _pointIDsGlobal = _currentDataSet->indices;
    // If points represent all data set, select them all
    if (_currentDataSet->isFull()) {
        std::vector<unsigned int> all(_currentDataSet->getNumPoints());
        std::iota(std::begin(all), std::end(all), 0);

        _pointIDsGlobal = all;
    }

    _numDims = _currentDataSet->getNumDimensions();
    _numPoints = _currentDataSet->getNumPoints();
    _dimNames = QStringList(_currentDataSet->getDimensionNames().begin(), _currentDataSet->getDimensionNames().end());

    _selectedDimensions = std::vector<bool>(_numDims, true);
    _numSelectedDims = _numDims;

    // if data set dimensions are not defined, just number them
    if (_dimNames.isEmpty()) {
        if (_numDims > 0) {
            for (std::uint32_t dimensionIndex = 0; dimensionIndex < _currentDataSet->getNumDimensions(); dimensionIndex++) {
                _dimNames.append(QString("Dim %1").arg(dimensionIndex));
            }
        }
        else
        {
            qDebug() << "ParallelCoordinatesPlugin: unable to load data set";
            return;
        }
    }

    // calc min and max per channel of source data
    calculateMinMaxPerDim();
    // init the clamping valies
    calculateMinMaxClampPerDim();

    // setup dimensio selection widget
    auto& dimensionSelectionWidget = _settingsWidget->getDimensionSelectionAction();
    dimensionSelectionWidget.getDimensionsPickerAction().setPointsDataset(_currentDataSet);
    dimensionSelectionWidget.setNumPoints(_numPoints);
    dimensionSelectionWidget.setNumDims(_numSelectedDims);
    dimensionSelectionWidget.setNumItems(_numPoints * _numSelectedDims);

    // parse data to JS in a different thread as to not block the UI
    QFuture<void> fvoid = QtConcurrent::run(&ParallelCoordinatesPlugin::passDataToJS, this, _pointIDsGlobal);

    updateWindowTitle();
}

void ParallelCoordinatesPlugin::calculateMinMaxPerDim()
{
    _minMaxPerDim.clear();
    _minMaxPerDim.resize(2 * _numDims);

    std::vector<float> attribute_data;
    _currentDataSet->visitFromBeginToEnd([&attribute_data](auto begin, auto end)
    {
        attribute_data.insert(attribute_data.begin(), begin, end);
    });

    float valRange = 0;
    uint32_t minIndex = 0;
    uint32_t maxIndex = 0;
    // for each dimension iterate over all values
    // remember data stucture (point1 d0, point1 d1,... point1 dn, point2 d0, point2 d1, ...)
    for (uint32_t dimCount = 0; dimCount < _numDims; dimCount++) {
        // init min and max
        minIndex = 2 * dimCount;
        maxIndex = 2 * dimCount + 1;

        float currentVal = attribute_data[dimCount];
        _minMaxPerDim[minIndex] = currentVal;
        _minMaxPerDim[maxIndex] = currentVal;

        for (uint32_t pointCount = 0; pointCount < _numPoints; pointCount++) {
            currentVal = attribute_data[pointCount * _numDims + dimCount];
            // min
            if (currentVal < _minMaxPerDim[minIndex])
                _minMaxPerDim[minIndex] = currentVal;
            // max
            else if (currentVal > _minMaxPerDim[maxIndex])
                _minMaxPerDim[maxIndex] = currentVal;
        }
    }

}

void ParallelCoordinatesPlugin::calculateMinMaxClampPerDim()
{
    if (!_currentDataSet.isValid()) // data set not yet set
        return;

    _minMaxClampPerDim.clear();
    _minMaxClampPerDim.resize(2 * _numDims);

    float valRange = 0;
    uint32_t minIndex = 0;
    uint32_t maxIndex = 0;

    for (uint32_t dimCount = 0; dimCount < _numDims; dimCount++) {
        // init min and max
        minIndex = 2 * dimCount;
        maxIndex = 2 * dimCount + 1;

        // set clamp values wrt to min and max in each dim and the user specified percentages
        valRange = _minMaxPerDim[maxIndex] - _minMaxPerDim[minIndex];
        _minMaxClampPerDim[minIndex] = _minMaxPerDim[minIndex] + static_cast<float>(_minClampPercent) / 100.0f * valRange;
        _minMaxClampPerDim[maxIndex] = _minMaxPerDim[maxIndex] - (1 - (static_cast<float>(_maxClampPercent) / 100.0f)) * valRange;
    }
}

void ParallelCoordinatesPlugin::passDataToJS(const std::vector<unsigned int>& pointIDsGlobal)
{
    // Qt has an internal maximum size for JSON files
    // For now, set an arbitrary, lower limit for points to display
    // If you want to instepct more then 7 million elements - don't use this parallel coordinates plugin
    if (_numSelectedDims * _numPoints > 7000000)
    {
        qDebug() << "ParallelCoordinatesPlugin: Data set too large - select fewer dimensions";
        return;
    }

    QVariantList payload;
    QVariantMap dimension;

    _currentDataSet->visitFromBeginToEnd([&pointIDsGlobal, &dimension, &payload, this](auto beginOfData, auto endOfData)
    {
        qDebug() << "ParallelCoordinatesPlugin: Prepare payload";
        QString dimName;

        for (const auto& pointId : pointIDsGlobal)
        {
            dimension.clear();
            dimension["__pointID"] = pointId;
            for (uint32_t dimId = 0; dimId < _numDims; dimId++)
            {
                if (_selectedDimensions[dimId] == false)
                    continue;

                // Reduce clutter by only showing numbers if there are many dimensions
                if (_numSelectedDims < 10)
                    dimName = _dimNames[dimId];
                else
                    dimName = QString::number(dimId);

                // clamp data if necessary
                dimension[dimName] = std::clamp((float)beginOfData[pointId * _numDims + dimId], _minMaxClampPerDim[2 * dimId], _minMaxClampPerDim[2 * dimId +1]);
            }
            payload.append(dimension);
        }
    });

    _pcpWidget->passDataToJS(payload);
}

void ParallelCoordinatesPlugin::applyClamping() {

    int32_t newMinClamp = _settingsWidget->getClampAction().getMinClamp();
    int32_t newMaxClamp = _settingsWidget->getClampAction().getMaxClamp();

    // min and max clamp are the same
    if ((newMinClamp == _minClampPercent) && (newMaxClamp == _maxClampPercent))
    {
        qDebug() << "ParallelCoordinatesPlugin: Same clamping values";
        return;
    }

    // Adjust clamping
    _minClampPercent = newMinClamp;
    _maxClampPercent = newMaxClamp;
    calculateMinMaxClampPerDim();

    // parse data to JS in a different thread as to not block the UI
    QFuture<void> fvoid = QtConcurrent::run(&ParallelCoordinatesPlugin::passDataToJS, this, _pointIDsGlobal);
}

void ParallelCoordinatesPlugin::applyDimensionSelection() {

    auto& dimensionSelectionWidget = _settingsWidget->getDimensionSelectionAction();
    std::vector<bool> newDimSelection = dimensionSelectionWidget.getDimensionsPickerAction().getEnabledDimensions();
    const auto newNumSelectedDims = std::count_if(newDimSelection.begin(), newDimSelection.end(), [](bool b) { return b; });

    // check if new dim selection is any different from the current one
    if (std::equal(_selectedDimensions.begin(), _selectedDimensions.end(), newDimSelection.begin()))
    {
        qDebug() << "ParallelCoordinatesPlugin: Same dimension selection";
        return;

    }
    // don't show less than two dimensions
    if (newNumSelectedDims < 2)
    {
        qDebug() << "ParallelCoordinatesPlugin: Select at least 2 dimensions";
        return;
    }

    _selectedDimensions = newDimSelection;
    _numSelectedDims = newNumSelectedDims;

    // update info
    dimensionSelectionWidget.setNumDims(_numSelectedDims);
    dimensionSelectionWidget.setNumItems(_numPoints * _numSelectedDims);

    // parse data to JS in a different thread as to not block the UI
    QFuture<void> fvoid = QtConcurrent::run(&ParallelCoordinatesPlugin::passDataToJS, this, _pointIDsGlobal);

}

void ParallelCoordinatesPlugin::publishSelection(const std::vector<unsigned int>& selectedIDs)
{
    _pcpWidget->enableBrushHighlight();

    // ask core for the selection set for the current data set
    auto selectionSet      = _currentDataSet->getSelection<Points>();
    auto& selectionIndices  = selectionSet->indices;

    // no need to update the selection when nothing is updated
    if ((selectedIDs.size() == 0) & (selectionIndices.size() == 0))
    {
        _pcpWidget->disableBrushHighlight();   // this makes sure that the brush indicator will be removed when selection from other plugins come in
        return;
    }

    // clear the selection and add the new points
    selectionIndices.clear();
    selectionIndices.reserve(_numPoints);
    for (const auto id : selectedIDs) {
        selectionIndices.push_back(id);
    }

    // notify core about the selection change
    if (_currentDataSet->isDerivedData())
        events().notifyDatasetSelectionChanged(_currentDataSet->getSourceDataset<DatasetImpl>());
    else
        events().notifyDatasetSelectionChanged(_currentDataSet);

}

void ParallelCoordinatesPlugin::updateWindowTitle()
{
    if (!_currentDataSet.isValid())
        getWidget().setWindowTitle(getGuiName());
    else
        getWidget().setWindowTitle(QString("%1: %2").arg(getGuiName(), _currentDataSet->getDataHierarchyItem().getFullPathName()));
}


// =============================================================================
// Plugin Factory 
// =============================================================================

QIcon ParallelCoordinatesPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("chart-bar", color);
}

ViewPlugin* ParallelCoordinatesPluginFactory::produce()
{
    return new ParallelCoordinatesPlugin(this);
}

hdps::DataTypes ParallelCoordinatesPluginFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;
    supportedTypes.append(PointType);
    return supportedTypes;
}

hdps::gui::PluginTriggerActions ParallelCoordinatesPluginFactory::getPluginTriggerActions(const hdps::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getInstance = [this]() -> ParallelCoordinatesPlugin* {
        return dynamic_cast<ParallelCoordinatesPlugin*>(plugins().requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (PluginFactory::areAllDatasetsOfTheSameType(datasets, PointType)) {
        if (numberOfDatasets >= 1) {
            if (datasets.first()->getDataType() == PointType) {
                auto pluginTriggerAction = new PluginTriggerAction(const_cast<ParallelCoordinatesPluginFactory*>(this), this, "Parallel coordinates", "Load dataset in parallel coordinates viewer", getIcon(), [this, getInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                    for (auto dataset : datasets)
                        getInstance()->loadData(Datasets({ dataset }));
                });

                pluginTriggerActions << pluginTriggerAction;
            }
        }
    }

    return pluginTriggerActions;
}
