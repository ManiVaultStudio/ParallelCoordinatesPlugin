#include "DimensionSelectionAction.h"
#include "ParallelCoordinatesSettings.h"
#include "ParallelCoordinatesPlugin.h"

#include <Application.h>

#include <QString>
#include <QLabel>

DimensionSelectionAction::DimensionSelectionAction(PCPSettings& parallelCoordinatesSettings) :
    WidgetAction(&parallelCoordinatesSettings),
    _parallelCoordinatesSettings(parallelCoordinatesSettings),
    _applyAction(this, "Apply"),
    _numPointsAction(this, "Number of points"),
    _numDimsAction(this, "Number of dimensions"),
    _numItemsAction(this, "Number of total items"),
    _dimensionAction(this)
{
    setText("Dimension Selection");
    setIcon(Application::getIconFont("FontAwesome").getIcon("layer-group"));

    _numPointsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numDimsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numItemsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);

    _numPointsAction.initialize(0, 7'000'000, 1, 1);
    _numDimsAction.initialize(0, 7'000'000, 1, 1);
    _numItemsAction.initialize(0, 7'000'000, 1, 1);

    _numPointsAction.setEnabled(false);
    _numDimsAction.setEnabled(false);
    _numItemsAction.setEnabled(false);

    _numItemsAction.setToolTip("#points * #dims must be smaller than 7,000,000");

    // only show selected dimensions
    connect(&_applyAction, &TriggerAction::triggered, &_parallelCoordinatesSettings, &PCPSettings::onApplyDimensionFiltering);
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
    WidgetActionWidget(parent, dimensionSelectionAction)
{
    auto layout = new QVBoxLayout();

    auto dimSelectionWidget = dimensionSelectionAction->getDimensionsPickerAction().createWidget(this);
    auto applyWidget = dimensionSelectionAction->getApplyAction().createWidget(this);

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

    setPopupLayout(layout);
}
