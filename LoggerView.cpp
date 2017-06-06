#include "LoggerView.h"
#include <QCoreApplication>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QTextCursor>
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



LoggerView::LoggerView(QWidget *parent)
	: QWidget(parent)
{
	initUi();
	load();
}

void LoggerView::closeEvent(QCloseEvent *e)
{
	if (maybeSave())
		e->accept();
	else
		e->ignore();
}
void LoggerView::paintEvent(QPaintEvent* e)
{
	QWidget::paintEvent(e);

}
void LoggerView::initUi()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	textEdit = new QTextEdit();
	textEdit->setFocus();
	mainLayout->addWidget(textEdit);
}
bool LoggerView::load()
{
	//QString f = QStringLiteral("report.htm");
	//if (!QFile::exists(f))
	//	return false;
	//QFile file(f);
	//if (!file.open(QFile::ReadOnly))
	//	return false;

	//QByteArray data = file.readAll();
	//QTextCodec *codec = Qt::codecForHtml(data);
	//QString str = codec->toUnicode(data);
	//if (Qt::mightBeRichText(str)) {
	//	textEdit->setHtml(str);
	//}
	//else {
	//	str = QString::fromLocal8Bit(data);
	//	textEdit->setPlainText(str);
	//}
	QString html = setHtmlReport(QString("N/A"), QString("N/A"), QString("N/A"), QString("N/A"), QString("N/A"), QString("N/A"), QString("N/A"));
	textEdit->setHtml(html);

	
	return true;
}

bool LoggerView::maybeSave()
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

bool LoggerView::fileSave()
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

bool LoggerView::fileSaveAs()
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

void LoggerView::filePrint()
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

void LoggerView::filePrintPreview()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
	QPrinter printer(QPrinter::HighResolution);
	QPrintPreviewDialog preview(&printer, this);
	connect(&preview, &QPrintPreviewDialog::paintRequested, this, &LoggerView::printPreview);
	preview.exec();
#endif
}

void LoggerView::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);
#else
	textEdit->print(printer);
#endif
}

void LoggerView::filePrintPdf()
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
QString LoggerView::setHtmlReport(QString& softCenter, 
									QString& laserCenter,
									QString& footA,
									QString& footB,
									QString& d1,
									QString& d2,
									QString& d3)
{
	QString html;
	QString title = QString::fromLocal8Bit("加速器校验报告--江苏富科思科技有限公司");
	QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
	QString image1path = QStringLiteral(":/Resources/image/report_1.png");
	QString image2path = QStringLiteral(":/Resources/image/report_2.png");
	QString image3path = QStringLiteral(":/Resources/image/report_3.png");
	QString pictrue1 = QString::fromLocal8Bit("图1、加速器等中心");
	QString pictrue2 = QString::fromLocal8Bit("图2、机架运动");
	QString pictrue3 = QString::fromLocal8Bit("图3、治疗床运动");
	QString item1 = QString::fromLocal8Bit("机械等中心:");
	QString item2 = QString::fromLocal8Bit("激光等中心:");
	QString item3 = QString::fromLocal8Bit("机械等中心到激光等中心距离:");
	QString item4 = QString::fromLocal8Bit("垂足A:");
	QString item5 = QString::fromLocal8Bit("垂足B:");
	QString item6 = QString::fromLocal8Bit("激光等中心到垂足A距离:");
	QString item7 = QString::fromLocal8Bit("激光等中心到垂足B距离:");
	QString item8 = QString::fromLocal8Bit("机架旋转误差--方差:");
	QString item9 = QString::fromLocal8Bit("机架旋转误差--平均值:");
	QString item10 = QString::fromLocal8Bit("机架旋转平均速度:");
	QString item11 = QString::fromLocal8Bit("治疗床旋转误差--方差:");
	QString item12 = QString::fromLocal8Bit("治疗床旋转误差--平均值:");
	QString item13 = QString::fromLocal8Bit("治疗床旋转平均速度:");


	item1 += softCenter;
	item2 += laserCenter;
	item3 += d1;
	item4 += footA;
	item5 += footB;
	item6 += d2;
	item7 += d3;


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
	//html += "<p align=\"left\">&nbsp;&nbsp;8.</p>";
	html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + pictrue1 + "< / p>";
	
	html += "<p>";
	html += "< img src = \"" + image2path + "\" width = \"320\" height = \"220\" align = \"left\" alt = \"\" / > ";
	html += "</p>";
	html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 8." + item8 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 9." + item9 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 10." + item10 + "</p>";
	html += "<p align=\"left\"><br/>< / p>";
	html += "<p align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + pictrue2 + "< / p>";

	html += "<p> ";
	html += "< img src = \"" + image3path + "\" width = \"320\" height = \"220\" align = \"left\" alt = \"\" / >";
	html += "</p>";
	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\">&nbsp;&nbsp;	11." + item11 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp;	12." + item12 + "</p>";
	html += "<p align=\"left\">&nbsp;&nbsp; 13." + item13 + "</p>";
	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\"><br/></p>";
	html += "<p align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + pictrue3 + "< / p>";

	return html;
}