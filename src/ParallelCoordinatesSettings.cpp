#include "ParallelCoordinatesSettings.h"

#include "ParallelCoordinatesPlugin.h"

#include <QMimeData>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QFileDialog>

#include <assert.h> // for NDEBUG

#if !defined(NDEBUG)
    #include <QPushButton> 
#endif NDEBUG

ParlCoorSettings::ParlCoorSettings(ParallelCoordinatesPlugin* parent): _parentPlugin(parent){
    // layout structure
    setFixedHeight(75);
    setAutoFillBackground(true);
    setAcceptDrops(true);

    auto* settingsLayout = new QGridLayout();
    //settingsLayout->setColumnStretch(0, 5);
    //settingsLayout->setColumnStretch(1, 200);
    
    // combobox of checkboxes
    QLabel* comboBoxLabel = new QLabel("Active Dimensions:");
    QComboBox* _comboBox = new QComboBox();
    _comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboBox->setMinimumWidth(100);
    _comboBox->setMaximumWidth(300);
    _comboBoxLineModel = new QStandardItemModel();
    _comboBox->setModel(_comboBoxLineModel);
    _comboBox->setStyleSheet("QComboBox QAbstractItemView{min-height: 250px;}");
    _loadDimsButton = new QPushButton("Load ...");
    _applyDimsButton = new QPushButton("Apply");

    // clamping slders
    QLabel* _minClampLabel = new QLabel("Range Min. (%):");
    QLabel* _maxClampLabel = new QLabel("Range Max. (%):");
    _minClampValLabel = new QLabel(QString::number(0));
    _maxClampValLabel = new QLabel(QString::number(100));

    _minClamp = new QSlider(Qt::Horizontal);
    _minClamp->setRange(0, 100);
    _minClamp->setValue(_minClamp->minimum());
    _maxClamp = new QSlider(Qt::Horizontal);
    _maxClamp->setRange(0, 100);
    _maxClamp->setValue(_maxClamp->maximum());
    
    // connect the two sliders
    connect(_minClamp, SIGNAL(valueChanged(int)), this, SLOT(adjustMaxClamp(int)));
    connect(_maxClamp, SIGNAL(valueChanged(int)), this, SLOT(adjustMinClamp(int)));
    // set the slider value in UI
    connect(_minClamp, &QSlider::valueChanged, [this](int val) {_minClampValLabel->setText(QString::number(val)); });
    connect(_maxClamp, &QSlider::valueChanged, [this](int val) {_maxClampValLabel->setText(QString::number(val)); });

    // read only displays

    // number of points
    QLabel* numPointsLabel = new QLabel("Num. points:");
    _numPoints.setFixedWidth(35);
    _numPoints.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // (number of) selected points 
    QLabel* numSelLabel = new QLabel("Selection size:");
    _numSel.setFixedWidth(35);
    _numSel.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // number of dims
    QLabel* numDimsLabel = new QLabel("Num. dims.:");
    _numDims.setFixedWidth(35);
    _numDims.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // only show selected dimensions
    connect(_loadDimsButton, &QPushButton::released, this, &ParlCoorSettings::onLoadDims);
    connect(_applyDimsButton, &QPushButton::released, _parentPlugin, &ParallelCoordinatesPlugin::onApplySettings);

    // add elements to gui
    settingsLayout->addWidget(_minClampLabel, 0, 1, Qt::AlignRight);
    settingsLayout->addWidget(_minClampValLabel, 0, 2, Qt::AlignRight);
    settingsLayout->addWidget(_minClamp, 0, 3, Qt::AlignRight);
    settingsLayout->addWidget(_maxClampLabel, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(_maxClampValLabel, 1, 2, Qt::AlignRight);
    settingsLayout->addWidget(_maxClamp, 1, 3, Qt::AlignRight);

    settingsLayout->addWidget(numPointsLabel, 0, 4, Qt::AlignRight);
    settingsLayout->addWidget(&_numPoints, 0, 5, Qt::AlignRight);
    settingsLayout->addWidget(numSelLabel, 0, 6, Qt::AlignRight);
    settingsLayout->addWidget(&_numSel, 0, 7, Qt::AlignRight);
    settingsLayout->addWidget(numDimsLabel, 0, 8, Qt::AlignRight);
    settingsLayout->addWidget(&_numDims, 0, 9, Qt::AlignRight);

    settingsLayout->addWidget(comboBoxLabel, 1, 6, Qt::AlignRight);
    settingsLayout->addWidget(_comboBox, 1, 7, Qt::AlignRight);
    settingsLayout->addWidget(_loadDimsButton, 1, 8, Qt::AlignRight);
    settingsLayout->addWidget(_applyDimsButton, 1, 9, Qt::AlignRight);

    setLayout(settingsLayout);
}

void ParlCoorSettings::adjustMaxClamp(int val) {
    if (val > _maxClamp->value())
        _maxClamp->setValue(val);
}

void ParlCoorSettings::adjustMinClamp(int val) {
    if (val < _minClamp->value())
        _minClamp->setValue(val);
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

void ParlCoorSettings::onLoadDims() {

    const auto fileName = QFileDialog::getOpenFileName(this,QObject::tr("Dimension selection"), {}, "*.txt");

    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            disableAllDimensions();

            while (!file.atEnd())
            {
                const auto trimmedLine = file.readLine().trimmed();

                if (!trimmedLine.isEmpty())
                {
                    const auto name = QString::fromUtf8(trimmedLine);

                    if (!tryToEnableDimensionByName(name))
                    {
                        qWarning() << "Failed to select dimension (name not found): " << name;
                    }
                }
            }
        }
        else
        {
            qCritical() << "Load failed to open file: " << fileName;
        }
    }
}

bool ParlCoorSettings::disableAllDimensions() {
    for (size_t i = 0; i < _comboBoxLineModel->rowCount(); i++) {
        _comboBoxLineModel->item(i)->setCheckState(Qt::Unchecked);
    }

    return true;
}

bool ParlCoorSettings::tryToEnableDimensionByName(QString name) {
    
    for (std::size_t i{}; i < _dimNames.size(); ++i)
    {
        if (name == _dimNames[i])
        {
            _comboBoxLineModel->item(i)->setCheckState(Qt::Checked);
            return true;
        }
    }
    return false;
}