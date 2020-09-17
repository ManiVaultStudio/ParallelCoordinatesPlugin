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
	_parallelCoordinatesWidget = new ParallelCoordinatesWidget();
    addWidget(_parallelCoordinatesWidget);

	//
	_settingsWidget = new ParallelCoordinatesSettings();
	addWidget(_settingsWidget);

	//
	connect(_settingsWidget, &ParallelCoordinatesSettings::onDataInput, this, &ParallelCoordinatesPlugin::onDataInput);
}

void ParallelCoordinatesPlugin::onDataInput(QString dataSetName)
{
	_currentDataSet = dataSetName;

	setWindowTitle(_currentDataSet);
//
//	const Points& points = _core->requestData<Points>(_currentDataSet);
//
//	//_dimNames = QStringList(points.getDimensionNames().begin(), points.getDimensionNames().end());
//	//_numPoints = points.getNumPoints();
//
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
