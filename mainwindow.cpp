#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CentralWidget.h"
#include "OpsTrackingDevice.h"
#include <qdesktopservices.h>
#include <QToolBar>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_CentralWidget(Q_NULLPTR)
{

    initUi();
    buildConnections();
	detectTracker();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUi()
{
	ui->setupUi(this);
    m_CentralWidget = new CentralWidget(this);
    setCentralWidget(m_CentralWidget);
	setWindowTitle(QCoreApplication::applicationName());
	setWindowIcon(QIcon(":/Resources/image/window_icon.png"));

	setupFileActions();
	setupViewActions();
	setupHelpAction();

	this->setObjectName("mainWindow");
	this->setStyleSheet("#mainWindow{background-color:rgb(205, 205, 205);}");

	ui->mainToolBar->setIconSize(QSize(32, 32));
}

void MainWindow::buildConnections()
{
	connect(mThreeDAction, &QAction::triggered, m_CentralWidget, &CentralWidget::show3DPage);
	connect(mPlotAction, &QAction::triggered,   m_CentralWidget, &CentralWidget::showPlotPage);
	connect(mExportAction, &QAction::triggered, m_CentralWidget, &CentralWidget::exportReport );
	connect(mSaveAction, &QAction::triggered,   m_CentralWidget, &CentralWidget::saveReport);
	connect(mSaveAsAction, &QAction::triggered, m_CentralWidget, &CentralWidget::saveAsReport);
	connect(mPrintAction, &QAction::triggered,  m_CentralWidget, &CentralWidget::printReport);
	connect(mPreviewAction, &QAction::triggered, m_CentralWidget, &CentralWidget::printPreviewReport);
	connect(mClearAction, &QAction::triggered,   m_CentralWidget, &CentralWidget::clearAllPlot);
	connect(mTextAction, &QAction::triggered,    m_CentralWidget, &CentralWidget::showLogTextPage);
	connect(mHelpAction, &QAction::triggered,    this, &MainWindow::showHelp);
	connect(mQuit,       &QAction::triggered,    this, &MainWindow::closeWindow);
}

void MainWindow::detectTracker()
{
	if (OpsTrackingDevice::getInstance()->findTracker() == NDIPolaris){

	   OpsTrackingDevice::getInstance()->openConnection();
	   mTrackerDetected = true;
	}else{
	       
		QMessageBox::warning(Q_NULLPTR, QCoreApplication::applicationName(), 
										QObject::tr("未找到相机，请检查相机连接！"));
		mTrackerDetected = false;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    exitMainWindow();
    QMainWindow::closeEvent(event);
}

void MainWindow::exitMainWindow()
{
    disconnectTracker();
}

void MainWindow::disconnectTracker()
{
	if (mTrackerDetected){
		OpsTrackingDevice *tracker = OpsTrackingDevice::getInstance();
		if (tracker->getState() >= TrackingDevice::Setup){
			tracker->stopTracking();
		}
		tracker->closeConnection();
	}
}

void MainWindow::showHelp()
{
	QDesktopServices::openUrl(QUrl("http://www.jsfocus.cn"));
}

void MainWindow::closeWindow()
{
	QMessageBox::StandardButton ret = QMessageBox::question(this, QCoreApplication::applicationName(), QObject::tr("Are you sure to leave?"));

	if (ret == QMessageBox::Yes){
        close();
	}
}

void MainWindow::setupFileActions()
{
	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/Resources/image/file_save.png"));
	mSaveAction = ui->mainToolBar->addAction(saveIcon, tr("&Save"));
	mSaveAction->setShortcut(QKeySequence::Save);
	mSaveAction->setEnabled(true);

	mSaveAsAction = ui->mainToolBar->addAction(QIcon(":/Resources/image/file_save_as.png"), tr("Save &As..."));
	mSaveAsAction->setPriority(QAction::LowPriority);

#ifndef QT_NO_PRINTER
	const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(":/Resources/image/print.png"));
	mPrintAction = ui->mainToolBar->addAction(printIcon, tr("&Print..."));
	mPrintAction->setPriority(QAction::LowPriority);
	mPrintAction->setShortcut(QKeySequence::Print);

	const QIcon filePrintIcon = QIcon::fromTheme("fileprint", QIcon(":/Resources/image/print_preview.png"));
	mPreviewAction = ui->mainToolBar->addAction(filePrintIcon, tr("Print Preview..."));

	const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(":/Resources/image/export_pdf.png"));
	mExportAction = ui->mainToolBar->addAction(exportPdfIcon, tr("&Export PDF..."));
	mExportAction->setPriority(QAction::LowPriority);
	mExportAction->setShortcut(Qt::CTRL + Qt::Key_D);
#endif

}

void MainWindow::setupViewActions()
{
	const QIcon threeDIcon = QIcon::fromTheme("3D-modle-view", QIcon(":/Resources/image/3d_view.png"));
	mThreeDAction = ui->mainToolBar->addAction(threeDIcon, tr("3D modle view"));

	const QIcon plotIcon = QIcon::fromTheme("Plot-view", QIcon(":/Resources/image/plot_view.png"));
	mPlotAction = ui->mainToolBar->addAction(plotIcon, tr("Plot view"));

	const QIcon textIcon = QIcon::fromTheme("Log-view", QIcon(":/Resources/image/report_view.png"));
	mTextAction = ui->mainToolBar->addAction(textIcon, tr("Log view"));

	const QIcon clearIcon = QIcon::fromTheme("edit-clear", QIcon(":/Resources/image/clear.png"));
	mClearAction = ui->mainToolBar->addAction(clearIcon, tr("&Clear the data"));
	mClearAction->setShortcut(QKeySequence::Delete);

}

