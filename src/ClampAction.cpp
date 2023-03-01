#include "ClampAction.h"
#include "ParallelCoordinatesSettings.h"

#include <Application.h>

#include <QDebug>
#include <QLabel>
#include <QGroupBox>

using namespace hdps;
using namespace hdps::gui;

ClampAction::ClampAction(PCPSettings& parallelCoordinatesSettings) :
    WidgetAction(&parallelCoordinatesSettings),
    _parallelCoordinatesSettings(parallelCoordinatesSettings),
    _minClampAction(this, "Range Min. (%): ", 0, 100, 0, 0),
    _maxClampAction(this, "Range Max. (%): ", 0, 100, 100, 100),
    _applyAction(this, "Apply")
{
    setText("Clamp");
    setIcon(Application::getIconFont("FontAwesome").getIcon("ruler-horizontal"));

    _minClampAction.setDefaultWidgetFlags(IntegralAction::SpinBox | IntegralAction::Slider);
    _maxClampAction.setDefaultWidgetFlags(IntegralAction::SpinBox | IntegralAction::Slider);

    // connect the two sliders
    connect(&_minClampAction, &IntegralAction::valueChanged, this, &ClampAction::adjustMaxClamp);
    connect(&_maxClampAction, &IntegralAction::valueChanged, this, &ClampAction::adjustMinClamp);

    connect(&_applyAction, &TriggerAction::triggered, &_parallelCoordinatesSettings, &PCPSettings::onApplyClamping);

}

void ClampAction::adjustMaxClamp(int32_t val) {
    if (val > _maxClampAction.getValue())
        _maxClampAction.setValue(val);
}

void ClampAction::adjustMinClamp(int32_t val) {
    if (val < _minClampAction.getValue())
        _minClampAction.setValue(val);
}

ClampAction::Widget::Widget(QWidget* parent, ClampAction* clampAction) :
    WidgetActionWidget(parent, clampAction, State::Standard)
{
    auto layout = new QVBoxLayout();

    auto minClampWidget     = clampAction->getMinClampAction().createWidget(this);
    auto maxClampWidget     = clampAction->getMaxClampAction().createWidget(this);
    auto applyWidget        = clampAction->getApplyAction().createWidget(this);

    layout->addWidget(minClampWidget);
    layout->addWidget(maxClampWidget);
    layout->addWidget(applyWidget);

    setPopupLayout(layout);
}
