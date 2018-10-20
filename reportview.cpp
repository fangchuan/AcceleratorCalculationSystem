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
		QString("N/A"));
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
									QString& bAngle)
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
	QString item3 = tr("Distance between Machanic ISOcenter to Laser ISOcenter:");//
	QString item4 = tr("Footpoint A:");//
	QString item5 = tr("Footpoint B:");//
	QString item6 = tr("Distance between Laser ISOcenter to Footpoint A:");
	QString item7 = tr("Distance between Laser ISOcenter to Footpoint B:");
	QString item8 = tr("CBCT panel verticalty:");//
	QString item8_2 = tr("EPID panel verticalty:");
	QString item9 = tr("Mechanic rotation error - variance:");
	QString item10 = tr("Mechanic rotation error - mean:");
	QString item11 = tr("Gantry rotation average velocity:");
	QString item12 = tr("Angle between gantry rotation plane and horizontal plane:");
	QString item13 = tr("Bed rotation error - variance:");
	QString item14 = tr("Bed rotation error - mean:");
	QString item15 = tr("Bed rotation average velocity:");
	QString item16 = tr("Bed rotation plane and horizontal plane angle:");
	
	QString itemName = tr("Name");
	QString itemValue = tr("Value");
	QString itemDescription = tr("Description");
	QString item1Description = tr("iterm1 Description");
	QString item9Description = tr(" iterm9 Description");
	QString item13Description = tr("item13 Description");

	html += "<h1 align=\"center\">";
	html += "<strong><span bgcolor=\"red\">" + title + "< / span>< / strong>";
	html += "</h1>";
	html += "<h3 align=\"center\">";
	html += "<span><b>" + current_date + "</b></span>";
	html += "</h3>";
	html += "<p align=\"center\">";
	html += "<hr style=\"FILTER: alpha(opacity = 100, finishopacity = 0, style = 3)\" width='100%' color='#ff0000' size='10' /> < / p>";

	html += "<p align=\"center\">";
	html += "< img src = \"" + image1path + "\" width = '320' height = '240'  align = 'center'  />";
	html += "</p>";
	html += "<p align=\"center\">" + pictrue1 + "< / p>";
	html += "<p align=\"center\"><br/></p>";

	html += "<table width='500' border='1' align='center' style='border-collapse:collapse;' cellpadding='2'>";
	html += "<tr style='background-color:gray'> <th>" + itemName + "</th> <th>" + itemValue + "</th> <th>" + itemDescription + "</th></tr>";
	html += "<tr> <td>" + item1 + "</td> <td>" + softCenter + "</td> <td>" + item1Description +"</td>";
	html += "<tr> <td>" + item4 + "</td> <td>" + footA + "</td> <td>" + item1Description + "</td>";
	html += "<tr> <td>" + item5 + "</td> <td>" + footB + "</td> <td>" + item1Description + "</td>";
	html += "<tr> <td>" + item2 + "</td> <td>" + laserCenter + "</td> <td>" + item1Description + "</td>";
	html += "<tr> <td>" + item3 + "</td> <td>" + d1 + "</td>";
	html += "<tr> <td>" + item6 + "</td> <td>" + d2 + "</td>";
	html += "<tr> <td>" + item7 + "</td> <td>" + d3 + "</td>";
	html += "<tr> <td>" + item8 + "</td> <td>" + cbctVerticalty + "</td>";
	html += "<tr> <td>" + item8_2 + "</td> <td>" + epidVerticalty + "</td>";
	html += "</table>";
	html += "<p align=\"center\"><br/></p>";
	html += "<p align=\"center\">";
	html += "<hr style=\"border:1 dashed #987cb9\" width='80%' color='#987cb9' size=1 /></p>";

	html += "<p align=\"center\">";
	html += "< img src = \"" + image2path + "\" width = \"320\" height = \"220\" align = \"center\" alt = \"\" / > ";
	html += "</p>";
	html += "<p align=\"center\">" + pictrue2 + "< / p>";
	html += "<p align=\"center\"><br/>< / p>";
	html += "<table width='500' border='1' align='center' style='border-collapse:collapse;' cellpadding='2'>";
	html += "<tr style='background-color:gray'> <th>" + itemName + "</th> <th>" + itemValue + "</th> <th>" + itemDescription + "</th></tr>";
	html += "<tr> <td>" + item9 + "</td> <td>" + gv + "</td> <td>" + item9Description + "</td>";
	html += "<tr> <td>" + item10 + "</td> <td>" + gm + "</td>";
	html += "<tr> <td>" + item11 + "</td> <td>" + gVel + "</td>";
	html += "<tr> <td>" + item12 + "</td> <td>" + gAngle + "</td>";
	html += "</table>";
	html += "<p align=\"center\"><br/></p>";
	html += "<p align=\"center\">";
	html += "<hr style=\"border:1 dashed #987cb9\" width=\"80%\" color=#987cb9 size=1 /></p>";

	html += "<p align=\"center\">";
	html += "< img src = \"" + image3path + "\" width = \"320\" height = \"220\" align = \"center\" alt = \"\" / >";
	html += "</p>";
	html += "<p align=\"center\">" + pictrue3 + "< / p>";
	html += "<p align=\"center\"><br/></p>";
	html += "<table width='500' border='1' align='center' style='border-collapse:collapse;' cellpadding='2'>";
	html += "<tr style='background-color:gray'> <th>" + itemName + "</th> <th>" + itemValue + "</th> <th>" + itemDescription + "</th></tr>";
	html += "<tr> <td>" + item13 + "</td> <td>" + bv + "</td> <td>" + item13Description + "</td>";
	html += "<tr> <td>" + item14 + "</td> <td>" + bm + "</td>";
	html += "<tr> <td>" + item15 + "</td> <td>" + bVel + "</td>";
	html += "<tr> <td>" + item16 + "</td> <td>" + bAngle + "</td>";
	html += "</table>";
	html += "<p align=\"center\"><br/></p>";

	textEdit->setHtml(html);

	return ;
}