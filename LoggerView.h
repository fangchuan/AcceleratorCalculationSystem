#pragma once

#include <QMainWindow>
#include <QMap>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QPrinter;
QT_END_NAMESPACE

class LoggerView : public QWidget
{
	Q_OBJECT

public:
	LoggerView(QWidget *parent = 0);

	bool load();
	QString setHtmlReport(QString& softCenter,
							QString& laserCenter,
							QString& footA,
							QString& footB,
							QString& d1,
							QString& d2,
							QString& d3);
public slots:

	void initUi();
	bool fileSave();
	bool fileSaveAs();
	void filePrint();
	void filePrintPreview();
	void filePrintPdf();
	void printPreview(QPrinter *);
protected:
	virtual void closeEvent(QCloseEvent* e) Q_DECL_OVERRIDE;
	virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
private:
	bool maybeSave();

	QString fileName;
	QTextEdit *textEdit;

};

