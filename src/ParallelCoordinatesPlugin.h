#pragma once

#include <ViewPlugin.h>
 
#include <vector>

#include <QString>
#include <QStringList>

using namespace hdps::plugin;
using namespace hdps::util;
using namespace hdps::gui;

namespace hdps {
    namespace gui {
        class DropWidget;
    }
}

// =============================================================================
// View
// =============================================================================

class Points;

class PCPSettings;
class PCPWidget;

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
    void loadData(const hdps::Datasets& datasets) override;

    /**
     * Callback which is invoked when &Dataset<Points>::dataSelectionChanged is emitted
     */
    void onDataSelectionChanged();

    hdps::CoreInterface* getCore() { return _core;  }
    QString getCurrentDataSetName() const;
    QString getCurrentDataSetGuid() const;

    void applyClamping();
    void applyDimensionSelection();

private:
    // Parses data to JSON and passes it to the web widget
    void passDataToJS(const std::vector<unsigned int>& pointIDsGlobal);

    // informs the core about a selection 
    void publishSelection(const std::vector<unsigned int>& selectedIDs);

    /** Updates the window title (includes the name of the loaded dataset) */
    void updateWindowTitle();

    void calculateMinMaxPerDim();
    void calculateMinMaxClampPerDim();

public slots:
    // sets window title and calls passDataToJS in another thread
    void onDataInput();

signals:
    void dataSetChanged();

private:
    hdps::Dataset<Points>       _currentDataSet;
    std::vector<unsigned int>   _pointIDsGlobal;

    QStringList                 _dimNames;
    std::vector<bool>           _selectedDimensions;
    uint32_t                    _numDims;
    uint32_t                    _numSelectedDims;
    uint32_t                    _numPoints;

    std::vector<float>          _minMaxPerDim;         // [min_Ch0, max_Ch0, min_Ch1, max_Ch1, ...]
    std::vector<float>          _minMaxClampPerDim;    // [min_Ch0, max_Ch0, min_Ch1, max_Ch1, ...]
    int32_t                     _minClampPercent;
    int32_t                     _maxClampPercent;

    PCPWidget*                  _pcpWidget;
    PCPSettings*                _settingsWidget;
    hdps::gui::DropWidget*      _dropWidget;

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
    
    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    ViewPlugin* produce() override;

    /** Returns the data types that are supported by the example view plugin */
    hdps::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const hdps::Datasets& datasets) const override;
};
