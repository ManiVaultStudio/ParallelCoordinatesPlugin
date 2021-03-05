#include "ParallelCoordinatesSettings.h"

#include <QMimeData>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>

ParlCoorSettings::ParlCoorSettings() {
    // layout structure
    setFixedHeight(40);
    auto* settingsLayout = new QGridLayout();
    settingsLayout->setColumnStretch(0, 5);
    settingsLayout->setColumnStretch(1, 200);

    // read only displays

    // number of points
    QLabel* numPointsLabel = new QLabel("Num. points:");
    numPoints.setFixedWidth(50);
    numPoints.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // (number of) selected points 
    QLabel* numSelLabel = new QLabel("Selection size:");
    numSel.setFixedWidth(50);
    numSel.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // number of dims
    QLabel* numDimsLabel = new QLabel("Num. dims.:");
    numDims.setFixedWidth(50);
    numDims.setAlignment(Qt::AlignCenter | Qt::AlignLeft);

    // add elements to gui
    settingsLayout->addWidget(numPointsLabel, 0, 1, Qt::AlignRight);
    settingsLayout->addWidget(&numPoints, 0, 2, Qt::AlignRight);
    settingsLayout->addWidget(numSelLabel, 0, 3, Qt::AlignRight);
    settingsLayout->addWidget(&numSel, 0, 4, Qt::AlignRight);
    settingsLayout->addWidget(numDimsLabel, 0, 5, Qt::AlignRight);
    settingsLayout->addWidget(&numDims, 0, 6, Qt::AlignRight);

    setLayout(settingsLayout);
}

