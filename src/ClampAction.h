#pragma once

#include <actions/WidgetAction.h>
#include <actions/IntegralAction.h>
#include <actions/TriggerAction.h>

using namespace mv::gui;

class PCPSettings;

class ClampAction : public WidgetAction
{
protected:

    class Widget : public WidgetActionWidget {
    public:
        Widget(QWidget* parent, ClampAction* ClampAction);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new ClampAction::Widget(parent, this);
    };

public:
    ClampAction(PCPSettings& PCPSettings);

    int32_t getMinClamp() { return _minClampAction.getValue(); };
    int32_t getMaxClamp() { return _maxClampAction.getValue(); };

private slots:
    void adjustMinClamp(int32_t val);
    void adjustMaxClamp(int32_t val);

public: // Action getters

    IntegralAction& getMinClampAction() { return _minClampAction; }
    IntegralAction& getMaxClampAction() { return _maxClampAction; }
    TriggerAction& getApplyAction() { return _applyAction; }

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

protected:
    PCPSettings&       _settingsAction;

    IntegralAction     _minClampAction;
    IntegralAction     _maxClampAction;
    TriggerAction      _applyAction;

};