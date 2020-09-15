#include "ParallelCoordinatesPlugin.h"

#include "PointData.h"
#include "ParallelCoordinatesWidget.h"

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
    // Widgets can be freely added to the view plugin via the addWidget() function.
	ParallelCoordinatesWidget* parallelCoordinatesWidget = new ParallelCoordinatesWidget();
    addWidget(parallelCoordinatesWidget);
}

/**
 * Callback which gets triggered when a dataset is added.
 * The name of the dataset which was added is given.
 * The added dataset can be retrieved through _core->requestSet(name);
 * and casting it to the appropriate high-level set.
 */
void ParallelCoordinatesPlugin::dataAdded(const QString name)
{
    const Points& addedSet = _core->requestData<Points>(name);
}

/**
 * Callback which gets triggered when a dataset changes.
 * The name of the dataset which was changed is given.
 * The changed dataset can be retrieved through _core->requestSet(name);
 * and casting it to the appropriate high-level set.
 */
void ParallelCoordinatesPlugin::dataChanged(const QString name)
{
    const Points& changedSet = _core->requestData<Points>(name);
}

/**
 * Callback which gets triggered when a dataset gets removed.
 * The name of the dataset which was removed is given.
 */
void ParallelCoordinatesPlugin::dataRemoved(const QString name)
{
    const Points& removedSet = dynamic_cast<const Points&>(_core->requestData(name));
}

/**
 * Callback which gets triggered when any plugin calls notifySelectionChanged().
 * The name of the data whose selection was changed is given and the new selection
 * can be retrieved through _core->requestSelection(dataName);
 */
void ParallelCoordinatesPlugin::selectionChanged(const QString dataName)
{
    const hdps::DataSet& selectionSet = _core->requestSelection(dataName);
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
