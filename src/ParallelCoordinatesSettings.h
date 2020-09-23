#pragma once

#include <widgets/SettingsWidget.h>

//#include <QWidget>
#include <QComboBox>

using namespace hdps::gui;

class ParlCoorSettings : public SettingsWidget
{
	Q_OBJECT

public:
	ParlCoorSettings();

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

public: 
	QComboBox	coreDataSets;

};
