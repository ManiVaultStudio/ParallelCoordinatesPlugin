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

DimensionSelectionAction::Widget::Widget(QWidget* parent, DimensionSelectionAction* dimensionSelectionAction) :
    WidgetActionWidget(parent, dimensionSelectionAction),
    _groupsAction(this, "Dimension Selection")
{
    auto layout = new QVBoxLayout();

    auto dimSelectionWidget = dimensionSelectionAction->getDimensionsPickerAction().createWidget(this);
    auto applyWidget = dimensionSelectionAction->getApplyAction().createWidget(this);

    dimSelectionWidget->setFixedWidth(400);

    auto numPointsWidget = dimensionSelectionAction->getNumPointsAction().createWidget(this);
    auto numDimsWidget = dimensionSelectionAction->getNumDimsAction().createWidget(this);
    auto numItemsWidget = dimensionSelectionAction->getNumItemsAction().createWidget(this);

    layout->addWidget(new QLabel("Dimensions"));
    layout->addWidget(dimSelectionWidget);
    layout->addWidget(applyWidget);

    layout->addWidget(new QLabel("#Points:"));
    layout->addWidget(numPointsWidget);
    layout->addWidget(new QLabel("#Dimensions:"));
    layout->addWidget(numDimsWidget);
    layout->addWidget(new QLabel("#Items (max 7,000,000):"));
    layout->addWidget(numItemsWidget);

    setLayout(layout);
}
