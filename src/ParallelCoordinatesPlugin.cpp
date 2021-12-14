#include "ParallelCoordinatesPlugin.h"
#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesSettings.h"

#include "DataHierarchyItem.h"
#include "event/Event.h"
#include "Dataset.h"
#include "PointData.h"

#include <QtCore>
#include <QtConcurrent> 
#include <QVariantList> 
#include <QVariantMap> 
#include <QtDebug>

#include <numeric>      // iota, accumulate
#include <algorithm>    // std::equal, clamp

Q_PLUGIN_METADATA(IID "nl.tudelft.ParallelCoordinatesPlugin")

using namespace hdps;
using namespace hdps::util;

// =============================================================================
// View
// =============================================================================

namespace hdps
{
    /**
     * Creates a container of the specified type, and copies the elements from the
     * specified `std::vector` into the created container.
     */
    template <typename ContainerType, typename ValueType>
    auto fromStdVector(const std::vector<ValueType>& stdVector)
    {
        ContainerType result;
        result.reserve(static_cast<int>(stdVector.size()));
        std::copy(stdVector.cbegin(), stdVector.cend(), std::back_inserter(result));
        return result;
    }
}

ParallelCoordinatesPlugin::ParallelCoordinatesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),  _currentDataSet(nullptr), _parCoordWidget(nullptr), _settingsWidget(nullptr),
    _minClampPercent(0), _maxClampPercent(100), _numDims(0), _numSelectedDims(0), _numPoints(0)
{ 
}

ParallelCoordinatesPlugin::~ParallelCoordinatesPlugin()
{
}

void ParallelCoordinatesPlugin::init()
{
    // General
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    // Main Widget view
    _parCoordWidget = new ParlCoorWidget(this);
    initMainView();     // sets html page in _parCoordWidget
    layout->addWidget(_parCoordWidget);

    // Plugin setting
    _settingsWidget = new ParlCoorSettings(this);
    layout->addWidget(_settingsWidget);

    // load data after drop action
    connect(this, &ParallelCoordinatesPlugin::dataSetChanged, this, &ParallelCoordinatesPlugin::onDataInput);

    // load data after right-click view 
    connect(_parCoordWidget, &ParlCoorWidget::webViewLoaded, this, &ParallelCoordinatesPlugin::onDataInput);

    // Pass selection from js to core
    connect(_parCoordWidget, &ParlCoorWidget::newSelectionToQt, this, &ParallelCoordinatesPlugin::publishSelection);

    // Update the window title when the GUI name of the position dataset changes
    connect(&_currentDataSet, &Dataset<Points>::dataGuiNameChanged, this, &ParallelCoordinatesPlugin::updateWindowTitle);

    // Update the selection
    connect(&_currentDataSet, &Dataset<Points>::dataSelectionChanged, this, &ParallelCoordinatesPlugin::onDataSelectionChanged);

    updateWindowTitle();
}

void ParallelCoordinatesPlugin::loadData(const QVector<Dataset<DatasetImpl>>& datasets)
{
    // Exit if there is nothing to load
    if (datasets.isEmpty())
        return;

    // Load the first dataset, changes to _currentDataSet are connected with onDataInput
    _currentDataSet = datasets.first();
    _parCoordWidget->setDropWidgetShowDropIndicator(false);
}


void ParallelCoordinatesPlugin::setData(QString newDatasetGuid)
{ 
    _currentDataSet = _core->requestDataset<Points>(newDatasetGuid); 
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
    _parCoordWidget->passSelectionToJS(selectionIndices);
    _parCoordWidget->disableBrushHighlight();

    _settingsWidget->setNumSel(selectionSet->getNumPoints());

}

void ParallelCoordinatesPlugin::onDataInput()
{
    if (!_currentDataSet.isValid())
        return;

    setWindowTitle(_currentDataSet->getGuiName());

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

    _settingsWidget->setNumPoints(_numPoints);
    _settingsWidget->setDimensionNames(_dimNames);
    _settingsWidget->setNumDims(_dimNames.length());
    _settingsWidget->setNumSel(0);

    // parse data to JS in a different thread as to not block the UI
    QtConcurrent::run(this, &ParallelCoordinatesPlugin::passDataToJS, _pointIDsGlobal);

    updateWindowTitle();
}

void ParallelCoordinatesPlugin::initMainView() {
    _parCoordWidget->setPage(":parcoords/parcoords.html", "qrc:/parcoords/");
}

void ParallelCoordinatesPlugin::onRefreshMainView() {
    initMainView();
}

void ParallelCoordinatesPlugin::minDimClampChanged(int min)
{
    _minClampPercent = min;
}

