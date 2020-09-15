#pragma once

#include <ViewPlugin.h>

using namespace hdps::plugin;


class ParallelCoordinatesPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
	ParallelCoordinatesPlugin() : ViewPlugin("Parallel Coordinates") { }
    ~ParallelCoordinatesPlugin(void) override;
    
    void init() override;
    
    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    hdps::DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;
};


// =============================================================================
// Factory
// =============================================================================

class ParallelCoordinatesPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ParallelCoordinatesPlugin"
                      FILE  "ParallelCoordinatesPlugin.json")
    
public:
	ParallelCoordinatesPluginFactory(void) {}
    ~ParallelCoordinatesPluginFactory(void) override {}
    
    ViewPlugin* produce() override;
};
