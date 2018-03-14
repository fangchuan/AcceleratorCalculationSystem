#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centralwidget.h"
#include "opstrackingdevice.h"
#include <qdesktopservices.h>
#include <QToolBar>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent)
    ,ui(new Ui::MainWindow)
    ,m_CentralWidget(Q_NULLPTR)
	, mThreeDAction(Q_NULLPTR)
	, mPlotAction(Q_NULLPTR)
	, mExportAction(Q_NULLPTR)
	, mExportDegreePlotAction(Q_NULLPTR)
	, mExportDegreeVelPlotAction(Q_NULLPTR)
	, mExportDistancePlotAction(Q_NULLPTR)
	, mExportDistanceVelPlotAction(Q_NULLPTR)
	, mSaveAction(Q_NULLPTR)
	, mSaveAsAction(Q_NULLPTR)
	, mPrintAction(Q_NULLPTR)
	, mPreviewAction(Q_NULLPTR)
	, mQuit(Q_NULLPTR)
	, mUndoAction(Q_NULLPTR)
	, mRedoAction(Q_NULLPTR)
	, mClearAction(Q_NULLPTR)
	, mTextAction(Q_NULLPTR)
	, mHelpAction(Q_NULLPTR)
	, mTrackerDetected(false)
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
	//setWindowIcon(QIcon(":/Resources/image/splash_screen.png"));

	initFileActions();
	initViewActions();
	initHelpAction();

	//set window style
	setObjectName("mainWindow");
	setStyleSheet("#mainWindow{background-color:rgb(200, 200, 200);}");
	//set toolbar icon size
	ui->mainToolBar->setIconSize(QSize(32, 32));
}

void MainWindow::buildConnections()
{
	connect(mThreeDAction, &QAction::triggered, m_CentralWidget, &CentralWidget::show3DPage);
	connect(mPlotAction,   &QAction::triggered, m_CentralWidget, &CentralWidget::showPlotPage);
	connect(mExportAction, &QAction::triggered, m_CentralWidget, &CentralWidget::exportReport );
	connect(mExportDegreePlotAction, &QAction::triggered, m_CentralWidget, &CentralWidget::exportDegreePlot);
	connect(mExportDegreeVelPlotAction, &QAction::triggered, m_CentralWidget, &CentralWidget::exportDegreeVelocityPlot);
	connect(mExportDistancePlotAction, &QAction::triggered, m_CentralWidget, &CentralWidget::exportDistancePlot);
	connect(mExportDistanceVelPlotAction, &QAction::triggered, m_CentralWidget, &CentralWidget::exportDistanceVelocityPlot);
	connect(mSaveAction,   &QAction::triggered, m_CentralWidget, &CentralWidget::saveReport);
	connect(mSaveAsAction, &QAction::triggered, m_CentralWidget, &CentralWidget::saveAsReport);
	connect(mPrintAction,  &QAction::triggered, m_CentralWidget, &CentralWidget::printReport);
	connect(mPreviewAction, &QAction::triggered,m_CentralWidget, &CentralWidget::printPreviewReport);
	connect(mClearAction,  &QAction::triggered, m_CentralWidget, &CentralWidget::clearAllPlot);
	connect(mTextAction,   &QAction::triggered, m_CentralWidget, &CentralWidget::showReportPage);
	connect(mHelpAction,   &QAction::triggered,  this, &MainWindow::showHelp);
	connect(mQuit,         &QAction::triggered,  this, &MainWindow::closeWindow);
}

void MainWindow::detectTracker()
{
	if (OpsTrackingDevice::getInstance()->findTracker() == NDIPolaris){

	   OpsTrackingDevice::getInstance()->openConnection();
	   mTrackerDetected = true;
	}else{
	       
		QMessageBox::warning(Q_NULLPTR, QCoreApplication::applicationName(), 
                                        QObject::tr("Cannot find Camera! Please check connection"));
		mTrackerDetected = false;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	disconnectTracker();
    QMainWindow::closeEvent(event);
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

void MainWindow::initFileActions()
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

	const QIcon exportReportIcon = QIcon::fromTheme("exportreport", QIcon(":/Resources/image/export_pdf.png"));
	mExportAction = ui->mainToolBar->addAction(exportReportIcon, tr("&Export Report PDF..."));
	mExportAction->setPriority(QAction::LowPriority);
	mExportAction->setShortcut(Qt::CTRL + Qt::Key_D);

	const QIcon exportPdfIcon = QIcon::fromTheme("exportplot", QIcon(":/Resources/image/export_pdf.png"));
	mExportDegreePlotAction = ui->mainToolBar->addAction(exportPdfIcon, tr("&Export Degree Plot PDF..."));
	mExportDegreePlotAction->setPriority(QAction::LowPriority);

	mExportDegreeVelPlotAction = ui->mainToolBar->addAction(exportPdfIcon, tr("&Export Degree Velocity Plot PDF..."));
	mExportDegreeVelPlotAction->setPriority(QAction::LowPriority);

	mExportDistancePlotAction = ui->mainToolBar->addAction(exportPdfIcon, tr("&Export Distance Plot PDF..."));
	mExportDistancePlotAction->setPriority(QAction::LowPriority);

	mExportDistanceVelPlotAction = ui->mainToolBar->addAction(exportPdfIcon, tr("&Export Distance velocity Plot PDF..."));
	mExportDistanceVelPlotAction->setPriority(QAction::LowPriority);
#endif

}

void MainWindow::initViewActions()
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

void MainWindow::initHelpAction()
{
	const QIcon helpIcon = QIcon::fromTheme("help", QIcon(":/Resources/image/help.png"));
	mHelpAction = ui->mainToolBar->addAction(helpIcon, tr("Help"));

	mQuit = ui->mainToolBar->addAction(QIcon(":/Resources/image/close.png"), tr("&Quit"));
	mQuit->setShortcut(Qt::CTRL + Qt::Key_Q);
}

