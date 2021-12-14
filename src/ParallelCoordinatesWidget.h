#pragma once 

#include "widgets/WebWidget.h"

#include <QWidget>
#include <QVariantList>
#include <QJsonValue>

class QResizeEvent;
class ParlCoorWidget;
class ParallelCoordinatesPlugin;

namespace hdps {
    namespace gui {
        class DropWidget;
    }
}
// =============================================================================
// ParlCoorCommunicationObject
// =============================================================================

class ParlCoorCommunicationObject : public hdps::gui::WebCommunicationObject
{
    Q_OBJECT
public:
    ParlCoorCommunicationObject(ParlCoorWidget* parent);

signals:
    // Signals to JS side
    void qt_setDataInJS(QVariantList data);
    void qt_setSelectionInJS(QVariantList selection);
    void qt_enableBrushHighlight();
    void qt_disableBrushHighlight();

    // Signal to Qt side
    void newSelectionToQt(std::vector<unsigned int>& selectionIDs);

public slots:
    // Invoked from JS side
    void js_passSelectionToQt(QVariantList data);

    // invoked from Qt side

    // converts vector to array string and emits qt_setSelectionInJS
    void newSelectionToJS(const std::vector<unsigned int>& selectionIDs);

private:
    ParlCoorWidget* _parent;
};


// =============================================================================
// ParlCoorWidget
// =============================================================================

class ParlCoorWidget : public hdps::gui::WebWidget
{
    Q_OBJECT
public:
    ParlCoorWidget(ParallelCoordinatesPlugin* parentPlugin);

    void passDataToJS(QVariantList);
    void passSelectionToJS(const std::vector<unsigned int>& selectionIDs);

    void enableBrushHighlight();
    void disableBrushHighlight();

    void setDropWidgetShowDropIndicator(bool);

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
    hdps::gui::DropWidget*          _dropWidget;

    /** Whether the web view has loaded and web-functions are ready to be called. */
    bool loaded;
};
