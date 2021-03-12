#include "ParallelCoordinatesSettings.h"

#include "ParallelCoordinatesPlugin.h"

#include <QMimeData>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>

#include <assert.h> // for NDEBUG

#if !defined(NDEBUG)
    #include <QPushButton> 
#endif NDEBUG

ParlCoorSettings::ParlCoorSettings(ParallelCoordinatesPlugin* parent): _parentPlugin(parent){
    // layout structure
    setFixedHeight(40);
    auto* settingsLayout = new QGridLayout();
    settingsLayout->setColumnStretch(0, 5);
    settingsLayout->setColumnStretch(1, 200);
    
    // combobox of checkboxes
    QLabel* comboBoxLabel = new QLabel("Active Dimensions:");
    QComboBox* _comboBox = new QComboBox();
    _comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboBox->setMinimumWidth(100);
    _comboBox->setMaximumWidth(300);
    _comboBoxLineModel = new QStandardItemModel();
    _comboBox->setModel(_comboBoxLineModel);
    _comboBox->setStyleSheet("QComboBox QAbstractItemView{min-height: 250px;}");
    _applyDimsButton = new QPushButton("Apply");

    // read only displays

    // number of points
    QLabel* numPointsLabel = new QLabel("Num. points:");
    _numPoints.setFixedWidth(50);
    _numPoints.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // (number of) selected points 
    QLabel* numSelLabel = new QLabel("Selection size:");
    _numSel.setFixedWidth(50);
    _numSel.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // number of dims
    QLabel* numDimsLabel = new QLabel("Num. dims.:");
    _numDims.setFixedWidth(50);
    _numDims.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // only show selected dimensions
    connect(_applyDimsButton, &QPushButton::released, _parentPlugin, &ParallelCoordinatesPlugin::onDimensionSelectionChanged);

    // add elements to gui
    settingsLayout->addWidget(numPointsLabel, 0, 1, Qt::AlignRight);
    settingsLayout->addWidget(&_numPoints, 0, 2, Qt::AlignRight);
    settingsLayout->addWidget(numSelLabel, 0, 3, Qt::AlignRight);
    settingsLayout->addWidget(&_numSel, 0, 4, Qt::AlignRight);
    settingsLayout->addWidget(numDimsLabel, 0, 5, Qt::AlignRight);
    settingsLayout->addWidget(&_numDims, 0, 6, Qt::AlignRight);
    settingsLayout->addWidget(comboBoxLabel, 0, 7, Qt::AlignRight);
    settingsLayout->addWidget(_comboBox, 0, 8, Qt::AlignRight);
    settingsLayout->addWidget(_applyDimsButton, 0, 9, Qt::AlignRight);

    // button to refresh html page
#if !defined(NDEBUG)
    QPushButton* refreshButton = new QPushButton("Refresh HTML", this);
    connect(refreshButton, &QPushButton::released, _parentPlugin, &ParallelCoordinatesPlugin::onRefreshMainView);

    settingsLayout->addWidget(numPointsLabel, 0, 10, Qt::AlignRight);
#endif NDEBUG

    setLayout(settingsLayout);
}

void ParlCoorSettings::setDimensionNames(QStringList dimNames) {
    _dimNames = dimNames;
    int numDims = _dimNames.length();

    _comboBoxLineModel->clear();
    _comboBoxItems.clear();
    _comboBoxItems.resize(numDims);


    // create new checkable items in the combo box
    for (size_t i = 0; i < numDims; i++) {
        _comboBoxItems[i] = new QStandardItem();
        
        // Reduce clutter by only showing numbers if there are many dimensions
        // Therefor, enumerate the dimensions for the user the easier reference them
        _comboBoxItems[i]->setText(QString::number(i) + ": " + _dimNames[i]);
        _comboBoxItems[i]->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        _comboBoxItems[i]->setData(Qt::Checked, Qt::CheckStateRole);

        _comboBoxLineModel->insertRow(i, _comboBoxItems[i]);
    }

}

QStringList ParlCoorSettings::getSelectedDimensionNames() {
    QStringList selectedDimensionNames;

    for (size_t i = 0; i < _comboBoxLineModel->rowCount(); i++) {
        if (_comboBoxLineModel->item(i)->checkState())
        {
            selectedDimensionNames.push_back(_dimNames[i]);
        }
    }

    return selectedDimensionNames;
}

std::vector<bool> ParlCoorSettings::getSelectedDimensions() {
    std::vector<bool> selectedDimensions;

    for (size_t i = 0; i < _comboBoxLineModel->rowCount(); i++) {
        selectedDimensions.push_back(_comboBoxLineModel->item(i)->checkState());
    }

    return selectedDimensions;
}
