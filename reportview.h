#pragma once

#include <QMainWindow>



class QTextEdit;
class QPrinter;

// 报告视图类
class ReportView : public QWidget
{
	Q_OBJECT

public:
	ReportView(QWidget *parent = 0);
	~ReportView();
	void setHtmlReport(QString& softCenter,
						QString& laserCenter,
						QString& footA,
						QString& footB,
						QString& d1,
						QString& d2,
						QString& d3,
						QString& cbctVerticalty,
						QString& epidVerticalty,
						QString& gv,
						QString& gm,
						QString& gVel,
						QString& gAngle,
						QString& bv,
						QString& bm,
						QString& bVel,
						QString& bAngle);
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

