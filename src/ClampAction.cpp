#include "ClampAction.h"
#include "SettingsAction.h"

#include <Application.h>

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>

using namespace hdps;
using namespace hdps::gui;

ClampAction::ClampAction(PCPSettings& settingsAction) :
    WidgetAction(&settingsAction, "Clamp"),
    _settingsAction(settingsAction),
    _minClampAction(this, "Range Min. (%): ", 0, 100, 0),
    _maxClampAction(this, "Range Max. (%): ", 0, 100, 100),
    _applyAction(this, "Apply")
{
    setText("Clamp");
    setSerializationName("Clamp");
    setIcon(Application::getIconFont("FontAwesome").getIcon("ruler-horizontal"));

    _minClampAction.setDefaultWidgetFlags(IntegralAction::SpinBox | IntegralAction::Slider);
    _maxClampAction.setDefaultWidgetFlags(IntegralAction::SpinBox | IntegralAction::Slider);

    _minClampAction.setSerializationName("MinClamp");
    _maxClampAction.setSerializationName("MaxClamp");
    _applyAction.setSerializationName("ApplyAction");

    // connect the two sliders
    connect(&_minClampAction, &IntegralAction::valueChanged, this, &ClampAction::adjustMaxClamp);
    connect(&_maxClampAction, &IntegralAction::valueChanged, this, &ClampAction::adjustMinClamp);

    connect(&_applyAction, &TriggerAction::triggered, &_settingsAction, &PCPSettings::onApplyClamping);

}

void ClampAction::adjustMaxClamp(int32_t val) {
    if (val > _maxClampAction.getValue())
        _maxClampAction.setValue(val);
}

void ClampAction::adjustMinClamp(int32_t val) {
    if (val < _minClampAction.getValue())
        _minClampAction.setValue(val);
}

void ClampAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _minClampAction.fromParentVariantMap(variantMap);
    _maxClampAction.fromParentVariantMap(variantMap);
    _applyAction.fromParentVariantMap(variantMap);
}

QVariantMap ClampAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _minClampAction.insertIntoVariantMap(variantMap);
    _maxClampAction.insertIntoVariantMap(variantMap);
    _applyAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ClampAction::Widget::Widget(QWidget* parent, ClampAction* clampAction) :
    WidgetActionWidget(parent, clampAction)
{
    auto layout = new QVBoxLayout();

    auto minClampWidget     = clampAction->getMinClampAction().createWidget(this);
    auto maxClampWidget     = clampAction->getMaxClampAction().createWidget(this);
    auto applyWidget        = clampAction->getApplyAction().createWidget(this);

    layout->addWidget(minClampWidget);
    layout->addWidget(maxClampWidget);
    layout->addWidget(applyWidget);

    setLayout(layout);
}
