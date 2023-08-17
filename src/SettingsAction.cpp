#include "SettingsAction.h"

#include "ParallelCoordinatesPlugin.h"


using namespace hdps;
using namespace hdps::gui;


PCPSettings::PCPSettings(ParallelCoordinatesPlugin& parallelCoordinatesPlugin) :
    WidgetAction(&parallelCoordinatesPlugin, "Settings"),
    _pcpPlugin(parallelCoordinatesPlugin),
    _clampAction(*this),
    _dimensionSelectionAction(*this),
    _dataGUID(this, "DataGUIDAction")
{
    setText("Settings");
    setSerializationName("Settings");

    _clampAction.setSerializationName("ClampAction");
    _dimensionSelectionAction.setSerializationName("DimensionSelectionAction");
    _dataGUID.setSerializationName("DataGUIDAction");
}

void PCPSettings::onApplyClamping()
{
    _pcpPlugin.applyClamping();
}

void PCPSettings::onApplyDimensionFiltering()
{
    _pcpPlugin.applyDimensionSelection();
}

void PCPSettings::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _clampAction.fromParentVariantMap(variantMap);
    _dimensionSelectionAction.fromParentVariantMap(variantMap);
    _dataGUID.fromParentVariantMap(variantMap);

    //qDebug() << variantMap["DataGUIDAction"].toString();

    //qDebug() << _dataGUID.text();
}

QVariantMap PCPSettings::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _clampAction.insertIntoVariantMap(variantMap);
    _dimensionSelectionAction.insertIntoVariantMap(variantMap);
    _dataGUID.insertIntoVariantMap(variantMap);

    //qDebug() << _dataGUID.text();

    return variantMap;
}


PCPSettings::Widget::Widget(QWidget* parent, PCPSettings* settingsAction) :
    WidgetActionWidget(parent, settingsAction)
{
    setAutoFillBackground(true);

    auto layout = new QHBoxLayout();

    setLayout(layout);

    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(3);

    layout->addWidget(settingsAction->getClampAction().createCollapsedWidget(this));
    layout->addWidget(settingsAction->getDimensionSelectionAction().createCollapsedWidget(this));
    layout->addStretch(1);
}