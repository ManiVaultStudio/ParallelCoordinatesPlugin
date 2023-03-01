#pragma once 

#include "widgets/WebWidget.h"

#include <QWidget>
#include <QVariantList>
#include <QJsonValue>

class QResizeEvent;
class PCPWidget;
class ParallelCoordinatesPlugin;

// =============================================================================
// ParlCoorCommunicationObject
// =============================================================================

class ParlCoorCommunicationObject : public hdps::gui::WebCommunicationObject
{
    Q_OBJECT
public:
    ParlCoorCommunicationObject(PCPWidget* parent);

signals:
    // Signals to JS side
    void qt_setDataInJS(QVariantList data);
    void qt_setSelectionInJS(QVariantList selection);
    void qt_enableBrushHighlight();
    void qt_disableBrushHighlight();

    // to make sure that the web view is loaded: js->notifyBridgeAvailable triggers PCPWidget::initWebPage emits qt_triggerDataRequest which in turn lets the js side call askForDataFromQt back here 
    void qt_triggerDataRequest();

    // Signal to Qt side
    void newSelectionToQt(std::vector<unsigned int>& selectionIDs);

    // Signals from qt to qt
    void askForDataFromQt();

public slots:
    // Invoked from JS side //
    void js_passSelectionToQt(QVariantList data);

    // to make sure that the web view is loaded: js->notifyBridgeAvailable triggers PCPWidget::initWebPage emits qt_triggerDataRequest which in turn lets the js side call askForDataFromQt back here 
    void js_askForDataFromQt();

    // invoked from Qt side //

    // converts vector to array string and emits qt_setSelectionInJS
    void newSelectionToJS(const std::vector<unsigned int>& selectionIDs);

private:
    PCPWidget* _parent;
};


// =============================================================================
// PCPWidget
// =============================================================================

class PCPWidget : public hdps::gui::WebWidget
{
    Q_OBJECT
public:
    PCPWidget(ParallelCoordinatesPlugin* parentPlugin);

    void passDataToJS(QVariantList);
    void passSelectionToJS(const std::vector<unsigned int>& selectionIDs);

    void enableBrushHighlight();
    void disableBrushHighlight();

protected:
    void resizeEvent(QResizeEvent * e) override;

signals:
    void newSelectionToQt(std::vector<unsigned int> selectionIDs);

private slots:
    /** Is invoked when the js side calls js_available of the WebCommunicationObject (ParlCoorCommunicationObject) 
        js_available emits notifyJsBridgeIsAvailable, which is conencted to initWebPage in WebWidget.cpp*/
    void initWebPage() override;

private:
    ParlCoorCommunicationObject*    _communicationObject;
    ParallelCoordinatesPlugin*      _parentPlugin;

};
