#pragma once

#include <QWidget>


class ParallelCoordinatesSettings : public QWidget
{
	Q_OBJECT

public:
	ParallelCoordinatesSettings();

private:
	// Drag and drop

	/**
	 * Invoked when an item is dragged into the widget
	 * and checks if the data type is okay to be used
	 * @param dragEnterEvent Drag enter event
	 */
	void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;

	/**
	 * Invoked when an item is dropped onto the widget
	 * and invokes futher data handling
	 * @param dropEvent Drop event
	 */
	void dropEvent(QDropEvent* dropEvent) override;

signals:
	void onDataInput(QString dataName);

};
