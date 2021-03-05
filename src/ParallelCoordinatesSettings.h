#pragma once

#include <widgets/SettingsWidget.h>

#include <QWidget>
#include <QString>
#include <QLabel>

using namespace hdps::gui;

class ParlCoorSettings : public QWidget
{
    Q_OBJECT

public:
    ParlCoorSettings();

    void setCurrentDataSetName(QString datasetName) { currentDataSetName.setText(datasetName); };
    void setNumPoints(int num) { numPoints.setNum(num); };
    void setNumSel(int num) { numSel.setNum(num); };
    void setNumDims(int num) { numDims.setNum(num); };

private:
    // Drag and drop

    /**
     * Invoked when an item is dragged into the widget
     * and checks if the data type is okay to be used
     * @param dragEnterEvent Drag enter event
     */
    void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;

    /**
     * Invoked when an item is dropped onto the widget
     * and invokes futher data handling
     * @param dropEvent Drop event
     */
    void dropEvent(QDropEvent* dropEvent) override;

signals:
    void onDataInput(QString dataName);

public: 
    QLabel  currentDataSetName;
    QLabel  numPoints;
    QLabel  numSel;
    QLabel  numDims;
};
