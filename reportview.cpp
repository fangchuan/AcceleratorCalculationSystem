#include "reportview.h"
#include <QCoreApplication>
//#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
//#include <QFileInfo>
//#include <QFontDatabase>
#include <QTextEdit>
//#include <QStatusBar>
//#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <qpainter.h>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QDateTime>
#include <QVBoxLayout>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#endif



ReportView::ReportView(QWidget *parent)
	: QWidget(parent)
{
	initUi();
	setHtmlReport(QString("N/A"), QString("N/A"),
		QString("N/A"), QString("N/A"),
		QString("N/A"), QString("N/A"),
		QString("N/A"), QString("N/A"), 
		QString("N/A"), QString("N/A"), 
		QString("N/A"), QString("N/A"), 
		QString("N/A"), QString("N/A"),
		QString("N/A"), QString("N/A"),
		QString("N/A"), QString("N/A"),
		QString("N/A"), QString("N/A"));
}

ReportView::~ReportView()
{

}

void ReportView::closeEvent(QCloseEvent *e)
{
	if (maybeSave())
		e->accept();
	else
		e->ignore();
}

void ReportView::initUi()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	textEdit = new QTextEdit();
	textEdit->setFocus();
	mainLayout->addWidget(textEdit);
}

bool ReportView::maybeSave()
{
	if (!textEdit->document()->isModified())
		return true;

	const QMessageBox::StandardButton ret = QMessageBox::warning(this, QCoreApplication::applicationName(),
											tr("The document has been modified.\n"
											"Do you want to save your changes?"),
											QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	if (ret == QMessageBox::Save)
		return fileSave();
	else 
		if (ret == QMessageBox::Cancel)
			return false;
	return true;
}

bool ReportView::fileSave()
{
	if (fileName.isEmpty())
		return fileSaveAs();
	if (fileName.startsWith(QStringLiteral(":/")))
		return fileSaveAs();

	QTextDocumentWriter writer(fileName);

	bool success = writer.write(textEdit->document());
	if (success) {
		textEdit->document()->setModified(false);
		QMessageBox::information(this, QCoreApplication::applicationName(), tr("Wrote to file"));
	}else {
		QMessageBox::critical(this, QCoreApplication::applicationName(), tr("Could not write to file"));
	}
	return success;
}

bool ReportView::fileSaveAs()
{
	QFileDialog fileDialog(this, tr("Save as..."));
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	QStringList mimeTypes;
	mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
	fileDialog.setMimeTypeFilters(mimeTypes);
	//fileDialog.setDefaultSuffix("odt");
	if (fileDialog.exec() != QDialog::Accepted)
		return false;
	const QString fn = fileDialog.selectedFiles().first();
	fileName = fn;
	return fileSave();
}

void ReportView::filePrint()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog *dlg = new QPrintDialog(&printer, this);

	if (textEdit->textCursor().hasSelection())
		dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);

	dlg->setWindowTitle(tr("Print Document"));
	if (dlg->exec() == QDialog::Accepted)
		textEdit->print(&printer);

	delete dlg;
#endif
}

void ReportView::filePrintPreview()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
	QPrinter printer(QPrinter::HighResolution);
	QPrintPreviewDialog preview(&printer, this);
	connect(&preview, &QPrintPreviewDialog::paintRequested, this, &ReportView::printPreview);
	preview.exec();
#endif
}

void ReportView::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);
#else
	textEdit->print(printer);
#endif
}

void ReportView::filePrintPdf()
{
#ifndef QT_NO_PRINTER
	//
	QFileDialog fileDialog(this, tr("Export PDF"));
	fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
	fileDialog.setDefaultSuffix("pdf");
	if (fileDialog.exec() != QDialog::Accepted)
		return;
	QString fileName = fileDialog.selectedFiles().first();
	QPrinter printer(QPrinter::HighResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(fileName);
	textEdit->document()->print(&printer);
#endif
}
//
//
//
//
void ReportView::setHtmlReport(QString& softCenter, 
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
									QString& gAngle,
									QString& bv,
									QString& bm,
									QString& bVel,
									QString& bAngle,
									QString& cv,
									QString& cm,
									QString& cVel,
									QString& cAngle)
{
	QString html;
	QString title = tr("Accelerator Calibration Report--Nanjing University Medical Physics and Medical Imaging Research Center");
	QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
	QString image1path = QStringLiteral(":/Resources/image/report_1.png");
	QString image2path = QStringLiteral(":/Resources/image/report_2.png");
	QString image3path = QStringLiteral(":/Resources/image/report_3.png");
	QString pictrue1 = tr("Figure 1, ISOcenter ");//
	QString pictrue2 = tr("Figure 2, Gantry Movement");//);
	QString pictrue3 = tr("Figure 3, Bed Movement");//
	QString item1 = tr("Mechainc ISOcenter:");//
	QString item2 = tr("Laser ISOcenter:");//
	QString item3 = tr("Light Center:");//
	QString item4 = tr("Distance between Machanic ISOcenter to Laser ISOcenter:");//
	QString item5 = tr("Footpoint A:");//
	QString item6 = tr("Footpoint B:");//
	QString item7 = tr("Distance between Laser ISOcenter to Footpoint A:");
	QString item8 = tr("Distance between Laser ISOcenter to Footpoint B:");
	QString item9 = tr("Mechanic rotation error - variance:");
	QString item10 = tr("Mechanic rotation error - mean:");
	QString item11 = tr("Gantry rotation average velocity:");
	QString item12 = tr("Angle between gantry rotation plane and horizontal plane:");
	QString item13 = tr("Bed rotation error - variance:");
	QString item14 = tr("Bed rotation error - mean:");
	QString item15 = tr("Bed rotation average velocity:");
	QString item16 = tr("Bed rotation plane and horizontal plane angle:");
	


	item1 += softCenter;
	item2 += laserCenter;
	item3 += lightCenter;
	item4 += d1;
	item5 += footA;
	item6 += footB;
	item7 += d2;
	item8 += d3;
	item9 += gv;
	item10 += gm;
	item11 += gVel;
	item12 += gAngle;
	item13 += bv;
	item14 += bm;
	item15 += bVel;
	item16 += bAngle;

	html += "<h1 align=\"center\">";
	html += "<strong><span bgcolor=\"red\">"+ title +"< / span>< / strong>";
	html += "</h1>";
	html += "<h3 align=\"center\">";
	html += "<span><b>" + current_date + "</b></span>";
	html += "</h3><br/>";

	html += "<p>";
	html += "<b> < img src = \"" + image1path + "\" width = \"320\" height = \"240\" title = \"\" align = \"left\" alt = \"\" / >";
	html += "</p>";
	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\">&nbsp;&nbsp;1." + item1 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;2." + item2 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;3." + item3 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;4." + item4 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;5." + item5 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;6." + item6 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;7." + item7 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;8." + item8 + "</p>";
	//html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + pictrue1 + "< / p>";
	
	html += "<p>";
	html += "< img src = \"" + image2path + "\" width = \"320\" height = \"220\" align = \"left\" alt = \"\" / > ";
	html += "</p>";
	html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 9." + item9 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 10." + item10 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;	11." + item11 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;	12." + item12 + "</p>";
	html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + pictrue2 + "< / p>";

	html += "<p> ";
	html += "< img src = \"" + image3path + "\" width = \"320\" height = \"220\" align = \"left\" alt = \"\" / >";
	html += "</p>";
	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\">&nbsp;&nbsp;	13." + item13 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 14." + item14 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 15." + item15 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 16." + item16 + "</p>";

	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + pictrue3 + "< / p>";

	textEdit->setHtml(html);

	return ;
}