#include "ParallelCoordinatesSettings.h"

#include "ParallelCoordinatesPlugin.h"


using namespace hdps;
using namespace hdps::gui;


ParallelCoordinatesSettings::ParallelCoordinatesSettings(ParallelCoordinatesPlugin& parallelCoordinatesPlugin) :
    WidgetAction(&parallelCoordinatesPlugin),
    _parentPlugin(parallelCoordinatesPlugin),
    _clampAction(*this)
    //    _dimensionSelectionAction(*this)
{

}

void ParallelCoordinatesSettings::onApplyClamping()
{
    _parentPlugin.applyClamping();
}

void ParallelCoordinatesSettings::onApplyDimensionFiltering()
{
    _parentPlugin.applyDimensionSelection();
}


ParallelCoordinatesSettings::Widget::Widget(QWidget* parent, ParallelCoordinatesSettings* settingsAction) :
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