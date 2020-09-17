#include "ParallelCoordinatesWidget.h"

#include <QUrl>
#include <QDebug>
#include <QResizeEvent>
#include <QWebEngineView>
#include <QWebEnginePage>

#include <sstream>


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


ParallelCoordinatesWidget::ParallelCoordinatesWidget()
{
	_webView = new QWebEngineView(this);
	_webPage = new QWebEnginePage();

	QString html = readTextFromFile("D:/Documents/hdps/ParallelCoordinatesPlugin/src/web/brushing.html");
	_webView->setPage(_webPage);
	_webView->setHtml(html, QUrl("D:/Documents/hdps/ParallelCoordinatesPlugin/src/web/"));
	_webView->show();

	// TODO: adaptive resize of html/d3 content
	_webView->resize(size());
}

void ParallelCoordinatesWidget::resizeEvent(QResizeEvent * e) {
	_webView->resize(size());
}
