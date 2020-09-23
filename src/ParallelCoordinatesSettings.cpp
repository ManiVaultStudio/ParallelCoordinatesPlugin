#include "ParallelCoordinatesSettings.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

ParlCoorSettings::ParlCoorSettings() {
	// enable drag and drop of data elements, see https://doc.qt.io/qt-5/dnd.html#dropping
	setAcceptDrops(true);

	setFixedHeight(100);

	QGroupBox* settingsBox = new QGroupBox("Basic settings");
	auto* const settingsLayout = new QGridLayout();

	settingsLayout->addWidget(&QLabel("Data Set"), 0, 0);
	settingsLayout->addWidget(&coreDataSets, 1, 0);

	settingsBox->setLayout(settingsLayout);

	//addWidget(settingsBox);	// TODO: add the widget, maybe refactor some code
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

	// let the ParallelCoordinatesPlugin know about the data set name
	emit onDataInput(datasetName);
	dropEvent->acceptProposedAction();

}