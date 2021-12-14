#pragma once

#include <ViewPlugin.h>
#include <Dataset.h>
 
#include <vector>

#include <QString>
#include <QStringList>

using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

// =============================================================================
// View
// =============================================================================

class Points;

class ParlCoorSettings;
class ParlCoorWidget;

class ParallelCoordinatesPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ParallelCoordinatesPlugin(const PluginFactory* factory);
    ~ParallelCoordinatesPlugin() override;
    
    void init() override;
    
    /**
     * Load one (or more datasets in the view)
     * @param datasets Dataset(s) to load
     */
    void loadData(const QVector<hdps::Dataset<hdps::DatasetImpl>>& datasets) override;

    void setData(QString newDatasetGuid);

    /**
     * Callback which is invoked when a \p dataEvent occurs
     * @param dataEvent Data event that occurred
     */
    void onDataEvent(hdps::DataEvent* dataEvent);

    hdps::CoreInterface* getCore() { return _core;  }
    QString getCurrentDataSetName() const;
    QString getCurrentDataSetGuid() const;

private:
    // Parses data to JSON and passes it to the web widget
    void passDataToJS(const std::vector<unsigned int>& pointIDsGlobal);

    // informs the core about a selection 
    void publishSelection(std::vector<unsigned int> selectedIDs);

    // Sets the html page in the main viewer widger
    void initMainView();

    /** Updates the window title (includes the name of the loaded dataset) */
    void updateWindowTitle();

    void calculateMinMaxPerDim();


public slots:
    // sets window title and calls passDataToJS in another thread
    void onDataInput();

    // update selected dimensions
    void onApplySettings();

    // calls initMainView
    void onRefreshMainView();

    void minDimClampChanged(int min);
    void maxDimClampChanged(int max);
    void calculateMinMaxClampPerDim();

signals:
    void dataSetChanged(QString datasetGuid);

private:
    hdps::Dataset<Points> _currentDataSet;
    std::vector<unsigned int> _pointIDsGlobal;

    QStringList _dimNames;
    std::vector<bool> _selectedDimensions;
    unsigned int _numDims;
    unsigned int _numSelectedDims;
    unsigned int _numPoints;

    std::vector<float> _minMaxPerDim;    // [min_Ch0, max_Ch0, min_Ch1, max_Ch1, ...]
    std::vector<float> _minMaxClampPerDim;    // [min_Ch0, max_Ch0, min_Ch1, max_Ch1, ...]
    int _minClampPercent;
    int _maxClampPercent;

    ParlCoorWidget* _parCoordWidget;
    ParlCoorSettings* _settingsWidget;

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

    /** Returns the data types that are supported by the example view plugin */
    hdps::DataTypes supportedDataTypes() const override;
};
