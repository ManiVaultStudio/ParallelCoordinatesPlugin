#pragma once
#ifndef JS_WIDGET_H
#define JS_WIDGET_H

#include <utility>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <QWidget>

class QWebEngineView;
class QWebEnginePage;


class ParallelCoordinatesWidget : public QWidget
{
	Q_OBJECT
public:
	ParallelCoordinatesWidget();

	//Slots used by C++
public slots:
	void onSetData(std::vector<std::pair<double, double>>& points);

	//Slots used by JS
public slots:
	void onJsLog(QString text);
	void onJsError(QString text);

private:
	void initUI();
	QString readTextFromFile(QString filename);
	void asyncSetData(std::vector<std::pair<double, double>> points);

	//JS connection handling
private slots:
	void onWebViewFinishedLoading(bool ok);
	void onConnectJs();

signals:
	void sgnSetData(QString);

protected:
	virtual void resizeEvent(QResizeEvent * e);

private:
	QWebEngineView* _webView;
	QWebEnginePage* _mainPage;
	bool _connection_ready;
	std::mutex _mutex;
	std::condition_variable _cv;
	std::thread _connection_thread;

};

#endif // JS_WIDGET_H