void ParallelCoordinatesPlugin::maxDimClampChanged(int max)
{
    _maxClampPercent = max;
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
    int minIndex = 0;
    int maxIndex = 0;
    // for each dimension iterate over all values
    // remember data stucture (point1 d0, point1 d1,... point1 dn, point2 d0, point2 d1, ...)
    for (unsigned int dimCount = 0; dimCount < _numDims; dimCount++) {
        // init min and max
        minIndex = 2 * dimCount;
        maxIndex = 2 * dimCount + 1;

        float currentVal = attribute_data[dimCount];
        _minMaxPerDim[minIndex] = currentVal;
        _minMaxPerDim[maxIndex] = currentVal;

        for (unsigned int pointCount = 0; pointCount < _numPoints; pointCount++) {
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
    int minIndex = 0;
    int maxIndex = 0;

    for (unsigned int dimCount = 0; dimCount < _numDims; dimCount++) {
        // init min and max
        minIndex = 2 * dimCount;
        maxIndex = 2 * dimCount + 1;

        // set clamp values wrt to min and max in each dim and the user specified percentages
        valRange = _minMaxPerDim[maxIndex] - _minMaxPerDim[minIndex];
        _minMaxClampPerDim[minIndex] = _minMaxPerDim[minIndex] + (float)_minClampPercent / 100.0 * valRange;
        _minMaxClampPerDim[maxIndex] = _minMaxPerDim[maxIndex] - (1 - ((float)_maxClampPercent / 100.0)) * valRange;
    }
}

void ParallelCoordinatesPlugin::passDataToJS(const std::vector<unsigned int>& pointIDsGlobal)
{
    // Qt has an internal maximum size for JSON files
    // For now, set an arbitrary, lower limit for points to display
    // If you want to instepct more then 7 million elements - don't use parallel coordinates
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
            for (unsigned int dimId = 0; dimId < _numDims; dimId++)
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

    _parCoordWidget->passDataToJS(payload);
}

void ParallelCoordinatesPlugin::onApplySettings() {
    bool settingChanged = true;

    // current settings
    std::vector<bool> newDimSelection = _settingsWidget->getSelectedDimensions();
    unsigned int newNumSelectedDims = std::accumulate(newDimSelection.begin(), newDimSelection.end(), (unsigned int)(0));

    int newMinClamp = _settingsWidget->getMinClamp();
    int newMaxClamp = _settingsWidget->getMaxClamp();

    // compare to saved settings

    // check if new dim selection is any different from the current one
    if (std::equal(_selectedDimensions.begin(), _selectedDimensions.end(), newDimSelection.begin()))
    {
        qDebug() << "ParallelCoordinatesPlugin: Same dimension selection";
        settingChanged = false;
    }
    // don't show less than two dimensions
    if (newNumSelectedDims < 2)
    {
        qDebug() << "ParallelCoordinatesPlugin: Select at least 2 dimensions";
        settingChanged = false;
    }

    // min and max clamp are the same
    if ((newMinClamp == _minClampPercent) && (newMaxClamp == _maxClampPercent) && !settingChanged)
    {
        qDebug() << "ParallelCoordinatesPlugin: Same clamping values";
        settingChanged = false;
    }
    else
    {
        settingChanged = true;

        // Adjust clamping
        minDimClampChanged(newMinClamp);
        maxDimClampChanged(newMaxClamp);
        calculateMinMaxClampPerDim();
    }

    if (!settingChanged)
        return;

    _selectedDimensions = newDimSelection;
    _numSelectedDims = newNumSelectedDims;

    // parse data to JS in a different thread as to not block the UI
    QtConcurrent::run(this, &ParallelCoordinatesPlugin::passDataToJS, _pointIDsGlobal);

}

void ParallelCoordinatesPlugin::publishSelection(std::vector<unsigned int> selectedIDs)
{
    _parCoordWidget->enableBrushHighlight();

    // ask core for the selection set for the current data set

    auto& selectionSet      = _currentDataSet->getSelection<Points>();
    auto& selectionIndices  = selectionSet->indices;
    //auto& sourceIndices     = _currentDataSet->getSourceData<Points>(*_currentDataSet).indices;

    // no need to update the selection when nothing is updated
    if ((selectedIDs.size() == 0) & (selectionIndices.size() == 0))
    {
        _parCoordWidget->disableBrushHighlight();   // this makes sure that the brush indicator will be removed when selection from other plugins come in
        return;
    }

    // clear the selection and add the new points
    selectionIndices.clear();
    selectionIndices.reserve(_numPoints);
    for (auto id : selectedIDs) {
        selectionIndices.push_back(id);
    }

    // notify core about the selection change
    if (_currentDataSet->isDerivedData())
        _core->notifyDataSelectionChanged(_currentDataSet->getSourceDataset<DatasetImpl>());
    else
        _core->notifyDataSelectionChanged(_currentDataSet);

    //
    _settingsWidget->setNumSel(selectedIDs.size());

}

void ParallelCoordinatesPlugin::updateWindowTitle()
{
    if (!_currentDataSet.isValid())
        setWindowTitle(getGuiName());
    else
        setWindowTitle(QString("%1: %2").arg(getGuiName(), _currentDataSet->getDataHierarchyItem().getFullPathName()));
}


// =============================================================================
// Factory DOES NOT NEED TO BE ALTERED
// Merely responsible for generating new plugins when requested
// =============================================================================

ViewPlugin* ParallelCoordinatesPluginFactory::produce()
{
    return new ParallelCoordinatesPlugin(this);
}

hdps::DataTypes ParallelCoordinatesPluginFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;

    // This example analysis plugin is compatible with points datasets
    supportedTypes.append(PointType);

    return supportedTypes;
}