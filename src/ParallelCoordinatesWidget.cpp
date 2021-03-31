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

void ParlCoorCommunicationObject::js_passDataNameToQt(QJsonValue name) {
    QString dataSetName = name.toString("___NODATASET___");
    if (!dataSetName.contains("\nPoints") || dataSetName == "___NODATASET___")
    {
        qDebug() << "Invalid drop into the parallel coordinates widget";
        return;
    }

    // data name from drop action ends in "\nPoints" which needs to be deleted 
    dataSetName = dataSetName.split("\n")[0];

    qDebug() << "ParallelCoordinatesPlugin: Load data set " << dataSetName;

    emit newDataSetName(dataSetName);
}

void ParlCoorCommunicationObject::newSelectionToJS(std::vector<unsigned int>& selectionIDs) {
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
    loaded(false), _parentPlugin(parentPlugin), _dropWidget(nullptr)
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

    _dropWidget = new DropWidget(this);

    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this, parentPlugin](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText             = mimeData->text();
        const auto tokens               = mimeText.split("\n");
        const auto datasetName          = tokens[0];
        const auto dataType             = DataType(tokens[1]);
        const auto dataTypes            = DataTypes({ PointType });
        const auto candidateDataset     = parentPlugin->getCore()->requestData<Points>(datasetName);
        const auto candidateDatasetName = candidateDataset.getName();

        if (!dataTypes.contains(dataType))
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", false);

        if (dataType == PointType) {
            const auto currentDatasetName   = parentPlugin->getCurrentDataSetName();
            const auto description          = QString("Visualize %1 as parallel coordinates").arg(candidateDatasetName);

            if (currentDatasetName.isEmpty()) {
                dropRegions << new DropWidget::DropRegion(this, "Points", description, true, [this, parentPlugin, candidateDatasetName]() {
                    parentPlugin->onDataInput(candidateDatasetName);
                    _dropWidget->setShowDropIndicator(false);
                });
            }
            else {
                if (candidateDatasetName == currentDatasetName) {
                    dropRegions << new DropWidget::DropRegion(this, "Warning", "Data already loaded", false);
                }
                else {
                    dropRegions << new DropWidget::DropRegion(this, "Points", description, true, [this, parentPlugin, candidateDatasetName]() {
                        parentPlugin->onDataInput(candidateDatasetName);
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
    loaded = true;
    qDebug() << "ParlCoorWidget: WebChannel bridge is available.";
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

void ParlCoorWidget::passSelectionToJS(std::vector<unsigned int>& selectionIDs)
{
    _communicationObject->newSelectionToJS(selectionIDs);
}
