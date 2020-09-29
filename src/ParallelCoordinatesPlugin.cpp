#include "ParallelCoordinatesPlugin.h"

#include "PointData.h"
#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesSettings.h"

#include <QtCore>
#include <QtConcurrent> 
#include <QtDebug>

#include <string>
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
	setMainLayout(layout);


	//
	_parCoordWidget = new ParlCoorWidget(this);
	_parCoordWidget->setPage(":parcoords/parcoords.html", "qrc:/parcoords/");
    addWidget(_parCoordWidget);

	//
	_settingsWidget = new ParlCoorSettings();
	addWidget(_settingsWidget);

	//
	connect(_settingsWidget, &ParlCoorSettings::onDataInput, this, &ParallelCoordinatesPlugin::onDataInput);
	connect(_parCoordWidget, &ParlCoorWidget::newSelectionToQt, this, &ParallelCoordinatesPlugin::publishSelection);

}

void ParallelCoordinatesPlugin::onDataInput(const QString dataSetName)
{
	_currentDataSetName = dataSetName;
	setWindowTitle(dataSetName);

	// parse data to JS in a different thread as to not block the UI
	QtConcurrent::run(this, &ParallelCoordinatesPlugin::passDataToJS, dataSetName);

}


void ParallelCoordinatesPlugin::passDataToJS(const QString dataSetName)
{
	// TODO: might be faster to not use JSON but rather hand over an array with the first entry for each point as the ID and then go .hideAxis([0])
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

	_dimNames = QStringList(_currentDataSet->getDimensionNames().begin(), _currentDataSet->getDimensionNames().end());
	_numDims = _currentDataSet->getNumDimensions();
	_numPoints = _currentDataSet->getNumPoints();

	// write data to json string
	std::string jsonObject = "[";
	std::vector<std::string> jsonPoints(_numPoints, "");

	qDebug() << "ParallelCoordinatesPlugin: Prepare JSON string for data exchange";

	_currentDataSet->visitFromBeginToEnd([&jsonPoints, &pointIDsGlobal, this](auto beginOfData, auto endOfData)
	{
		std::string currentPoint = "";
		// parse values of each point to JSON
		// for each point "{ "__pointID" : ID, "dimName0" : Val, "dimName1" : Vals, ...}
		// TODO: parallelize this
		for (const auto& pointId : pointIDsGlobal)
		{
			currentPoint = "{";

			// add point ID
			currentPoint.append("\"__pointID\" : " + std::to_string(pointId) + ",");

			// add channel names and values
			for (unsigned int dimId = 0; dimId < _numDims; dimId++)
			{
				currentPoint.append("\"" + _dimNames[dimId].toStdString() + "\" : " + std::to_string(beginOfData[pointId * _numDims + dimId]) + ",");
			}
			currentPoint.back() = '}';	// replace the last , with a closing brace

			jsonPoints[pointId] = currentPoint;
		}
	});

	// Join all points with a delimiter ","
	jsonObject.append(
		std::accumulate(std::begin(jsonPoints), std::end(jsonPoints), std::string(),
			[](std::string &ss, std::string &s)
	{
		return ss.empty() ? s : ss + "," + s;
	}));

	jsonObject.append("]");	// end JSON


	qDebug() << "ParallelCoordinatesPlugin: Passing JSON to .js side";
	_parCoordWidget->passDataToJS(jsonObject);
}


void ParallelCoordinatesPlugin::dataAdded(const QString name)
{
	// Current, this plugin does not care whether data was added
}

void ParallelCoordinatesPlugin::dataChanged(const QString name)
{
	// Current, this plugin does not care whether data was changed
}

void ParallelCoordinatesPlugin::dataRemoved(const QString name)
{
	// Current, this plugin does not care whether data was removed
}

/**
 * Callback which gets triggered when any plugin calls notifySelectionChanged().
 * The name of the data whose selection was changed is given and the new selection
 * can be retrieved through _core->requestSelection(dataName);
 */
void ParallelCoordinatesPlugin::selectionChanged(const QString dataName)
{
	// get indices from core
	auto selectedPoints = dynamic_cast<Points&>(_core->requestSelection(dataName));

	// send them to js side
	_parCoordWidget->passSelectionToJS(selectedPoints.indices);

	_parCoordWidget->disableBrushHighlight();
}

void ParallelCoordinatesPlugin::publishSelection(std::vector<unsigned int> selectedIDs)
{
	_parCoordWidget->enableBrushHighlight();

	// ask core for the selection set for the current data set
	auto& selectionIndices = dynamic_cast<Points&>(_core->requestSelection(_currentDataSet->getDataName())).indices;
	auto& sourceIndices = _currentDataSet->getSourceData<Points>(*_currentDataSet).indices;

	// no need to update the selection when nothing is updated
	if ((selectedIDs.size() == 0) & (selectionIndices.size() == 0))
		return;

	// clear the selection and add the new points
	selectionIndices.clear();
	selectionIndices.reserve(_numPoints);
	for (auto id : selectedIDs) {
		selectionIndices.push_back(id);
	}

	// notify core about the selection change
	if (_currentDataSet->isDerivedData())
		_core->notifySelectionChanged(_currentDataSet->getSourceData<Points>(*_currentDataSet).getDataName());
	else
		_core->notifySelectionChanged(_currentDataSet->getDataName());

}

DataTypes ParallelCoordinatesPlugin::supportedDataTypes() const
{
	DataTypes supportedTypes;
	supportedTypes.append(PointType);
	return supportedTypes;
}


// =============================================================================
// Factory DOES NOT NEED TO BE ALTERED
// Merely responsible for generating new plugins when requested
// =============================================================================

ViewPlugin* ParallelCoordinatesPluginFactory::produce()
{
    return new ParallelCoordinatesPlugin();
}
