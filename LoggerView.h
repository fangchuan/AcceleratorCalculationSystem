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
	~LoggerView();
	void setHtmlReport(QString& softCenter,
							QString& laserCenter,
							QString& footA,
							QString& footB,
							QString& d1,
							QString& d2,
							QString& d3,
							QString& gv,
							QString& gm,
							QString& bv,
							QString& bm);
public slots:
	bool fileSave();
	bool fileSaveAs();
	void filePrint();
	void filePrintPreview();
	void filePrintPdf();
	void printPreview(QPrinter *);
protected:
	virtual void closeEvent(QCloseEvent* e) Q_DECL_OVERRIDE;

private:
	void initUi();
	bool maybeSave();

	QString fileName;
	QTextEdit *textEdit;

};

