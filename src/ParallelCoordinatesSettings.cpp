#include "ParallelCoordinatesSettings.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>

ParlCoorSettings::ParlCoorSettings() {
	// enable drag and drop of data elements, see https://doc.qt.io/qt-5/dnd.html#dropping
	setAcceptDrops(true);

	// layout structure
	setFixedHeight(100);
	auto* settingsLayout = new QGridLayout();
	settingsLayout->setColumnStretch(0, 5);
	settingsLayout->setColumnStretch(1, 200);

	// read only displays

	// data set name display
	QLabel* dataSetLabel = new QLabel("Data Set");
	currentDataSetName.setFixedWidth(150);
	currentDataSetName.setFixedHeight(20);
	currentDataSetName.setFrameStyle(QFrame::Panel | QFrame::Sunken);
	currentDataSetName.setAlignment(Qt::AlignCenter | Qt::AlignRight);

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
	settingsLayout->addWidget(dataSetLabel, 0, 0, Qt::AlignLeft);
	settingsLayout->addWidget(&currentDataSetName, 0, 1, Qt::AlignLeft);
	settingsLayout->addWidget(numPointsLabel, 0, 2, Qt::AlignLeft);
	settingsLayout->addWidget(&numPoints, 0, 3, Qt::AlignLeft);
	settingsLayout->addWidget(numSelLabel, 0, 4, Qt::AlignLeft);
	settingsLayout->addWidget(&numSel , 0, 5, Qt::AlignLeft);
	settingsLayout->addWidget(numDimsLabel, 1, 2, Qt::AlignLeft);
	settingsLayout->addWidget(&numDims, 1, 3, Qt::AlignLeft);

	setLayout(settingsLayout);
}

void ParlCoorSettings::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
	const auto items = dragEnterEvent->mimeData()->text().split("\n");
	const auto datasetType = items.at(1);

	// Allow drag event for point data set type
	if (datasetType == "Points") {
		dragEnterEvent->acceptProposedAction();
	}

}

void ParlCoorSettings::dropEvent(QDropEvent* dropEvent)
{
	const auto items = dropEvent->mimeData()->text().split("\n");
	const auto datasetName = items.at(0);

	setCurrentDataSetName(datasetName);
	// let the ParallelCoordinatesPlugin know about the data set name
	emit onDataInput(datasetName);

	dropEvent->acceptProposedAction();
}

