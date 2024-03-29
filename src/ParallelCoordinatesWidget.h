#pragma once 

#include "widgets/WebWidget.h"

#include <QVariantList>

class ParallelCoordinatesPlugin;

// =============================================================================
// ParlCoorCommunicationObject
// =============================================================================

class ParlCoorCommunicationObject : public mv::gui::WebCommunicationObject
{
    Q_OBJECT
public:
    ParlCoorCommunicationObject();

signals:
    // Signals to JS side
    void qt_setDataInJS(QVariantList data);
    void qt_setSelectionInJS(QVariantList selection);
    void qt_enableBrushHighlight();
    void qt_disableBrushHighlight();

    // Signals Qt internal
    void newSelectionInPCP(const std::vector<unsigned int>& selectionIDs);

public slots:
    // Invoked from JS side //
    void js_passSelectionToQt(QVariantList data);

    // converts vector to array string and emits qt_setSelectionInJS
    void newSelectionToJS(const std::vector<unsigned int>& selectionIDs); 

private:
    std::vector<unsigned int> _selectedIDsFromjs;

};


// =============================================================================
// PCPWidget
// =============================================================================

class PCPWidget : public mv::gui::WebWidget
{
    Q_OBJECT
public:
    PCPWidget(ParallelCoordinatesPlugin& pcpPlugin);

    void passDataToJS(QVariantList);
    void passSelectionToJS(const std::vector<unsigned int>& selectionIDs);

    void enableBrushHighlight();
    void disableBrushHighlight();

    ParlCoorCommunicationObject& getCommunicationObject() { return _communicationObject; };
    bool isInit() { return _isInit; };

private slots:
    /** Is invoked when the js side calls js_available of the WebCommunicationObject (ParlCoorCommunicationObject) 
        js_available emits notifyJsBridgeIsAvailable, which is conencted to initWebPage in WebWidget.cpp*/
    void initWebPage() override;

private:
    ParallelCoordinatesPlugin&      _pcpPlugin;
    ParlCoorCommunicationObject     _communicationObject;
    bool                            _isInit;
};
