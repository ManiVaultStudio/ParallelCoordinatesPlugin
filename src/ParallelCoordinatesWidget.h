#include <QWidget>
#include <QString>

class QWebEngineView;
class QWebEnginePage;
class QResizeEvent;

class ParallelCoordinatesWidget : public QWidget
{
	Q_OBJECT
public:
	ParallelCoordinatesWidget();

private:
	QString readTextFromFile(QString filename);

protected:
	void resizeEvent(QResizeEvent * e) override;

private:
	QWebEngineView* _webView;
	QWebEnginePage* _webPage;
};
