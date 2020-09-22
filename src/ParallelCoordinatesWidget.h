#pragma once 

#include "widgets/WebWidget.h"

#include <QWidget>
#include <QString>

class QResizeEvent;
class ParlCoorWidget;

// 
class ParlCoorCommunicationObject : public hdps::gui::WebCommunicationObject
{
	Q_OBJECT
public:
	ParlCoorCommunicationObject(ParlCoorWidget* parent);

signals:
	void qt_setData(QString data);
	void qt_addAvailableData(QString name);
	void qt_setSelection(QList<int> selection);
	void qt_setHighlight(int highlightId);
	void qt_setMarkerSelection(QList<int> selection);

public slots:
	void js_selectData(QString text);
	void js_selectionUpdated(QVariant selectedClusters);
	void js_highlightUpdated(int highlightId);

private:
	ParlCoorWidget* _parent;
};


// 
class ParlCoorWidget : public hdps::gui::WebWidget
{
	Q_OBJECT
public:
	ParlCoorWidget();

	void passDataToJS(std::string _jsonObject);

	void js_selectData(QString text);
	void js_selectionUpdated(QVariant selectedClusters);
	void js_highlightUpdated(int highlightId);

protected:
	void resizeEvent(QResizeEvent * e) override;

signals:

private slots:
	/** Is invoked when the js side calls js_available of the WebCommunicationObject (ParlCoorCommunicationObject) 
		js_available emits notifyJsBridgeIsAvailable, which is conencted to initWebPage in WebWidget.cpp*/
	void initWebPage() override;

private:
	ParlCoorCommunicationObject*  _communicationObject;

	/** Whether the web view has loaded and web-functions are ready to be called. */
	bool loaded;
};
