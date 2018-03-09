#include "centralwidget.h"
#include "reportview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QTimer>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QMessageBox>

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
	reportWidget = new ReportView();
	m_StackedWidget->addWidget(renderContainer);
	m_StackedWidget->addWidget(plotWidget);
	m_StackedWidget->addWidget(reportWidget);
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
	connect(m_ControlWidget, &ControlWidget::switchToCbct, this, &CentralWidget::switchToCbct);
	connect(m_ControlWidget, &ControlWidget::recordingCbct, this, &CentralWidget::recordingCbct);
	connect(m_ControlWidget, &ControlWidget::recordingBed, this, &CentralWidget::recordingBed);
	connect(m_ControlWidget, &ControlWidget::switchToBed, this, &CentralWidget::switchToBed);
	connect(m_ControlWidget, &ControlWidget::recordingLaserISO, this, &CentralWidget::recordingLaserISO);
	connect(m_ControlWidget, &ControlWidget::switchToLaserISO, this, &CentralWidget::switchToLaserISO);
	connect(m_ControlWidget, &ControlWidget::recordingLightCenter, this, &CentralWidget::recordingLightCenter);
	connect(m_ControlWidget, &ControlWidget::switchToLightCenter, this, &CentralWidget::switchToLightCenter);
	connect(m_ControlWidget, &ControlWidget::resetRequest, this, &CentralWidget::handleReset);
	connect(m_ControlWidget, &ControlWidget::reportRequest, m_Model, &CentralModel::handleReport);
	//建立拟合结果与界面更新信号槽(所有的结果都在加速器坐标系/三维场景坐标系下)
	connect(m_Model, &CentralModel::markerSize, this, &CentralWidget::markerSize);
	connect(m_Model, &CentralModel::pseudoMarkerSize, this, &CentralWidget::pseudoMarkerSize);
	connect(m_Model, &CentralModel::markerPosition, this, &CentralWidget::markerPosition);
	connect(m_Model, &CentralModel::horizontalRegisterRecorded, this, &CentralWidget::horizontalRegisterRecorded);
	connect(m_Model, &CentralModel::horizontalRegisterFailed, this, &CentralWidget::horizontalRegisterFailed);
	connect(m_Model, &CentralModel::loadHorizontalRegisterData, this, &CentralWidget::loadHorizontalRegisterData);
	connect(m_Model, &CentralModel::circleResult, this, &CentralWidget::circleResult);
	connect(m_Model, &CentralModel::translateResult, this, &CentralWidget::translateResult);
	connect(m_Model, &CentralModel::registerLaserISO, this, &CentralWidget::registerLaserISOPosition);
	connect(m_Model, &CentralModel::registerLightCenter, this, &CentralWidget::registerLightCenterPosition);
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

void CentralWidget::showReportPage()
{
	m_StackedWidget->setCurrentIndex(STACKED_REPORT_INDEX);
}

void CentralWidget::exportReport()
{
	reportWidget->filePrintPdf();
}

void CentralWidget::saveReport()
{
	reportWidget->fileSave();
}

void CentralWidget::saveAsReport()
{
	reportWidget->fileSaveAs();

}

void CentralWidget::printReport()
{
	reportWidget->filePrint();
}

