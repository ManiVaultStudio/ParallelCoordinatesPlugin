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

void ParlCoorCommunicationObject::js_selectData(QString text)
{
	_parent->js_selectData(text);
}

void ParlCoorCommunicationObject::js_selectionUpdated(QVariant selectedClusters)
{
	_parent->js_selectionUpdated(selectedClusters);
}

void ParlCoorCommunicationObject::js_highlightUpdated(int highlightId)
{
	_parent->js_highlightUpdated(highlightId);
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

	if (selectedIDs.size() > 0)
		emit newSelection(selectedIDs);
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
	connect(_communicationObject, &ParlCoorCommunicationObject::newSelection, [&](std::vector<unsigned int> selectedIDs) {emit newSelection(selectedIDs); });
}

void ParlCoorWidget::resizeEvent(QResizeEvent * e) {
	getView()->resize(size());
}

void ParlCoorWidget::initWebPage()
{
	loaded = true;
	qDebug() << "ParlCoorWidget: WebChannel bridge is available.";
}

void ParlCoorWidget::passDataToJS(std::string _jsonObject)
{
	emit _communicationObject->qt_setData(QString(_jsonObject.c_str()));
}

void ParlCoorWidget::js_selectData(QString name)
{
}

void ParlCoorWidget::js_selectionUpdated(QVariant selectedClusters)
{
}

void ParlCoorWidget::js_highlightUpdated(int highlightId)
{
}

