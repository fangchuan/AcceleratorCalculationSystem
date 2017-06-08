#include "CentralWidget.h"
#include "LoggerView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QTimer>
#include <QSplitter>
#include <QPlainTextEdit>
#include <qmessagebox.h>
#include "vtkMath.h"

CentralWidget::CentralWidget(QWidget *parent)
	: QWidget(parent),
	m_TrackingMode(MarkerTracking3D),
	m_Model(new CentralModel(this)),
	m_Tracker(NULL),
	logger(NULL)
{
	initUi();
	initLogger();
	initCamera();
	initTimer();
	buildConnections();
}

CentralWidget::~CentralWidget()
{
	if (NULL != logger)
		delete logger;
}

void CentralWidget::initUi()
{

	QHBoxLayout *mainLayout = new QHBoxLayout(this);

	QVBoxLayout* leftLayout = new QVBoxLayout();
	m_StackedWidget = new QStackedWidget();
	renderWidget  = new QtRenderView::RenderView();
	QWidget* renderContainer = QWidget::createWindowContainer(renderWidget, m_StackedWidget);
	plotWidget = new PlotView();
	loggerWidget = new LoggerView();
	m_StackedWidget->addWidget(renderContainer);
	m_StackedWidget->addWidget(plotWidget);
	m_StackedWidget->addWidget(loggerWidget);
	leftLayout->addWidget(m_StackedWidget);

	QVBoxLayout *rightLayout = new QVBoxLayout();
	m_DisplayWidget = new DisplayWidget;
	rightLayout->addWidget(m_DisplayWidget);
	rightLayout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Preferred, QSizePolicy::Fixed));
	m_ControlWidget = new ControlWidget;
	rightLayout->addWidget(m_ControlWidget);
	
	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(rightLayout);
	mainLayout->setStretchFactor(leftLayout, 7);
	mainLayout->setStretchFactor(rightLayout, 3);

}

void CentralWidget::initLogger()
{
#ifdef  USE_LOG
	logger = Logger::getInstance();
	//logger->setTextEdit(logText);
	logger->write(QString::fromLocal8Bit("江苏富科思科技有限公司"));
	logger->write(QString::fromLocal8Bit("LNAC setup"));
#endif
}

void CentralWidget::initCamera()
{
	m_Tracker = OpsTrackingDevice::getInstance();
}

void CentralWidget::initTimer()
{
	m_Timer = new QTimer(this);

	if (NULL != m_Tracker)
		m_Timer->setInterval(int(1000.0 / m_Tracker->getFrequency()));
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
	connect(m_ControlWidget, &ControlWidget::resetRequest, this, &CentralWidget::handleReset);
	connect(m_ControlWidget, &ControlWidget::reportRequest, m_Model, &CentralModel::handleReport);
	//建立拟合结果与界面更新信号槽(所有的结果都在加速器坐标系/三维场景坐标系下)
	connect(m_Model, &CentralModel::markerSize, this, &CentralWidget::markerSize);
	connect(m_Model, &CentralModel::markerPosition, this, &CentralWidget::markerPosition);
	connect(m_Model, &CentralModel::horizontalRegisterRecorded, this, &CentralWidget::horizontalRegisterRecorded);
	connect(m_Model, &CentralModel::circleResult, this, &CentralWidget::circleResult);
	connect(m_Model, &CentralModel::translateResult, this, &CentralWidget::translateResult);
	connect(m_Model, &CentralModel::registerPosition, this, &CentralWidget::registerPosition);
	connect(m_Model, &CentralModel::sendReport, this, &CentralWidget::reportResult);

}

void CentralWidget::show3DPage()
{
	m_StackedWidget->setCurrentIndex(STACKED_THREED_VIEW_INDEX);
}

void CentralWidget::showPlotPage()
{

	m_StackedWidget->setCurrentIndex(STACKED_PLOT_VIEW_INDEX);
}

void CentralWidget::showLogTextPage()
{
	m_StackedWidget->setCurrentIndex(STACKED_PLAINTEXT_INDEX);
}

void CentralWidget::exportReport()
{
	loggerWidget->filePrintPdf();
}

void CentralWidget::saveReport()
{
	loggerWidget->fileSave();
}

void CentralWidget::saveAsReport()
{
	loggerWidget->fileSaveAs();

}

void CentralWidget::printReport()
{
	loggerWidget->filePrint();
}

