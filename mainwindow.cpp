#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CentralWidget.h"
#include "OpsTrackingDevice.h"
#include <qdesktopservices.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_CentralWidget(Q_NULLPTR)
{
    ui->setupUi(this);

    initUi();
    buildConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUi()
{
    m_CentralWidget = new CentralWidget;
    setCentralWidget(m_CentralWidget);
	setMinimumSize(QSize(1024, 768));
	setWindowTitle("Accelrator Calibration System");
	setWindowIcon(QIcon(":/Resources/image/window_icon.png"));

	mThreeDAction = new QAction(QIcon(":/Resources/image/3d_view.png"), "3D View", ui->mainToolBar);
	mPlotAction = new QAction(QIcon(":/Resources/image/plot_view.png"), "Plot View", ui->mainToolBar);
	mExportAction = new QAction(QIcon(":/Resources/image/export.png"), "Export to PDF", ui->mainToolBar);
	mClearAction = new QAction(QIcon(":/Resources/image/clear.xpm"), "Clear History Data", ui->mainToolBar);
	mHelpAction = new QAction(QIcon(":/Resources/image/help.png"), "Help", ui->mainToolBar);

	ui->mainToolBar->addAction(mThreeDAction);
	ui->mainToolBar->addAction(mPlotAction);
	ui->mainToolBar->addAction(mExportAction);
	ui->mainToolBar->addAction(mClearAction);
	ui->mainToolBar->addAction(mHelpAction);
}

void MainWindow::buildConnections()
{
	connect(mThreeDAction, &QAction::triggered, m_CentralWidget, &CentralWidget::show3DPage);
	connect(mPlotAction, &QAction::triggered, m_CentralWidget, &CentralWidget::showPlotPage);
	//connect(mExport, &QAction::triggered, );
	connect(mClearAction, &QAction::triggered, m_CentralWidget, &CentralWidget::clearAllPlot);
	connect(mHelpAction, &QAction::triggered, this, &MainWindow::showHelp);
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
    OpsTrackingDevice *tracker = OpsTrackingDevice::getInstance();
    if (tracker->getState() >= TrackingDevice::Setup)
    {
        tracker->stopTracking();
    }
    tracker->closeConnection();
}

void MainWindow::showHelp()
{
	QDesktopServices::openUrl(QUrl("http://www.jsfocus.cn"));
}