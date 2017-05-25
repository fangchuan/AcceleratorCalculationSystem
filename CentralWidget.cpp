#include "CentralWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QTimer>
#include <QSplitter>
#include <qmessagebox.h>

CentralWidget::CentralWidget(QWidget *parent)
	: QWidget(parent),
	m_TrackingMode(MarkerTracking3D),
	m_Model(new CentralModel(this))
{
	m_Tracker = OpsTrackingDevice::getInstance();
	m_Timer = new QTimer(this);
	m_Timer->setInterval(int(1000.0 / m_Tracker->getFrequency()));
	initUi();
	buildConnections();
}

CentralWidget::~CentralWidget()
{
}

void CentralWidget::initUi()
{

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QSplitter *splitter = new QSplitter(Qt::Horizontal);
	mainLayout->addWidget(splitter);
	m_StackedWidget = new QStackedWidget();
	renderWidget  = new QtRenderView::RenderView();
	QWidget* renderContainer = QWidget::createWindowContainer(renderWidget, m_StackedWidget);
	plotWidget = new PlotView();
	m_StackedWidget->addWidget(renderContainer);
	m_StackedWidget->addWidget(plotWidget);
	m_StackedWidget->setMinimumSize(QSize(800, 600));
	splitter->addWidget(m_StackedWidget);

	QWidget *rightWidget = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(rightWidget);
	m_DisplayWidget = new DisplayWidget;
	layout->addWidget(m_DisplayWidget);
	layout->addSpacerItem(new QSpacerItem(1, 40, QSizePolicy::Preferred, QSizePolicy::Fixed));
	m_ControlWidget = new ControlWidget;
	layout->addWidget(m_ControlWidget);
	splitter->addWidget(rightWidget);
}

void CentralWidget::buildConnections()
{
	connect(m_Tracker, &OpsTrackingDevice::OperationModeChanged, this, &CentralWidget::TrackingModeChanged);
	connect(m_Timer, &QTimer::timeout, this, &CentralWidget::monitoring);

	//建立控制面板与界面更新信号槽
	connect(m_ControlWidget, &ControlWidget::recordingHorizontalRegister, this, &CentralWidget::recordingHorizontalRegister);
	connect(m_ControlWidget, &ControlWidget::switchToHorizontalRegister, this, &CentralWidget::switchToHorizontalRegister);
	connect(m_ControlWidget, &ControlWidget::recordingGantry, this, &CentralWidget::recordingGantry);
	connect(m_ControlWidget, &ControlWidget::switchToGantry, this, &CentralWidget::switchToGantry);
	connect(m_ControlWidget, &ControlWidget::recordingCollimator, this, &CentralWidget::recordingCollimator);
	connect(m_ControlWidget, &ControlWidget::switchToCollimator, this, &CentralWidget::switchToCollimator);
	connect(m_ControlWidget, &ControlWidget::recordingBed, this, &CentralWidget::recordingBed);
	connect(m_ControlWidget, &ControlWidget::switchToBed, this, &CentralWidget::switchToBed);
	connect(m_ControlWidget, &ControlWidget::recordingISOCenter, this, &CentralWidget::recordingISOCenter);
	connect(m_ControlWidget, &ControlWidget::switchToISOCenter, this, &CentralWidget::switchToISOCenter);

	//建立拟合结果与界面更新信号槽
	connect(m_Model, &CentralModel::markerSize, this, &CentralWidget::markerSize);
	connect(m_Model, &CentralModel::markerPosition, this, &CentralWidget::markerPosition);
	connect(m_Model, &CentralModel::horizontalRegisterRecorded, this, &CentralWidget::horizontalRegisterRecorded);
	connect(m_Model, &CentralModel::circleResult, this, &CentralWidget::circleResult);
	connect(m_Model, &CentralModel::translateResult, this, &CentralWidget::translateResult);
	connect(m_Model, &CentralModel::registerPosition, this, &CentralWidget::registerPosition);
}

void CentralWidget::show3DPage()
{
	if (NULL != m_StackedWidget){
		m_StackedWidget->setCurrentIndex(STACKED_THREED_VIEW_INDEX);
	}
}

void CentralWidget::showPlotPage()
{
	if (NULL != m_StackedWidget){
		m_StackedWidget->setCurrentIndex(STACKED_PLOT_VIEW_INDEX);
	}
}

void CentralWidget::TrackingModeChanged(OperationMode mode)
{
	m_TrackingMode = mode;
}

void CentralWidget::monitoring()
{
	switch (m_TrackingMode) {
	case ToolTracking6D:
		monitoringTool6D();
		break;
	case MarkerTracking3D:
		monitoringMarker3D();
		break;
	default:
		break;
	}
}

void CentralWidget::switchToHorizontalRegister() 
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(MarkerTracking3D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
	m_ControlWidget->doSwitchToHorizontalRegister();
	m_DisplayWidget->doSwitchToHorizontalRegister();
	m_Model->setHandlerToNone();
}

void CentralWidget::switchToGantry() 
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(MarkerTracking3D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
	m_ControlWidget->doSwitchToGantry();
	m_DisplayWidget->doSwitchToGantry();
	m_Model->setHandlerToNone();
}

