#include "ParallelCoordinatesSettings.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>


ParallelCoordinatesSettings::ParallelCoordinatesSettings() {
	// enable drag and drop of data elements, see https://doc.qt.io/qt-5/dnd.html#dropping
	setAcceptDrops(true);

	setFixedHeight(100);
}

void ParallelCoordinatesSettings::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
	const auto items = dragEnterEvent->mimeData()->text().split("\n");
	const auto datasetType = items.at(1);

	// Allow drag event for point data set type
	if (datasetType == "Points") {
		dragEnterEvent->acceptProposedAction();
	}

}

void ParallelCoordinatesSettings::dropEvent(QDropEvent* dropEvent)
{
	const auto items = dropEvent->mimeData()->text().split("\n");
	const auto datasetName = items.at(0);

	// let the ParallelCoordinatesPlugin know about the data set name
	emit onDataInput(datasetName);
	dropEvent->acceptProposedAction();

}