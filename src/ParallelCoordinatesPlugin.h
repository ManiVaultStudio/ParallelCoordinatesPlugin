#pragma once

#include <ViewPlugin.h>
#include <widgets/DataSlot.h>

#include <QStringList>

using namespace hdps::plugin;
using namespace hdps::gui;

// =============================================================================
// View
// =============================================================================

//class ParallelCoordinatesSettings;
class ParallelCoordinatesWidget;

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

	ParallelCoordinatesWidget* _parallelCoordinatesWidget;

public slots:
	void onDataInput(QString dataSetName);

private:
	QString _currentDataSet;
	DataSlot* _dataSlot;
//	ParallelCoordinatesSettings* settings;

	unsigned int _numPoints;
	QStringList	_dimNames;

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
