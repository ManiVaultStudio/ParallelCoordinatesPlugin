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
	_parCoordWidget = new ParlCoorWidget();
	_parCoordWidget->setPage(":parcoords/parcoords.html", "qrc:/parcoords/");
    addWidget(_parCoordWidget);

	//
	_settingsWidget = new ParlCoorSettings();
	addWidget(_settingsWidget);

	//
	connect(_settingsWidget, &ParlCoorSettings::onDataInput, this, &ParallelCoordinatesPlugin::onDataInput);
}

void ParallelCoordinatesPlugin::onDataInput(const QString dataSetName)
{
	_currentDataSet = dataSetName;
	setWindowTitle(dataSetName);

	// parse data to JS in a different thread as to not block the UI
	QtConcurrent::run(this, &ParallelCoordinatesPlugin::passDataToJS, dataSetName);

}


void ParallelCoordinatesPlugin::passDataToJS(const QString dataSetName)
{

	// get data set from core
	const Points& points = _core->requestData<Points>(dataSetName);
	// Get indices of selected points
	std::vector<unsigned int> pointIDsGlobal = points.indices;
	// If points represent all data set, select them all
	if (points.isFull()) {
		std::vector<unsigned int> all(points.getNumPoints());
		std::iota(std::begin(all), std::end(all), 0);

		pointIDsGlobal = all;
	}

	_dimNames = QStringList(points.getDimensionNames().begin(), points.getDimensionNames().end());
	_numDims = points.getNumDimensions();
	_numPoints = points.getNumPoints();

	// write data to json string
	std::string jsonObject = "[";
	std::vector<std::string> jsonPoints(_numPoints, "");

	qDebug() << "ParallelCoordinatesPlugin: Prepare JSON string for data exchange";

	// TODO: call this in a backgroud function
	points.visitFromBeginToEnd([&jsonPoints, &pointIDsGlobal, this](auto beginOfData, auto endOfData)
	{
		std::string currentPoint = "";
		// parse values of each point to JSON
		// for each point "{ "dimName0" : Val, "dimName1" : Vals, ...}
		// TODO: parallelize this
		for (const auto& pointId : pointIDsGlobal)
		{
			currentPoint = "{";
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


/**
 * Callback which gets triggered when a dataset is added.
 * The name of the dataset which was added is given.
 * The added dataset can be retrieved through _core->requestSet(name);
 * and casting it to the appropriate high-level set.
 */
void ParallelCoordinatesPlugin::dataAdded(const QString name)
{
	_settingsWidget->coreDataSets.addItem(name);
}

/**
 * Callback which gets triggered when a dataset changes.
 * The name of the dataset which was changed is given.
 * The changed dataset can be retrieved through _core->requestSet(name);
 * and casting it to the appropriate high-level set.
 */
void ParallelCoordinatesPlugin::dataChanged(const QString name)
{

}

/**
 * Callback which gets triggered when a dataset gets removed.
 * The name of the dataset which was removed is given.
 */
void ParallelCoordinatesPlugin::dataRemoved(const QString name)
{
    
}

/**
 * Callback which gets triggered when any plugin calls notifySelectionChanged().
 * The name of the data whose selection was changed is given and the new selection
 * can be retrieved through _core->requestSelection(dataName);
 */
void ParallelCoordinatesPlugin::selectionChanged(const QString dataName)
{

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
