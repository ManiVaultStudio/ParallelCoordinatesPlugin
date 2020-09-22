#include "ParallelCoordinatesPlugin.h"

#include "PointData.h"
#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesSettings.h"

#include <QtCore>
#include <QtDebug>

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

	//// get data set from core
	//const Points& points = _core->requestData<Points>(dataSetName);
	//_dimNames = QStringList(points.getDimensionNames().begin(), points.getDimensionNames().end());
	//_numDims = points.getNumDimensions();
	//_numPoints = points.getNumPoints();

	//// write data to json string
	//std::string jsonObject = "";
	//qDebug() << "ParallelCoordinatesPlugin: Prepare JSON string for data exchange";

	//unsigned int numDims = points.getNumDimensions();
	//points.visitFromBeginToEnd([&points, &numDims](auto beginOfData, auto endOfData)
	//{
	//	for (unsigned int dimensionId = 0; dimensionId < numDims; dimensionId++)
	//	{
	//		// add dimension to JSON

	//		for (const auto& pointId : points.indices)
	//		{
	//			const auto index = pointId * numDims + dimensionId;
	//			// add values of all points of one dimension to JSON

	//		}
	//	}
	//});

	std::string jsonObject = "{ \n "
		"\"0\" : [0, -0, 0, 0, 0, 3], \n"
		"\"1\" : [1, -1, 1, 2, 1, 6], \n"
		"\"2\" : [2, -2, 4, 4, 0.5, 2], \n"
		"\"3\" : [3, -3, 9, 6, 0.33, 4], \n"
		"\"4\" : [4, -4, 16, 8, 0.25, 9] \n"
		"}";

	_parCoordWidget->passDataToJS(jsonObject);

//	// TODO: pass points and names to the d3 widget
//	//if (_dimNames.size() == points.getNumDimensions())
//	//{
//	//	//settings->initDimOptions(points.getDimensionNames());
//	//	//settings->initScalarDimOptions(DataSet::getSourceData(points).getDimensionNames());
//	//}
//	//else
//	//{
//	//	//settings->initDimOptions(points.getNumDimensions());
//	//	//settings->initScalarDimOptions(DataSet::getSourceData(points).getNumDimensions());
//	//}
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
