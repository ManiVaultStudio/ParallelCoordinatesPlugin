#include "ParallelCoordinatesSettings.h"

#include "ParallelCoordinatesPlugin.h"


using namespace hdps;
using namespace hdps::gui;


PCPSettings::PCPSettings(ParallelCoordinatesPlugin& parallelCoordinatesPlugin) :
    WidgetAction(&parallelCoordinatesPlugin),
    _parentPlugin(parallelCoordinatesPlugin),
    _clampAction(*this)
    //    _dimensionSelectionAction(*this)
{

}

void PCPSettings::onApplyClamping()
{
    _parentPlugin.applyClamping();
}

void PCPSettings::onApplyDimensionFiltering()
{
    _parentPlugin.applyDimensionSelection();
}


PCPSettings::Widget::Widget(QWidget* parent, PCPSettings* settingsAction) :
    WidgetActionWidget(parent, settingsAction, State::Standard)
{
    setAutoFillBackground(true);

    auto layout = new QHBoxLayout();

    setLayout(layout);

    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(3);

    layout->addWidget(settingsAction->getClampAction().createCollapsedWidget(this));
//    layout->addWidget(settingsAction->getDimensionSelectionAction().createCollapsedWidget(this));
    layout->addStretch(1);
}