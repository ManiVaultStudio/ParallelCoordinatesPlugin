#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesPlugin.h"

#include <string>
#include <algorithm>
#include <vector>
#include <iterator>

#include <QResizeEvent>
#include <QWebEngineView>
#include <QDebug>

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
	loaded(false), _parentPlugin(parentPlugin)
{
	Q_INIT_RESOURCE(parcoords_resources);
	_communicationObject = new ParlCoorCommunicationObject(this);
	init(_communicationObject);

	// TODO: adaptive resize of html/d3 content
	getView()->resize(size());

	// re-emit the signal from the communication objection to the main plugin class where the selection is made public to the core
	connect(_communicationObject, &ParlCoorCommunicationObject::newSelectionToQt, [&](std::vector<unsigned int> selectedIDs) {emit newSelectionToQt(selectedIDs); });
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
