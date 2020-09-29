#include "ParallelCoordinatesWidget.h"
#include "ParallelCoordinatesPlugin.h"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include <QVariant>
#include <QString>
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


void ParlCoorCommunicationObject::js_passSelectionToQt(QString data) {
	std::vector<unsigned int> selectedIDs;
	char del = ',';

	// convert the string of IDs "1,2,3..." to a vector of unsigned ints
	std::istringstream iss(data.toStdString());
	std::string IDstring;
	auto IdIter = std::back_inserter(selectedIDs);
	while (std::getline(iss, IDstring, del)) {
		*IdIter++ = std::stoul(IDstring);
	}
	emit newSelectionToQt(selectedIDs);
}

void ParlCoorCommunicationObject::newSelectionToJS(std::vector<unsigned int>& selectionIDs) {
	// if nothing is selected, tell the parcoords to show all data
	if (selectionIDs.size() == 0)
	{
		emit this->qt_setSelectionInJS("-");
		return;
	}

	// parse all IDs into a string
	QString selection = "";
	for (const auto& ID : selectionIDs) {
		selection.append(QString::fromStdString(std::to_string(ID) + ","));
	}
	selection.chop(1);	// remove last ","

	// send string array to JS
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
