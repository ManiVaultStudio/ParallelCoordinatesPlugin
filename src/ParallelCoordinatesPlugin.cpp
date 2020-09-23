#include "ParallelCoordinatesPlugin.h"

#include "PointData.h"
#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesSettings.h"

#include <QtCore>
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
	std::string jsonObject = "";
	std::string stringPoints;

	qDebug() << "ParallelCoordinatesPlugin: Prepare JSON string for data exchange";

	points.visitFromBeginToEnd([&jsonObject, &pointIDsGlobal, this](auto beginOfData, auto endOfData)
	{
		std::string currentPoint = "";
		// parse values of each point to JSON
		// for each point "{ "dimName0" : Val, "dimName1" : Vals, ...}
		for (const auto& pointId : pointIDsGlobal)
		{
			currentPoint = "{";
			for (unsigned int dimId = 0; dimId < _numDims; dimId++)
			{
				const auto index = pointId * _numDims + dimId;
				float attr = beginOfData[index];
				currentPoint.append("\"" + _dimNames[dimId].toStdString() + "\" : " + std::to_string(attr) + ",");
			}
			currentPoint.back() = '}';	// replace the last , with a closing brace

			jsonObject.append(currentPoint + ",");
		}
	});
	jsonObject.pop_back();	// remove last ,

	jsonObject = "[" + jsonObject + "]";

	//std::string jsonObject = "["
	//	"{\"A\" : 0, \"B\" : -0, \"C\" : 0, \"D\" : 0, \"E\" : 0, \"F\" : 3}, "
	//	"{\"A\" : 1, \"B\" : -1, \"C\" : 1, \"D\" : 2, \"E\" : 1, \"F\" : 6}, "
	//	"{\"A\" : 2, \"B\" : -2, \"C\" : 4, \"D\" : 4, \"E\" : 0.5, \"F\" : 2}, "
	//	"{\"A\" : 3, \"B\" : -3, \"C\" : 9, \"D\" : 6, \"E\" : 0.33, \"F\" : 4}, "
	//	"{\"A\" : 4, \"B\" : -4, \"C\" : 16, \"D\" : 8, \"E\" : 0.25, \"F\" : 9}"
	//	"]";

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
