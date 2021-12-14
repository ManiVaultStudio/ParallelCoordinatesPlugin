#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesPlugin.h"

#include <widgets/DropWidget.h>
#include <PointData.h>

#include <string>
#include <algorithm>
#include <vector>
#include <iterator>

#include <QMimeData>
#include <QResizeEvent>
#include <QWebEngineView>
#include <QDebug>

using namespace hdps;
using namespace hdps::gui;

// =============================================================================
// ParlCoorCommunicationObject
// =============================================================================

ParlCoorCommunicationObject::ParlCoorCommunicationObject(ParlCoorWidget* parent):
    _parent(parent)
{
}


void ParlCoorCommunicationObject::js_passSelectionToQt(QVariantList data){
    // convert data structure
    std::vector<unsigned int> selectedIDs;
    std::for_each(data.begin(), data.end(), [&selectedIDs](const auto &dat) {selectedIDs.push_back(dat.toUInt()); });
    // hand selction to core
    emit newSelectionToQt(selectedIDs);
}

void ParlCoorCommunicationObject::js_askForDataFromQt() {
    emit askForDataFromQt();
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
    emit this->qt_setSelectionInJS(selection);
}

// =============================================================================
// ParlCoorWidget
// =============================================================================

ParlCoorWidget::ParlCoorWidget(ParallelCoordinatesPlugin* parentPlugin):
    _parentPlugin(parentPlugin), _dropWidget(nullptr)
{
    setAcceptDrops(true);

    Q_INIT_RESOURCE(parcoords_resources);
    _communicationObject = new ParlCoorCommunicationObject(this);
    init(_communicationObject);

    layout()->setMargin(0);

    getView()->resize(size());
    getView()->setAcceptDrops(false);

    // re-emit the signal from the communication objection to the main plugin class where the selection is made public to the core
    connect(_communicationObject, &ParlCoorCommunicationObject::newSelectionToQt, [&](std::vector<unsigned int> selectedIDs) {emit newSelectionToQt(selectedIDs); });

    // re-emit the signal from the communication objection to the main plugin class: ask for new data after web view is loaded
    connect(_communicationObject, &ParlCoorCommunicationObject::askForDataFromQt, _parentPlugin, &ParallelCoordinatesPlugin::onDataInput);

    _dropWidget = new DropWidget(this);

    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this, parentPlugin](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText = mimeData->text();
        const auto tokens = mimeText.split("\n");

        if (tokens.count() == 1)
            return dropRegions;

        // Gather information to generate appropriate drop regions
        const auto datasetName = tokens[0];
        const auto datasetId = tokens[1];
        const auto dataType = DataType(tokens[2]);
        const auto dataTypes = DataTypes({ PointType });

        if (!dataTypes.contains(dataType)) {
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", "exclamation-circle", false);
        }
        else {

            const auto currentDatasetGuid   = parentPlugin->getCurrentDataSetGuid();
            const auto description          = QString("Visualize %1 as parallel coordinates").arg(datasetName);

            // if no data has been loaded, load the data
            if (currentDatasetGuid.isEmpty()) {
                dropRegions << new DropWidget::DropRegion(this, "Points", description, "map-marker-alt", true, [this, parentPlugin, datasetId]() {
                    parentPlugin->setData(datasetId);
                    _dropWidget->setShowDropIndicator(false);
                });
            }
            else {
                // if new data is dropped, load it
                if (datasetId == currentDatasetGuid) {
                    dropRegions << new DropWidget::DropRegion(this, "Warning", "Data already loaded", "exclamation-circle", false);
                }
                else {
                    dropRegions << new DropWidget::DropRegion(this, "Points", description, "map-marker-alt", true, [this, parentPlugin, datasetId]() {
                        parentPlugin->setData(datasetId);
                        _dropWidget->setShowDropIndicator(false);
                    });
                }
            }
        }

        return dropRegions;
    });
}

void ParlCoorWidget::resizeEvent(QResizeEvent * e) {
    getView()->resize(size());
}

void ParlCoorWidget::initWebPage()
{
    qDebug() << "ParlCoorWidget: WebChannel bridge is available.";
    emit _communicationObject->qt_triggerDataRequest();
}

void ParlCoorWidget::passDataToJS(QVariantList data)
{
    emit _communicationObject->qt_setDataInJS(data);
}

void ParlCoorWidget::enableBrushHighlight()
{
    emit _communicationObject->qt_enableBrushHighlight();
}

void ParlCoorWidget::disableBrushHighlight()
{
    emit _communicationObject->qt_disableBrushHighlight();
}

void ParlCoorWidget::passSelectionToJS(const std::vector<unsigned int>& selectionIDs)
{
    _communicationObject->newSelectionToJS(selectionIDs);
}

void ParlCoorWidget::setDropWidgetShowDropIndicator(bool show) 
{
    _dropWidget->setShowDropIndicator(show);
}
