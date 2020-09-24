#pragma once

#include <ViewPlugin.h>

#include <vector>

#include <QString>
#include <QStringList>
#include <QThread>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ParlCoorSettings;
class ParlCoorWidget;
class Points;

class ParallelCoordinatesPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
	ParallelCoordinatesPlugin();
    ~ParallelCoordinatesPlugin(void) override;
    
    void init() override;
    
    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    hdps::DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;


private:
	// Parses data to JSON and passes it to the web widget
	void passDataToJS(const QString dataSetName);

	// informs the core about a selection 
	void publishSelection(std::vector<unsigned int> selectedIDs);

public slots:
	// sets window title and calls passDataToJS in another thread
	void onDataInput(const QString dataSetName);

private:
	QString _currentDataSetName;
	Points* _currentDataSet;

	QStringList _dimNames;
	unsigned int _numDims;
	unsigned int _numPoints;

	ParlCoorWidget* _parCoordWidget;
	ParlCoorSettings* _settingsWidget;

	QThread thread;

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
