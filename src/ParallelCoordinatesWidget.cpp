#include "ParallelCoordinatesWidget.h"

#include <sstream>
#include <iostream>

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QLayout>

#include <QWebEnginePage>	// object to view and edit web documents
#include <QWebEngineView>	// widget that is used to view and edit web documents
#include <QWebEngineSettings>	// settings used by QWebEnginePage

void loadJsWidgetResources() {
	Q_INIT_RESOURCE(js_widget);
}


ParallelCoordinatesWidget::ParallelCoordinatesWidget():
	_webView(nullptr),
	_mainPage(nullptr),
	_connection_ready(false)
{
	setFixedSize(300, 200);
	loadJsWidgetResources();
	initUI();
}

QString ParallelCoordinatesWidget::readTextFromFile(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		std::stringstream ss;
		ss << "Unable to open the file: " << filename.toStdString();
		throw std::logic_error(ss.str());
	}
	QTextStream in(&file);
	return in.readAll();
}

void ParallelCoordinatesWidget::initUI() {
	_webView = new QWebEngineView(this);

	connect(_webView, &QWebEngineView::loadFinished, this, &ParallelCoordinatesWidget::onWebViewFinishedLoading);

	_mainPage = _webView->page();

	//QObject::connect(_mainPage, &QWebEnginePage::javaScriptWindowObjectCleared, this, &JsWidget::onConnectJs);
	//if (!_webView->settings()->testAttribute(QWebEngineSettings::JavascriptEnabled)) {
	//	throw std::runtime_error("Javascript is not enabled!");
	//}

	QString html = readTextFromFile(":/myWidget/widget.html");
	_webView->setHtml(html, QUrl("qrc:/myWidget/"));
}

void ParallelCoordinatesWidget::onJsLog(QString text) {
	//QTextStream(stdout) << "JS Log:\t" << text << endl;
}

void ParallelCoordinatesWidget::onJsError(QString text) {
	throw std::runtime_error(text.toStdString().c_str());
}

void ParallelCoordinatesWidget::onConnectJs() {
	_mainPage->addToJavaScriptWindowObject("Qt", this);
}

void ParallelCoordinatesWidget::onWebViewFinishedLoading(bool ok) {
	_connection_ready = true;
	//_webView->page()->setViewportSize(size());
}


void ParallelCoordinatesWidget::onSetData(std::vector<std::pair<double, double>>& points) {
	std::stringstream ss;
	ss << "x,y" << std::endl;
	for (auto& p : points) {
		ss << std::get<0>(p) << "," << std::get<1>(p) << std::endl;
	}
	while (!_connection_ready) {//it is better to use condition_variable
		QApplication::processEvents();
	}

	emit sgnSetData(QString::fromStdString(ss.str()));
}

void ParallelCoordinatesWidget::resizeEvent(QResizeEvent * e) {
	_webView->page()->setViewportSize(size());
}