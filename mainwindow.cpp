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

	ui->mainToolBar->addAction(QIcon(":/Resources/image/3d_view.png"), "3D View", m_CentralWidget, &CentralWidget::show3DPage);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(QIcon(":/Resources/image/plot_view.png"), "Plot View", m_CentralWidget, &CentralWidget::showPlotPage);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(QIcon(":/Resources/image/export.png"), "Export to PDF");
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(QIcon(":/Resources/image/help.png"), "Help", this, &MainWindow::showHelp);
}

void MainWindow::buildConnections()
{

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