#pragma once

#include <actions/WidgetAction.h>
#include <actions/GroupsAction.h>
#include <actions/TriggerAction.h>
#include <actions/IntegralAction.h>

#include <PointData/DimensionsPickerAction.h>

using namespace mv;
using namespace mv::gui;

class PCPSettings;

class DimensionSelectionAction : public WidgetAction
{
protected:

    class Widget : public mv::gui::WidgetActionWidget {
    public:
        Widget(QWidget* parent, DimensionSelectionAction* layersAction);

    protected:
        GroupsAction    _groupsAction;
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:
    DimensionSelectionAction(PCPSettings& PCPSettings);

    void setNumPoints(int32_t num);
    void setNumDims(int32_t num);
    void setNumItems(int32_t num);

public: // Action getters

    DimensionsPickerAction& getDimensionsPickerAction() { return _dimensionAction; }
    TriggerAction& getApplyAction() { return _applyAction; }
    IntegralAction& getNumPointsAction() { return _numPointsAction; }
    IntegralAction& getNumDimsAction() { return _numDimsAction; }
    IntegralAction& getNumItemsAction() { return _numItemsAction; }

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
    PCPSettings&                _settingsAction;

    DimensionsPickerAction      _dimensionAction;
    TriggerAction               _applyAction;
    IntegralAction              _numPointsAction;
    IntegralAction              _numDimsAction;
    IntegralAction              _numItemsAction;

};