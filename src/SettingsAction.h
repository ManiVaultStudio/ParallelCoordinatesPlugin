#pragma once

#include <actions/WidgetAction.h>

#include "ClampAction.h"
#include "DimensionSelectionAction.h"

#include <QLabel>

using namespace hdps;

class ParallelCoordinatesPlugin;

class PCPSettings : public WidgetAction
{
protected:

    class Widget : public hdps::gui::WidgetActionWidget {
    public:
        Widget(QWidget* parent, PCPSettings* settingsAction);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new PCPSettings::Widget(parent, this);
    };

public:
    PCPSettings(ParallelCoordinatesPlugin& parallelCoordinatesPlugin);


private slots:

    void onLoadDims();

public slots:
    void onApplyClamping();
    void onApplyDimensionFiltering();

public: // Action getters

    ClampAction& getClampAction() { return _clampAction; }
    DimensionSelectionAction& getDimensionSelectionAction() { return _dimensionSelectionAction; }
    QLabel& getDataNameLabel() { return _dataName; }
    StringAction& getDataGUIDAction() { return _dataGUID; }

public: // Serialization

    /**
     * Load plugin from variant map
     * @param Variant map representation of the plugin
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save plugin to variant map
     * @return Variant map representation of the plugin
     */
    QVariantMap toVariantMap() const override;

private:
    ParallelCoordinatesPlugin&  _pcpPlugin;
    ClampAction                 _clampAction;
    DimensionSelectionAction    _dimensionSelectionAction;
    QLabel                      _dataName;
    StringAction                _dataGUID;      // internal for serialization
};
