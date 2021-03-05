#include "ParallelCoordinatesPlugin.h"

#include "PointData.h"
#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesSettings.h"

#include <QtCore>
#include <QtConcurrent> 
#include <QVariantList> 
#include <QVariantMap> 
#include <QtDebug>

#include <numeric>

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
    //
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);


    //
    _parCoordWidget = new ParlCoorWidget(this);
    _parCoordWidget->setPage(":parcoords/parcoords.html", "qrc:/parcoords/");
    layout->addWidget(_parCoordWidget);

    //
    _settingsWidget = new ParlCoorSettings();
    layout->addWidget(_settingsWidget);

    //
    connect(_settingsWidget, &ParlCoorSettings::onDataInput, this, &ParallelCoordinatesPlugin::onDataInput);            // pass name to core
    connect(_parCoordWidget, &ParlCoorWidget::newSelectionToQt, this, &ParallelCoordinatesPlugin::publishSelection);

    // Register for data events for points datasets
    registerDataEventByType(PointType, std::bind(&ParallelCoordinatesPlugin::onDataEvent, this, std::placeholders::_1));
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
    setWindowTitle(dataSetName);

    // get data set from core
    _currentDataSet = &_core->requestData<Points>(dataSetName);
    // Get indices of selected points
    std::vector<unsigned int> pointIDsGlobal = _currentDataSet->indices;
    // If points represent all data set, select them all
    if (_currentDataSet->isFull()) {
        std::vector<unsigned int> all(_currentDataSet->getNumPoints());
        std::iota(std::begin(all), std::end(all), 0);

        pointIDsGlobal = all;
    }

    _numDims = _currentDataSet->getNumDimensions();
    _numPoints = _currentDataSet->getNumPoints();
    _dimNames = QStringList(_currentDataSet->getDimensionNames().begin(), _currentDataSet->getDimensionNames().end());

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
    _settingsWidget->setNumDims(_dimNames.length());
    _settingsWidget->setNumSel(0);

    // parse data to JS in a different thread as to not block the UI
    QtConcurrent::run(this, &ParallelCoordinatesPlugin::passDataToJS, dataSetName, pointIDsGlobal);
}


void ParallelCoordinatesPlugin::passDataToJS(const QString dataSetName, const std::vector<unsigned int>& pointIDsGlobal)
{

    QVariantList payload;
    QVariantMap dimension;

    _currentDataSet->visitFromBeginToEnd([&pointIDsGlobal, &dimension, &payload, this](auto beginOfData, auto endOfData)
    {
        qDebug() << "ParallelCoordinatesPlugin: Prepare payload";
        for (const auto& pointId : pointIDsGlobal)
        {
            dimension.clear();
            dimension["__pointID"] = pointId;
            for (unsigned int dimId = 0; dimId < _numDims; dimId++)
            {
                dimension[_dimNames[dimId]] = (float)beginOfData[pointId * _numDims + dimId];
            }
            payload.append(dimension);
        }
    });

    _parCoordWidget->passDataToJS(payload);
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

// =============================================================================
// Factory DOES NOT NEED TO BE ALTERED
// Merely responsible for generating new plugins when requested
// =============================================================================

ViewPlugin* ParallelCoordinatesPluginFactory::produce()
{
    return new ParallelCoordinatesPlugin();
}