void MainWindow::setupHelpAction()
{
	const QIcon helpIcon = QIcon::fromTheme("help", QIcon(":/Resources/image/help.png"));
	mHelpAction = ui->mainToolBar->addAction(helpIcon, tr("Help"));

	mQuit = ui->mainToolBar->addAction(QIcon(":/Resources/image/close.png"), tr("&Quit"));
	mQuit->setShortcut(Qt::CTRL + Qt::Key_Q);
}
//void MainWindow::setupTextActions()
//{
//	QMenu *menu = ui->menuBar->addMenu(tr("&Format"));
//
//	const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon("/textbold.png"));
//	actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
//	actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
//	actionTextBold->setPriority(QAction::LowPriority);
//	QFont bold;
//	bold.setBold(true);
//	actionTextBold->setFont(bold);
//	tb->addAction(actionTextBold);
//	actionTextBold->setCheckable(true);
//
//	const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
//	actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
//	actionTextItalic->setPriority(QAction::LowPriority);
//	actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
//	QFont italic;
//	italic.setItalic(true);
//	actionTextItalic->setFont(italic);
//	tb->addAction(actionTextItalic);
//	actionTextItalic->setCheckable(true);
//
//	const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
//	actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
//	actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
//	actionTextUnderline->setPriority(QAction::LowPriority);
//	QFont underline;
//	underline.setUnderline(true);
//	actionTextUnderline->setFont(underline);
//	tb->addAction(actionTextUnderline);
//	actionTextUnderline->setCheckable(true);
//
//	menu->addSeparator();
//
//	const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
//	actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
//	actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
//	actionAlignLeft->setCheckable(true);
//	actionAlignLeft->setPriority(QAction::LowPriority);
//	const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
//	actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
//	actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
//	actionAlignCenter->setCheckable(true);
//	actionAlignCenter->setPriority(QAction::LowPriority);
//	const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
//	actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
//	actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
//	actionAlignRight->setCheckable(true);
//	actionAlignRight->setPriority(QAction::LowPriority);
//	const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
//	actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
//	actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
//	actionAlignJustify->setCheckable(true);
//	actionAlignJustify->setPriority(QAction::LowPriority);
//
//	// Make sure the alignLeft  is always left of the alignRight
//	QActionGroup *alignGroup = new QActionGroup(this);
//	connect(alignGroup, &QActionGroup::triggered, this, &TextEdit::textAlign);
//
//	if (QApplication::isLeftToRight()) {
//		alignGroup->addAction(actionAlignLeft);
//		alignGroup->addAction(actionAlignCenter);
//		alignGroup->addAction(actionAlignRight);
//	}
//	else {
//		alignGroup->addAction(actionAlignRight);
//		alignGroup->addAction(actionAlignCenter);
//		alignGroup->addAction(actionAlignLeft);
//	}
//	alignGroup->addAction(actionAlignJustify);
//
//	tb->addActions(alignGroup->actions());
//	menu->addActions(alignGroup->actions());
//
//	menu->addSeparator();
//
//	QPixmap pix(16, 16);
//	pix.fill(Qt::black);
//	actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
//	tb->addAction(actionTextColor);
//
//	tb = addToolBar(tr("Format Actions"));
//	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
//	addToolBarBreak(Qt::TopToolBarArea);
//	addToolBar(tb);
//
//	comboStyle = new QComboBox(tb);
//	tb->addWidget(comboStyle);
//	comboStyle->addItem("Standard");
//	comboStyle->addItem("Bullet List (Disc)");
//	comboStyle->addItem("Bullet List (Circle)");
//	comboStyle->addItem("Bullet List (Square)");
//	comboStyle->addItem("Ordered List (Decimal)");
//	comboStyle->addItem("Ordered List (Alpha lower)");
//	comboStyle->addItem("Ordered List (Alpha upper)");
//	comboStyle->addItem("Ordered List (Roman lower)");
//	comboStyle->addItem("Ordered List (Roman upper)");
//
//	typedef void (QComboBox::*QComboIntSignal)(int);
//	connect(comboStyle, static_cast<QComboIntSignal>(&QComboBox::activated), this, &TextEdit::textStyle);
//
//	typedef void (QComboBox::*QComboStringSignal)(const QString &);
//	comboFont = new QFontComboBox(tb);
//	tb->addWidget(comboFont);
//	connect(comboFont, static_cast<QComboStringSignal>(&QComboBox::activated), this, &TextEdit::textFamily);
//
//	comboSize = new QComboBox(tb);
//	comboSize->setObjectName("comboSize");
//	tb->addWidget(comboSize);
//	comboSize->setEditable(true);
//
//	const QList<int> standardSizes = QFontDatabase::standardSizes();
//	foreach(int size, standardSizes)
//		comboSize->addItem(QString::number(size));
//	comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));
//
//	connect(comboSize, static_cast<QComboStringSignal>(&QComboBox::activated), this, &TextEdit::textSize);
//}
