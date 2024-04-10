#include "DimensionSelectionAction.h"
#include "SettingsAction.h"

#include <Application.h>

#include <QString>
#include <QLabel>
#include <QVBoxLayout>

DimensionSelectionAction::DimensionSelectionAction(PCPSettings& settingsAction) :
    WidgetAction(&settingsAction, "Dimension Selection"),
    _settingsAction(settingsAction),
    _applyAction(this, "Apply"),
    _numPointsAction(this, "Number of points"),
    _numDimsAction(this, "Number of dimensions"),
    _numItemsAction(this, "Number of total items"),
    _dimensionAction(this, "Dimension Picker")
{
    setText("Dimension Selection");
    setSerializationName("Dimension Selection");
    setIcon(Application::getIconFont("FontAwesome").getIcon("layer-group"));

    _numPointsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numDimsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numItemsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);

    _numPointsAction.initialize(0, 7'000'000, 1);
    _numDimsAction.initialize(0, 7'000'000, 1);
    _numItemsAction.initialize(0, 7'000'000, 1);

    _numPointsAction.setEnabled(false);
    _numDimsAction.setEnabled(false);
    _numItemsAction.setEnabled(false);

    _numItemsAction.setToolTip("#points * #dims must be smaller than 7,000,000");

    _dimensionAction.setSerializationName("DimensionAction");
    _applyAction.setSerializationName("ApplyAction");
    _numPointsAction.setSerializationName("NumPointsAction");
    _numDimsAction.setSerializationName("NumDimsAction");
    _numItemsAction.setSerializationName("NumItemsAction");

    // only show selected dimensions
    connect(&_applyAction, &TriggerAction::triggered, &_settingsAction, &PCPSettings::onApplyDimensionFiltering);
}

void DimensionSelectionAction::setNumPoints(int32_t num) {
    _numPointsAction.setValue(num);
}

void DimensionSelectionAction::setNumDims(int32_t num) {
    _numDimsAction.setValue(num);
}

void DimensionSelectionAction::setNumItems(int32_t num) {
    _numItemsAction.setValue(num);
}

void DimensionSelectionAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _dimensionAction.fromParentVariantMap(variantMap);
    _applyAction.fromParentVariantMap(variantMap);
    _numPointsAction.fromParentVariantMap(variantMap);
    _numDimsAction.fromParentVariantMap(variantMap);
    _numItemsAction.fromParentVariantMap(variantMap);
}

QVariantMap DimensionSelectionAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _dimensionAction.insertIntoVariantMap(variantMap);
    _applyAction.insertIntoVariantMap(variantMap);
    _numPointsAction.insertIntoVariantMap(variantMap);
    _numDimsAction.insertIntoVariantMap(variantMap);
    _numItemsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

DimensionSelectionAction::Widget::Widget(QWidget* parent, DimensionSelectionAction* dimensionSelectionAction) :
    WidgetActionWidget(parent, dimensionSelectionAction),
    _groupsAction(this, "Dimension Selection")
{
    auto layout = new QGridLayout();

    layout->addWidget(dimensionSelectionAction->getDimensionsPickerAction().createWidget(this), 0, 0, 1, 2);
    layout->addWidget(dimensionSelectionAction->getApplyAction().createWidget(this),            1, 0, 1, 2);

    layout->addWidget(dimensionSelectionAction->getNumPointsAction().createLabelWidget(this),   2, 0);
    layout->addWidget(dimensionSelectionAction->getNumPointsAction().createWidget(this),        2, 1);
    layout->addWidget(dimensionSelectionAction->getNumDimsAction().createLabelWidget(this),     3, 0);
    layout->addWidget(dimensionSelectionAction->getNumDimsAction().createWidget(this),          3, 1);
    layout->addWidget(dimensionSelectionAction->getNumItemsAction().createLabelWidget(this),    4, 0);
    layout->addWidget(dimensionSelectionAction->getNumItemsAction().createWidget(this),         4, 1);

    setLayout(layout);
}
