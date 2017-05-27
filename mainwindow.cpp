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
    m_CentralWidget = new CentralWidget(this);
    setCentralWidget(m_CentralWidget);
	setWindowTitle("Accelrator Calibration System");
	setWindowIcon(QIcon(":/Resources/image/window_icon.png"));

	mThreeDAction = new QAction(QIcon(":/Resources/image/3d_view.png"), QString::fromLocal8Bit("3D模型"), ui->mainToolBar);
	mPlotAction = new QAction(QIcon(":/Resources/image/plot_view.png"), QString::fromLocal8Bit("波形曲线"), ui->mainToolBar);
	mExportAction = new QAction(QIcon(":/Resources/image/export.png"), QString::fromLocal8Bit("导出成PDF"), ui->mainToolBar);
	mClearAction = new QAction(QIcon(":/Resources/image/clear.xpm"), QString::fromLocal8Bit("清除历史数据"), ui->mainToolBar);
	mPlainTextAction = new QAction(QIcon(":/Resources/image/plainText.xpm"), QString::fromLocal8Bit("加速器运动记录"), ui->mainToolBar);
	mHelpAction = new QAction(QIcon(":/Resources/image/help.png"), QString::fromLocal8Bit("帮助/关于富科斯"), ui->mainToolBar);

	ui->mainToolBar->addAction(mThreeDAction);
	ui->mainToolBar->addAction(mPlotAction);
	ui->mainToolBar->addAction(mExportAction);
	ui->mainToolBar->addAction(mClearAction);
	ui->mainToolBar->addAction(mPlainTextAction);
	ui->mainToolBar->addAction(mHelpAction);
}

void MainWindow::buildConnections()
{
	connect(mThreeDAction, &QAction::triggered, m_CentralWidget, &CentralWidget::show3DPage);
	connect(mPlotAction, &QAction::triggered, m_CentralWidget, &CentralWidget::showPlotPage);
	//connect(mExport, &QAction::triggered, );
	connect(mClearAction, &QAction::triggered, m_CentralWidget, &CentralWidget::clearAllPlot);
	connect(mPlainTextAction, &QAction::triggered, m_CentralWidget, &CentralWidget::showPlainTextPage);
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