void CentralWidget::printPreviewReport()
{
	loggerWidget->filePrintPreview();
}

void CentralWidget::clearAllPlot()
{
	plotWidget->clearCurveDataVector();
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
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
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
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
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
		QMessageBox::warning(this,QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
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
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
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
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
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
	logger->write(QString::fromLocal8Bit("Recording HorizonRegister"));
}

void CentralWidget::recordingGantry()
{
	m_Model->setHandlerToGantry();
	plotWidget->setGantryUpdateFlag();

	logger->write(QString::fromLocal8Bit("Recording Gantry Rotate"));
}

void CentralWidget::recordingCollimator()
{
	m_Model->setHandlerToCollimator();
	plotWidget->setCollimatorUpdateFlag();
#ifdef USE_LOG
	logger->write(QString::fromLocal8Bit("Recording Collimator Rotate"));
#endif
}

void CentralWidget::recordingBed(int mode)
{
	m_Model->setHandlerToBed(mode);
	if (0 == mode){
		plotWidget->setBedDegreeUpdateFlag();
#ifdef USE_LOG
		logger->write(QString::fromLocal8Bit("Recording Bed Rotate"));
#endif
	}else{
		plotWidget->setBedDistanceUpdateFlag();
#ifdef USE_LOG
		logger->write(QString::fromLocal8Bit("Recording Bed Translate"));
#endif
	}
}

void CentralWidget::recordingISOCenter()
{
	m_Model->setHandlerToISOCenter();
#ifdef USE_LOG
	logger->write(QString::fromLocal8Bit("Recording ISO Center"));
#endif
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

#ifdef USE_LOG
	QString str = QStringLiteral("Horizon has registered!");
	logger->write(str);
#endif
	QMessageBox::information(Q_NULLPTR, QCoreApplication::applicationName(), QString::fromLocal8Bit("水平面已注册"));
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
		renderWidget->drawXAxis(QVector3D(circle_center[0], circle_center[1], circle_center[2]),
								QVector3D(circle_center[0]+normal_vector[0], circle_center[1]+normal_vector[1], circle_center[2]+normal_vector[2]),
								QColor(Qt::blue));

#ifdef USE_LOG
		QString str = QStringLiteral("Gantry rotate result:\n");
		str += QString("angle: %1").arg(angle, 0, 'f', 2);
		str += "\n";
		str += QString("circle: ( %1, %2, %3 )").arg(circle_center[0], 0, 'f', 2).arg(circle_center[1], 0, 'f', 2).arg(circle_center[2], 0, 'f', 2);
		str += "\n";
		str += QString("normal: ( %1, %2, %3 )").arg(normal_vector[0], 0, 'f', 2).arg(normal_vector[1], 0, 'f', 2).arg(normal_vector[2], 0, 'f', 2);
		logger->write(str);
#endif
	}
	if (handler == COLLIMATOR_HANDLER){
		plotWidget->updateCollimatorDegreeDistance(angle);
		plotWidget->updateCollimatorDegreeVelocity(angle);

		renderWidget->rotateCollimator(angle);

#ifdef USE_LOG
		QString str = QStringLiteral("Collimator rotate result:\n");
		str += QString("angle: %1").arg(angle, 0, 'f', 2);
		str += "\n";
		str += QString("circle: ( %1, %2, %3 )").arg(circle_center[0], 0, 'f', 2).arg(circle_center[1], 0, 'f', 2).arg(circle_center[2], 0, 'f', 2);
		str += "\n";
		str += QString("normal: ( %1, %2, %3 )").arg(normal_vector[0], 0, 'f', 2).arg(normal_vector[1], 0, 'f', 2).arg(normal_vector[2], 0, 'f', 2);
		logger->write(str);
#endif
	}
	if (handler == BED_HANDLER){
		plotWidget->updateBedDegreeDistance(angle);
		plotWidget->updateBedDegreeVelocity(angle);

		renderWidget->rotateBed(angle);
		renderWidget->drawYAxis(QVector3D(circle_center[0], circle_center[1], circle_center[2]),
								QVector3D(circle_center[0] + normal_vector[0], circle_center[1] + normal_vector[1], circle_center[2] + normal_vector[2]),
								QColor(Qt::red));

#ifdef USE_LOG
		QString str = QStringLiteral("Bed rotate result:\n");
		str += QString("angle: %1").arg(angle, 0, 'f', 2);
		str += "\n";
		str += QString("circle: ( %1, %2, %3 )").arg(circle_center[0], 0, 'f', 2).arg(circle_center[1], 0, 'f', 2).arg(circle_center[2], 0, 'f', 2);
		str += "\n";
		str += QString("normal: ( %1, %2, %3 )").arg(normal_vector[0], 0, 'f', 2).arg(normal_vector[1], 0, 'f', 2).arg(normal_vector[2], 0, 'f', 2);
		logger->write(str);
#endif
	}
}
//
//机床位移结果
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

