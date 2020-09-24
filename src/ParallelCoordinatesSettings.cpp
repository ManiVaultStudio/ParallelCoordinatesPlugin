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

	setFixedHeight(100);

	auto* settingsLayout = new QGridLayout();
	settingsLayout->setColumnStretch(0, 5);
	settingsLayout->setColumnStretch(1, 200);

	QLabel* dataSetLabel = new QLabel("Data Set");

	currentDataSetName.setReadOnly(true);
	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Base, Qt::lightGray);
	palette->setColor(QPalette::Text, Qt::black);
	currentDataSetName.setPalette(*palette);

	settingsLayout->addWidget(dataSetLabel, 0, 0, Qt::AlignLeft);
	settingsLayout->addWidget(&currentDataSetName, 0, 1, Qt::AlignLeft);

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

	currentDataSetName.setText(datasetName);

	// let the ParallelCoordinatesPlugin know about the data set name
	emit onDataInput(datasetName);
	dropEvent->acceptProposedAction();

}