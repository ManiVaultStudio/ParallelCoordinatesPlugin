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

    void setNumPoints(int num) { numPoints.setNum(num); };
    void setNumSel(int num) { numSel.setNum(num); };
    void setNumDims(int num) { numDims.setNum(num); };

signals:
    void onDataInput(QString dataName);

public: 
    QLabel  numPoints;
    QLabel  numSel;
    QLabel  numDims;
};
