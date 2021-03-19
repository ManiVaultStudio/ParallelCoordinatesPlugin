#pragma once

#include <widgets/SettingsWidget.h>

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>

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

    int getMinClamp() { return _minClamp->value(); };
    int getMaxClamp() { return _maxClamp->value(); };

private slots:
    void adjustMinClamp(int val);
    void adjustMaxClamp(int val);

    void onLoadDims();

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
    QPushButton* _loadDimsButton;

    // Clamp axis sliders
    QSlider* _minClamp;
    QSlider* _maxClamp;

    QLabel* _minClampValLabel;
    QLabel* _maxClampValLabel;

    bool disableAllDimensions();
    bool tryToEnableDimensionByName(QString name);
};