void CentralWidget::switchToCollimator()
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(MarkerTracking3D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
	m_ControlWidget->doSwitchToCollimator();
	m_DisplayWidget->doSwitchToCollimator();
	m_Model->setHandlerToNone();
}

void CentralWidget::switchToBed(int mode)
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(MarkerTracking3D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
    m_ControlWidget->doSwitchToBed();
	m_DisplayWidget->doSwitchToBed(mode);

	m_Model->setHandlerToNone();
}

void CentralWidget::switchToISOCenter()
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(ToolTracking6D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
	m_ControlWidget->doSwitchToISOCenter();
	m_DisplayWidget->doSwitchToISOCenter();
	m_Model->setHandlerToNone();
}

void CentralWidget::recordingHorizontalRegister()
{
	m_Model->setHandlerToHorizontalRegister();

	//MarkerPointContainerType positions;
	//m_Tracker->getMarkerPositions(&positions);
	//m_Model->handle(positions);
}

void CentralWidget::recordingGantry()
{
	m_Model->setHandlerToGantry();
	plotWidget->setGantryUpdateFlag();
}

void CentralWidget::recordingCollimator()
{
	m_Model->setHandlerToCollimator();
	plotWidget->setCollimatorUpdateFlag();
}

void CentralWidget::recordingBed(int mode)
{
	m_Model->setHandlerToBed(mode);
	if (0 == mode){
		plotWidget->setBedDegreeUpdateFlag();
	}
	else{
		plotWidget->setBedDistanceUpdateFlag();
	}
}

void CentralWidget::recordingISOCenter()
{
	m_Model->setHandlerToISOCenter();
}

void CentralWidget::monitoringMarker3D()
{
	MarkerPointContainerType positions;
	m_Tracker->getMarkerPositions(&positions);
	m_Model->handle(positions);
}

void CentralWidget::monitoringTool6D()
{
	Point3D point;
	m_Tracker->getToolPosition(point);
	m_Model->handle(point);
}
//
//update display widget and render widget
//
void CentralWidget::markerSize(int size)
{
	m_DisplayWidget->setMarkerSize(size);
}
//
//在显示面板和3D界面更新小球位置
//
void CentralWidget::markerPosition(MarkerPointType &point)
{
	double x = point[0] * 0.01;
	double y = point[1] * 0.01;
	double z = point[2] * 0.01;

	m_DisplayWidget->setMarkerPosition(point);

	renderWidget->drawMarkerPoint(z, y, -x);
}
//
//显示面板显示治疗床是否水平
//
void CentralWidget::horizontalRegisterRecorded()
{
	m_DisplayWidget->horizontalRegisterRecorded();
	QMessageBox::information(Q_NULLPTR, QString::fromLocal8Bit("Information"), QString::fromLocal8Bit("水平面已注册"));
}
//
//在曲线图表和显示面板更新
//
void CentralWidget::circleResult(Circle *circle)
{
	double angle = circle->Angle;
	double normal_vector[3] = { (*circle).Normal[0], (*circle).Normal[1], (*circle).Normal[2]};
	double circle_center[3] = { (*circle).Center[0]*0.01, (*circle).Center[1]*0.01, (*circle).Center[2]*0.01 };

	m_DisplayWidget->setCircleResult(circle);

	int handler = m_Model->getHandler();
	if (handler == GANTRY_HANDLER){
		plotWidget->updateGantryDegreeDistance(angle);
		plotWidget->updateGantryDegreeVelocity(angle);

		renderWidget->rotateGantry(angle);
		renderWidget->drawXAxis(QVector3D(circle_center[2]-5, circle_center[1], -circle_center[0]),
								QVector3D(circle_center[2]+5, circle_center[1], -circle_center[0]),
								QColor(Qt::blue));

	}
	if (handler == COLLIMATOR_HANDLER){
		plotWidget->updateCollimatorDegreeDistance(angle);
		plotWidget->updateCollimatorDegreeVelocity(angle);

		renderWidget->rotateCollimator(angle);
	}
	if (handler == BED_HANDLER){
		plotWidget->updateBedDegreeDistance(angle);
		plotWidget->updateBedDegreeVelocity(angle);

		renderWidget->rotateBed(angle);
		renderWidget->drawYAxis(QVector3D(circle_center[0], circle_center[1]-5, circle_center[2]),
								QVector3D(circle_center[0], circle_center[1]+5, circle_center[2]),
								QColor(Qt::red));
	}

}
//
//
//
void CentralWidget::translateResult(double bias[3])
{
	double x = bias[0];
	double y = bias[1];
	double z = bias[2];

	m_DisplayWidget->setTranslateResult(bias);

	renderWidget->translateBedAlongX(x);
	renderWidget->translateBedAlongY(y);
	renderWidget->translateBedAlongZ(z);

	plotWidget->updateBedDistance(x, y, z);
	plotWidget->updateBedVelocity(x, y, z);
}
//
//在显示面板更新激光灯确定的等中心坐标
//
void CentralWidget::registerPosition(Point3D &point)
{
	double position[3] = { point[0], point[1], point[3] };
	m_DisplayWidget->setRegisteredPosition(position);
	renderWidget->drawISOCenter(position[2], position[1], -position[0]);
}
