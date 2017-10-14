#pragma once

#include <QMainWindow>
#include <QMap>
#include <QPointer>


class QTextEdit;
class QPrinter;

// 报告视图类
class LoggerView : public QWidget
{
	Q_OBJECT

public:
	LoggerView(QWidget *parent = 0);
	~LoggerView();
	void setHtmlReport(QString& softCenter,
						QString& laserCenter,
						QString& lightCenter,
						QString& footA,
						QString& footB,
						QString& d1,
						QString& d2,
						QString& d3,
						QString& gv,
						QString& gm,
						QString& gVel,
						QString& bv,
						QString& bm,
						QString& bVel);
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

