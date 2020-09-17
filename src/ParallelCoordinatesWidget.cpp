#include "ParallelCoordinatesWidget.h"

#include <QVariant>
#include <QString>
#include <QResizeEvent>
#include <QWebEngineView>
#include <QDebug>

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


ParlCoorWidget::ParlCoorWidget():
	loaded(false)
{
	_communicationObject = new ParlCoorCommunicationObject(this);
	init(_communicationObject);

	// TODO: adaptive resize of html/d3 content
	getView()->resize(size());
}

void ParlCoorWidget::resizeEvent(QResizeEvent * e) {
	getView()->resize(size());
}

void ParlCoorWidget::initWebPage()
{
	loaded = true;

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
