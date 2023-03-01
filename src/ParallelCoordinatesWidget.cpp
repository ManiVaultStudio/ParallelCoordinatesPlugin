#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesPlugin.h"

#include <PointData.h>

#include <QDebug>

using namespace hdps;
using namespace hdps::gui;

// =============================================================================
// ParlCoorCommunicationObject
// =============================================================================

ParlCoorCommunicationObject::ParlCoorCommunicationObject()
{
}


void ParlCoorCommunicationObject::js_passSelectionToQt(QVariantList data){
    // convert data structure
    _selectedIDsFromjs.clear();
    std::for_each(data.begin(), data.end(), [this](const auto &dat) {_selectedIDsFromjs.push_back(dat.toUInt()); });
    // hand selction to core
    emit newSelectionInPCP(_selectedIDsFromjs);
}

void ParlCoorCommunicationObject::newSelectionToJS(const std::vector<unsigned int>& selectionIDs) {
    QVariantList selection;

    // if nothing is selected, tell the parcoords to show all data
    if (selectionIDs.size() == 0)
    {
        selection.append("-");
    }
    // otherwise send all IDs
    else
    {
        for (const auto& ID : selectionIDs)
            selection.append(ID);
    }

    // send data to JS
    emit qt_setSelectionInJS(selection);
}

// =============================================================================
// PCPWidget
// =============================================================================

PCPWidget::PCPWidget(ParallelCoordinatesPlugin* pcpPlugin):
    _pcpPlugin(pcpPlugin)
{
    setAcceptDrops(true);   // drag & drop handled in ParallelCoordinatesPlugin.cpp

    Q_INIT_RESOURCE(parcoords_resources);
    _communicationObject = new ParlCoorCommunicationObject();
    init(_communicationObject);

    layout()->setContentsMargins(0, 0, 0, 0);

    // selection is made public to the core
    connect(_communicationObject, &ParlCoorCommunicationObject::newSelectionInPCP, this, [this](const std::vector<unsigned int>& selectionIDs) {
        _pcpPlugin->publishSelection(selectionIDs);
    });

}

void PCPWidget::initWebPage()
{
    qDebug() << "PCPWidget: WebChannel bridge is available.";
    
    // call to data load, used when plugin is opened via right-clicking data set
    _pcpPlugin->onDataInput();
}

void PCPWidget::passDataToJS(QVariantList data)
{
    emit _communicationObject->qt_setDataInJS(data);
}

void PCPWidget::enableBrushHighlight()
{
    emit _communicationObject->qt_enableBrushHighlight();
}

void PCPWidget::disableBrushHighlight()
{
    emit _communicationObject->qt_disableBrushHighlight();
}

void PCPWidget::passSelectionToJS(const std::vector<unsigned int>& selectionIDs)
{
    _communicationObject->newSelectionToJS(selectionIDs);
}
