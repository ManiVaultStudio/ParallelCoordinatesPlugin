#pragma once

#include <actions/WidgetAction.h>
#include <actions/IntegralAction.h>
#include <actions/TriggerAction.h>

using namespace hdps::gui;

class ParallelCoordinatesSettings;

class ClampAction : public WidgetAction
{
protected:

    class Widget : public hdps::gui::WidgetActionWidget {
    public:
        Widget(QWidget* parent, ClampAction* ClampAction);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new ClampAction::Widget(parent, this);
    };

public:
    ClampAction(ParallelCoordinatesSettings& ParallelCoordinatesSettings);

    int32_t getMinClamp() { return _minClampAction.getValue(); };
    int32_t getMaxClamp() { return _maxClampAction.getValue(); };

private slots:
    void adjustMinClamp(int32_t val);
    void adjustMaxClamp(int32_t val);

public: // Action getters

    IntegralAction& getMinClampAction() { return _minClampAction; }
    IntegralAction& getMaxClampAction() { return _maxClampAction; }
    TriggerAction& getApplyAction() { return _applyAction; }

protected:
    ParallelCoordinatesSettings&     _parallelCoordinatesSettings;

    IntegralAction     _minClampAction;
    IntegralAction     _maxClampAction;
    TriggerAction      _applyAction;

};