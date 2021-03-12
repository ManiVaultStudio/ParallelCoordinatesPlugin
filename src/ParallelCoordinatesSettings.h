#pragma once

#include <widgets/SettingsWidget.h>

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QComboBox>
#include <QPushButton>

using namespace hdps::gui;

class ParallelCoordinatesPlugin;

class ParlCoorSettings : public QWidget
{
    Q_OBJECT

public:
    ParlCoorSettings(ParallelCoordinatesPlugin* parent);

    void setDimensionNames(QStringList dimNames);
    QStringList getSelectedDimensionNames();
    std::vector<bool> getSelectedDimensions();

    void setNumPoints(int num) { _numPoints.setNum(num); };
    void setNumSel(int num) { _numSel.setNum(num); };
    void setNumDims(int num) { _numDims.setNum(num); };

private:
    ParallelCoordinatesPlugin* _parentPlugin;

    QLabel _numPoints;
    QLabel _numSel;
    QLabel _numDims;

    QStringList _dimNames;

    // For ComboBoxOfCheckBoxes, see http://programmingexamples.net/wiki/Qt/ModelView/ComboBoxOfCheckBoxes
    QStandardItemModel* _comboBoxLineModel;
    std::vector<QStandardItem*> _comboBoxItems;
    QComboBox* _comboBox;

    QPushButton* _applyDimsButton;

};
