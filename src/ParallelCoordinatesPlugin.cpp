#include "ParallelCoordinatesPlugin.h"

#include "PointData.h"
#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesSettings.h"

#include <QtCore>
#include <QtConcurrent> 
#include <QVariantList> 
#include <QVariantMap> 
#include <QtDebug>

#include <numeric>      // iota, accumulate
#include <algorithm>    // std::equal

Q_PLUGIN_METADATA(IID "nl.tudelft.ParallelCoordinatesPlugin")

using namespace hdps;

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

ParallelCoordinatesPlugin::ParallelCoordinatesPlugin() : 
    ViewPlugin("Parallel Coordinates"), _currentDataSet(nullptr), _parCoordWidget(nullptr), _settingsWidget(nullptr)
{ 
}

ParallelCoordinatesPlugin::~ParallelCoordinatesPlugin(void)
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

    // Signal conenctions
    connect(_parCoordWidget, &ParlCoorWidget::newSelectionToQt, this, &ParallelCoordinatesPlugin::publishSelection);

    // Register for data events for points datasets
    registerDataEventByType(PointType, std::bind(&ParallelCoordinatesPlugin::onDataEvent, this, std::placeholders::_1));

    updateWindowTitle();
}

void ParallelCoordinatesPlugin::onDataEvent(hdps::DataEvent* dataEvent)
{
    if (dataEvent->getType() == EventType::SelectionChanged)
    {
        if (dataEvent->dataSetName != _currentDataSetName || _currentDataSet == nullptr)
            return;

      auto& selectionSet      = dynamic_cast<Points&>(_currentDataSet->getSelection());
        auto& selectionIndices  = selectionSet.indices;

        // send them to js side
        _parCoordWidget->passSelectionToJS(selectionIndices);
        _parCoordWidget->disableBrushHighlight();

        _settingsWidget->setNumSel(selectionSet.getNumPoints());
    }
}

void ParallelCoordinatesPlugin::onDataInput(const QString dataSetName)
{
    _currentDataSetName = dataSetName;
    setWindowTitle(_currentDataSetName);

    // get data set from core
    _currentDataSet = &_core->requestData<Points>(_currentDataSetName);
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

    _settingsWidget->setNumPoints(_numPoints);
    _settingsWidget->setDimensionNames(_dimNames);
    _settingsWidget->setNumDims(_dimNames.length());
    _settingsWidget->setNumSel(0);

    // parse data to JS in a different thread as to not block the UI
    QtConcurrent::run(this, &ParallelCoordinatesPlugin::passDataToJS, _currentDataSetName, _pointIDsGlobal);

    updateWindowTitle();
}

void ParallelCoordinatesPlugin::initMainView() {
    _parCoordWidget->setPage(":parcoords/parcoords.html", "qrc:/parcoords/");
}

void ParallelCoordinatesPlugin::onRefreshMainView() {
    initMainView();
}

void ParallelCoordinatesPlugin::passDataToJS(const QString dataSetName, const std::vector<unsigned int>& pointIDsGlobal)
{
    // Qt has an internal maximum size for JSON files
    // For now, set an arbitrary, lower limit for points to display
    // If you want to instepct more then 5 million elements - don't use parallel coordinates
    if (_numSelectedDims * _numPoints > 5000000)
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

                dimension[dimName] = (float)beginOfData[pointId * _numDims + dimId];
            }
            payload.append(dimension);
        }
    });

    _parCoordWidget->passDataToJS(payload);
}

void ParallelCoordinatesPlugin::onDimensionSelectionChanged() {
    std::vector<bool> newDimSelection = _settingsWidget->getSelectedDimensions();
    unsigned int newNumSelectedDims = std::accumulate(newDimSelection.begin(), newDimSelection.end(), (unsigned int)(0));

    // check if new dim selection is any different from the current one
    if (std::equal(_selectedDimensions.begin(), _selectedDimensions.end(), newDimSelection.begin()))
    {
        qDebug() << "ParallelCoordinatesPlugin: View not changed - same dimension selection";
        return;
    }
    // don't show less than two dimensions
    if (newNumSelectedDims < 2)
    {
        qDebug() << "ParallelCoordinatesPlugin: View not changed - select at least 2 dimensions";
        return;
    }

    _selectedDimensions = newDimSelection;
    _numSelectedDims = newNumSelectedDims;

    // parse data to JS in a different thread as to not block the UI
    QtConcurrent::run(this, &ParallelCoordinatesPlugin::passDataToJS, _currentDataSetName, _pointIDsGlobal);

}

void ParallelCoordinatesPlugin::publishSelection(std::vector<unsigned int> selectedIDs)
{
    _parCoordWidget->enableBrushHighlight();

    // ask core for the selection set for the current data set

    auto& selectionSet      = dynamic_cast<Points&>(_currentDataSet->getSelection());
    auto& selectionIndices  = selectionSet.indices;
    auto& sourceIndices     = _currentDataSet->getSourceData<Points>(*_currentDataSet).indices;

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
        _core->notifySelectionChanged(_currentDataSet->getSourceData<Points>(*_currentDataSet).getName());
    else
        _core->notifySelectionChanged(_currentDataSet->getName());

    //
    _settingsWidget->setNumSel(selectedIDs.size());

}

void ParallelCoordinatesPlugin::updateWindowTitle()
{
    if (_currentDataSetName.isEmpty())
        setWindowTitle(getGuiName());
    else
        setWindowTitle(QString("%1: %2").arg(getGuiName(), _currentDataSetName));
}


// =============================================================================
// Factory DOES NOT NEED TO BE ALTERED
// Merely responsible for generating new plugins when requested
// =============================================================================

ViewPlugin* ParallelCoordinatesPluginFactory::produce()
{
    return new ParallelCoordinatesPlugin();
}