#ifdef USE_LOG
	QString str = QStringLiteral("Bed Translate Resulte:\n");
	str += QString("x = %1").arg(x, 0, 'f', 2);
	str += "\n";
	str += QString("y = %1").arg(y, 0, 'f', 2);
	str += "\n";
	str += QString("z = %1").arg(z, 0, 'f', 2);
	logger->write(str);
#endif
}
//
//更新激光灯等中心坐标
//
void CentralWidget::registerPosition(Point3D &point)
{
	double position[3] = { point[0], point[1], point[3] };
	m_DisplayWidget->setRegisteredPosition(position);
	renderWidget->drawLaserISOCenter(position[2]*0.01, position[1]*0.01, -position[0]*0.01);

#ifdef USE_LOG
	QString str = QStringLiteral("Laser register ISOCenter: ( x = %1, y = %2, z = %3 )").arg(point[0], 0, 'f', 2)
																				.arg(point[1], 0, 'f', 2)
																				.arg(point[2], 0, 'f', 2);
	logger->write(str);
#endif
}

void CentralWidget::handleReset()
{
	m_Model->resetAccelerator();
	m_ControlWidget->reset();
	m_DisplayWidget->reset();
	renderWidget->resetScene();
	plotWidget->resetPlot();
}
//
//根据计算得到的等中心、垂足A/B坐标和公垂线长度更新报告和三维场景
//
void CentralWidget::reportResult(const ReportData& report)
{
	QString softCenter = QString("( %1, %2, %3 )").arg(report.softCenter[0], 0, 'f', 2)
												.arg(report.softCenter[1], 0, 'f', 2)
												.arg(report.softCenter[2], 0, 'f', 2);

	QString laserCenter = QString("( %1, %2, %3 )").arg(report.laserCenter[0], 0, 'f', 2)
												.arg(report.laserCenter[1], 0, 'f', 2)
												.arg(report.laserCenter[2], 0, 'f', 2);

	QString foot_A = QString("( %1, %2, %3)").arg(report.footA[0], 0, 'f', 2)
											.arg(report.footA[1], 0, 'f', 2)
											.arg(report.footA[2], 0, 'f', 2);

	QString foot_B = QString("( %1, %2, %3)").arg(report.footB[0], 0, 'f', 2)
											.arg(report.footB[1], 0, 'f', 2)
											.arg(report.footB[2], 0, 'f', 2);

	QString distanceSoft2Laser = QString(" %1").arg(report.distanceLaser2Soft, 0, 'f', 2);

	double distanceA = sqrt(vtkMath::Distance2BetweenPoints(report.footA, report.laserCenter));
	double distanceB = sqrt(vtkMath::Distance2BetweenPoints(report.footB, report.laserCenter));
	QString distanceA2Laser = QString(" %1").arg(distanceA, 0, 'f', 2);
	QString distanceB2Laser = QString(" %1").arg(distanceB, 0, 'f', 2);
	QString gantryVar = QString(" %1").arg(report.gantryVar, 0,'f', 2);
	QString gantryMean = QString(" %1").arg(report.gantryMean, 0, 'f', 2);
	QString bedVar = QString(" %1").arg(report.bedVar, 0, 'f', 2);
	QString bedMean = QString(" %1").arg(report.bedMean, 0, 'f', 2);

	loggerWidget->setHtmlReport(softCenter, laserCenter, foot_A, foot_B, distanceSoft2Laser, distanceA2Laser, distanceB2Laser, gantryVar, gantryMean, bedVar, bedMean);
	renderWidget->drawSoftISOCenter(report.softCenter[2]*0.01, report.softCenter[1]*0.01, -report.softCenter[0]*0.01);
	renderWidget->drawVerticalLine(report.footA, report.footB);
}