void CentralWidget::printPreviewReport()
{
	reportWidget->filePrintPreview();
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

void CentralWidget::switchToLaserISO()
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(ToolTracking6D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
	m_ControlWidget->doSwitchToLaserISO();
	m_DisplayWidget->doSwitchToISOCenter();
	m_Model->setHandlerToNone();
}

void CentralWidget::switchToLightCenter()
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(ToolTracking6D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
	m_ControlWidget->doSwitchToLightCenter();
	m_DisplayWidget->doSwitchToISOCenter();
	m_Model->setHandlerToNone();
}

void CentralWidget::switchToCbct()
{
	if (m_Tracker->getState() < TrackingDevice::Ready) {
		QMessageBox::warning(this, QCoreApplication::applicationName(), QString::fromLocal8Bit("摄像机未连接或已连接未就绪！"));
		return;
	}

	m_Tracker->startTrackingInMode(MarkerTracking3D);
	if (!m_Timer->isActive()) {
		m_Timer->start();
	}
	m_ControlWidget->doSwitchToCbct();
	m_DisplayWidget->doSwitchToCbct();
	m_Model->setHandlerToNone();
}

void CentralWidget::recordingHorizontalRegister()
{
	m_Model->setHandlerToHorizontalRegister();

	logger->write(QString::fromLocal8Bit("Recording horizontal plane register"));
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

void CentralWidget::recordingCbct()
{
	m_Model->setHandlerToCbct();
	//plotWidget->setCbctUpdateFlag();
#ifdef USE_LOG
	logger->write(QString::fromLocal8Bit("Recording CBCT Rotate"));
#endif

}

void CentralWidget::recordingBed(int mode)
{
	m_Model->setHandlerToBed(mode);

	switch (mode)
	{
	case 0:
		plotWidget->setEmptyBedDegreeUpdateFlag();
#ifdef USE_LOG
		logger->write(QString::fromLocal8Bit("Recording Bed Rotate on no payload condition: "));
#endif
		break;
	case 1:
		plotWidget->setEmptyBedDistanceUpdateFlag();
#ifdef USE_LOG
		logger->write(QString::fromLocal8Bit("Recording Bed Translate on no payload condition: "));
#endif
		break;
	case 2:
		plotWidget->setPayloadBedDegreeUpdateFlag();
#ifdef USE_LOG
		logger->write(QString::fromLocal8Bit("Recording Bed Rotate on payload condition: "));
#endif
		break;
	case 3:
		plotWidget->setPayloadBedDistanceUpdateFlag();
#ifdef USE_LOG
		logger->write(QString::fromLocal8Bit("Recording Bed Translate on payload condition: "));
#endif
		break;
	default:
		break;
	}
}

void CentralWidget::recordingLaserISO()
{
	m_Model->setHandlerToISOCenter();
#ifdef USE_LOG
	logger->write(QString::fromLocal8Bit("Recording Laser ISO Center"));
#endif
}

void CentralWidget::recordingLightCenter()
{
	m_Model->setHandlerToLightCenter();
#ifdef USE_LOG
	logger->write(QString::fromLocal8Bit("Recording Light Center"));
#endif
}

void CentralWidget::monitoringMarker3D()
{
	MarkerPointContainerType positions;
	m_Tracker->getMarkerPositions(&positions);

	//int size = positions.size();
	//for (int i = 0; i < size; i++){
	//	Point3D p = positions.at(i);
	//	double x = p[0];
	//	double y = p[1];
	//	double z = p[2];
	//	QString str = QString("%1 : ( %2, %3, %4)").arg(i).arg(x, 0, 'f', 2).arg(y, 0, 'f', 2).arg(z, 0, 'f', 2);
	//	qDebug() << str;
	//}
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

void CentralWidget::pseudoMarkerSize(int size)
{
	m_DisplayWidget->setMarkerSize(size);
    QString str;
    str += QString("find pseudo marker points: %1").arg(size);
#ifdef USE_LOG
	logger->write(str);
#endif
    QMessageBox::information(Q_NULLPTR, QCoreApplication::applicationName(), QString::fromStdString(str.toStdString()));
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
	QString str = QStringLiteral("Horizontal plane has registered!");
	logger->write(str);
#endif
	QMessageBox::information(Q_NULLPTR, QCoreApplication::applicationName(), QString::fromLocal8Bit("水平面注册成功!"));
}

void CentralWidget::horizontalRegisterFailed()
{
#ifdef USE_LOG
	QString str = QStringLiteral("Horizontal plane registered Failed, check for horizontalRegister or record register data again!");
	logger->write(str);
#endif
	QMessageBox::information(Q_NULLPTR, QCoreApplication::applicationName(), QString::fromLocal8Bit("水平面注册失败，请检查水平仪或重新记录注册数据!\n如需记录注册数据请再次点击记录水平面按钮"));
}
void CentralWidget::loadHorizontalRegisterData(int index)
{
	if (index == 1) {
#ifdef USE_LOG
		QString str = QStringLiteral("Horizontal plane register data 1 recorded, please hit the button again to record data 2!");
		logger->write(str);
#endif
		QMessageBox::information(Q_NULLPTR, QCoreApplication::applicationName(), QString::fromLocal8Bit("1号水平面数据点记录成功！\n请点击记录水平面按钮记录2号数据点。"));
	}
	if (index == 2) {
#ifdef USE_LOG
		QString str = QStringLiteral("Horizontal plane register data 2 recorded, please hit the button again to record data 3!");
		logger->write(str);
#endif
		QMessageBox::information(Q_NULLPTR, QCoreApplication::applicationName(), QString::fromLocal8Bit("2号水平面数据点记录成功！\n请点击记录水平面按钮记录3号数据点。"));
	}
	if (index == 3) {
#ifdef USE_LOG
		QString str = QStringLiteral("Horizontal plane register data 3 recorded, complete horizontal plane register data record!");
		logger->write(str);
#endif
		QMessageBox::information(Q_NULLPTR, QCoreApplication::applicationName(), QString::fromLocal8Bit("3号水平面数据点记录成功！\n完成水平面数据点记录，请拿掉小球遮挡套。"));
	}

}
//
//在曲线图表和显示面板更新
//
void CentralWidget::circleResult(Circle *circle)
{
	double angle = circle->Angle;
	double normal_vector[3] = { (*circle).Normal[0], (*circle).Normal[1], (*circle).Normal[2]};
	double circle_center[3] = { (*circle).Center[0]*0.01, (*circle).Center[1]*0.01, (*circle).Center[2]*0.01 };
	double normalx10_vector[3] = { normal_vector[0] * 10, normal_vector[1] * 10, normal_vector[2]*10 };

	m_DisplayWidget->setCircleResult(circle);

	int handler = m_Model->getHandler();
	//更新机架运动
	if (handler == GANTRY_HANDLER){
		plotWidget->updateGantryDegree(angle);
		renderWidget->rotateGantry(angle);
		renderWidget->drawGantryAxis(
			QVector3D(circle_center[0] + normalx10_vector[0], circle_center[1] + normalx10_vector[1], circle_center[2] + normalx10_vector[2]),
			QVector3D(circle_center[0] - normalx10_vector[0], circle_center[1] - normalx10_vector[1], circle_center[2] - normalx10_vector[2]),
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
	//更新准直器运动
	if (handler == COLLIMATOR_HANDLER){
		plotWidget->updateCollimatorDegree(angle);
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
	//更新CBCT运动
	if (handler == CBCT_HANDLER) {
		plotWidget->updateCollimatorDegree(angle);
		renderWidget->rotateCollimator(angle);

#ifdef USE_LOG
		QString str = QStringLiteral("CBCT rotate result:\n");
		str += QString("angle: %1").arg(angle, 0, 'f', 2);
		str += "\n";
		str += QString("circle: ( %1, %2, %3 )").arg(circle_center[0], 0, 'f', 2).arg(circle_center[1], 0, 'f', 2).arg(circle_center[2], 0, 'f', 2);
		str += "\n";
		str += QString("normal: ( %1, %2, %3 )").arg(normal_vector[0], 0, 'f', 2).arg(normal_vector[1], 0, 'f', 2).arg(normal_vector[2], 0, 'f', 2);
		logger->write(str);
#endif
	}
	//更新治疗床运动
	if (handler == BED_HANDLER){
		plotWidget->updateBedDegree(angle);

		renderWidget->rotateBed(angle);
		renderWidget->drawBedAxis(
			QVector3D(circle_center[0] + normalx10_vector[0], circle_center[1] + normalx10_vector[1], circle_center[2] + normalx10_vector[2]),
			QVector3D(circle_center[0] - normalx10_vector[0], circle_center[1] - normalx10_vector[1], circle_center[2] - normalx10_vector[2]),
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
void CentralWidget::registerLaserISOPosition(Point3D &point)
{
	double position[3] = { point[0], point[1], point[3] };
	m_DisplayWidget->setRegisteredPosition(position);
	renderWidget->drawLaserISOCenter(position[2]*0.01, position[1]*0.01, -position[0]*0.01);

#ifdef USE_LOG
	QString str = QStringLiteral("Laser ISOCenter Position: ( x = %1, y = %2, z = %3 )").arg(point[0], 0, 'f', 2)
									.arg(point[1], 0, 'f', 2)
									.arg(point[2], 0, 'f', 2);
	logger->write(str);
#endif
}
//
//更新模拟光野中心坐标
//
void CentralWidget::registerLightCenterPosition(Point3D &point)
{
	double position[3] = { point[0], point[1], point[3] };
	m_DisplayWidget->setRegisteredPosition(position);
	renderWidget->drawLightCenter(position[2] * 0.01, position[1] * 0.01, -position[0] * 0.01);

#ifdef USE_LOG
	QString str = QStringLiteral("Light Center Position: ( x = %1, y = %2, z = %3 )").arg(point[0], 0, 'f', 2)
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

	QString lightCenter = QString("( %1, %2, %3 )").arg(report.lightCenter[0], 0, 'f', 2)
												.arg(report.lightCenter[1], 0, 'f', 2)
												.arg(report.lightCenter[2], 0, 'f', 2);

	QString foot_A = QString("( %1, %2, %3)").arg(report.footA[0], 0, 'f', 2)
											.arg(report.footA[1], 0, 'f', 2)
											.arg(report.footA[2], 0, 'f', 2);

	QString foot_B = QString("( %1, %2, %3)").arg(report.footB[0], 0, 'f', 2)
											.arg(report.footB[1], 0, 'f', 2)
											.arg(report.footB[2], 0, 'f', 2);

	QString distanceSoft2Laser = QString(" %1 mm").arg(report.distanceLaser2Soft, 0, 'f', 2);

	double distanceA = sqrt(vtkMath::Distance2BetweenPoints(report.footA, report.laserCenter));
	double distanceB = sqrt(vtkMath::Distance2BetweenPoints(report.footB, report.laserCenter));
	QString distanceA2Laser = QString(" %1 mm").arg(distanceA, 0, 'f', 2);
	QString distanceB2Laser = QString(" %1 mm").arg(distanceB, 0, 'f', 2);
	QString gantryVar = QString(" %1").arg(report.gantryVar, 0,'f', 2);
	QString gantryMean = QString(" %1 mm").arg(report.gantryMean, 0, 'f', 2);
	QString bedVar = QString(" %1").arg(report.bedVar, 0, 'f', 2);
	QString bedMean = QString(" %1 mm").arg(report.bedMean, 0, 'f', 2);

	double gantryVelocity = plotWidget->getGantryAvrDegreeVelocity();
	double bedVelocity = plotWidget->getBedAvrDegreeVelocity();
	QString gantryVel = QString(" %1  degree/s").arg(gantryVelocity, 0, 'f', 2);
	QString bedVel = QString(" %1  degree/s").arg(bedVelocity, 0, 'f', 2);


	reportWidget->setHtmlReport(softCenter, laserCenter, lightCenter, foot_A, foot_B, distanceSoft2Laser, distanceA2Laser, distanceB2Laser, 
															gantryVar, gantryMean, gantryVel, bedVar, bedMean, bedVel);
	renderWidget->drawSoftISOCenter(report.softCenter[2]*0.01, report.softCenter[1]*0.01, -report.softCenter[0]*0.01);
	renderWidget->drawVerticalLine(report.footA, report.footB);
}
