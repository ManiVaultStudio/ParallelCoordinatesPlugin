#pragma once

#include <actions/WidgetAction.h>

#include "ClampAction.h"
//#include "DimensionSelectionAction.h"

using namespace hdps;

class ParallelCoordinatesPlugin;

class ParallelCoordinatesSettings : public WidgetAction
{
protected:

    class Widget : public hdps::gui::WidgetActionWidget {
    public:
        Widget(QWidget* parent, ParallelCoordinatesSettings* settingsAction);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new ParallelCoordinatesSettings::Widget(parent, this);
    };

public:
    ParallelCoordinatesSettings(ParallelCoordinatesPlugin& parallelCoordinatesPlugin);


private slots:

    void onLoadDims();

public slots:
    void onApplyClamping();
    void onApplyDimensionFiltering();

public: // Action getters

    ClampAction& getClampAction() { return _clampAction; }
//    DimensionSelectionAction& getDimensionSelectionAction() { return _dimensionSelectionAction; }

private:
    ParallelCoordinatesPlugin&  _parentPlugin;
    ClampAction                 _clampAction;
//    DimensionSelectionAction    _dimensionSelectionAction;
};
