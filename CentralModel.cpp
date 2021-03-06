#include "centralmodel.h"
#include "opstrackingdevice.h"
#include "abstractmonitorhandler.h"
#include "horizontalregisterhandler.h"
#include "gantryhandler.h"
#include "collimatorhandler.h"
#include "bedhandler.h"
#include "isocenterhandler.h"
#include "lightcenterhandler.h"
#include "epidhandler.h"
#include "cbctpositionhandler.h"
#include "circle.h"


#include <QDebug>
#include "vtkLine.h"
#include "vtkMath.h"

CentralModel::CentralModel(QObject *parent)
	: QObject(parent),
	m_Handler(nullptr),
	m_HorizontalRegisterHandler(new HorizontalRegisterHandler(this)),
	m_GantryHandler(new GantryHandler(this)),
	m_CollimatorHandler(new CollimatorHandler(this)),
	m_BedHandler(new BedHandler(this)),
	m_ISOCenterHanlder(new ISOCenterHandler(this)),
	m_LightCenterHanlder(new LightCenterHandler(this)),
	m_EpidPositionHandler(new EpidHandler(this)),
	m_CbctPositionHandler(new CbctPositionHandler(this)),
	gantryCircle(new Circle),
	bedCircle(new Circle)
{
	initTimer();
	initData();
	buildConnections();
}

bool CentralModel::initTimer()
{
	int timerId = this->startTimer(1000);
	if (timerId != 0)
		return true;
	else
		return false;
}
void CentralModel::initData()
{
	m_BedFitted = false;
	m_GantryFitted = false;
	m_softCenterIsCal = false;
	m_LaserISODetected = false;
	//m_LightCenterDetected = false;
	m_ReadyToReport = false;
	gantryCircle ->init();
	bedCircle -> init();
	reportData.softCenter[0] = 0; reportData.softCenter[1] = 0; reportData.softCenter[2] = 0;
	reportData.laserCenter[0] = 0; reportData.laserCenter[1] = 0; reportData.laserCenter[2] = 0;
	reportData.cbctPlaneVerticalty = 0;
	reportData.epidPlaneVerticalty = 0;
	reportData.footA[0] = 0; reportData.footA[1] = 0; reportData.footA[2] = 0;
	reportData.footB[0] = 0; reportData.footB[1] = 0; reportData.footB[2] = 0;
	reportData.distanceLaser2Soft = 0;
	reportData.bedVar = 0; reportData.bedMean = 0;
	reportData.gantryVar = 0; reportData.gantryMean = 0;
}

CentralModel::~CentralModel()
{
	delete gantryCircle;
	delete bedCircle;
}

void CentralModel::buildConnections()
{
	connect(m_HorizontalRegisterHandler, &HorizontalRegisterHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_HorizontalRegisterHandler, &HorizontalRegisterHandler::horizontalRegisterRecorded, this, &CentralModel::horizontalRegisterRecorded);
	connect(m_HorizontalRegisterHandler, &HorizontalRegisterHandler::horizontalRegisterFailed, this, &CentralModel::horizontalRegisterFailed);
	connect(m_HorizontalRegisterHandler, &HorizontalRegisterHandler::loadHorizontalRegisterData, this, &CentralModel::loadHorizontalRegisterData);
	connect(m_GantryHandler, &GantryHandler::pseudoMarkerSize, this, &CentralModel::pseudoMarkerSize);
	connect(m_GantryHandler, &GantryHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_GantryHandler, &GantryHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_CollimatorHandler, &CollimatorHandler::pseudoMarkerSize, this, &CentralModel::pseudoMarkerSize);
	connect(m_CollimatorHandler, &CollimatorHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_CollimatorHandler, &CollimatorHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_BedHandler, &BedHandler::pseudoMarkerSize, this, &CentralModel::pseudoMarkerSize);
	connect(m_BedHandler, &BedHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_BedHandler, &BedHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_BedHandler, &BedHandler::translateResult, this, &CentralModel::translateResult);
	connect(m_ISOCenterHanlder, &ISOCenterHandler::registerLaserISO, this, &CentralModel::registerLaserISO);
	connect(m_ISOCenterHanlder, &ISOCenterHandler::registerLaserISOSuccess, this, &CentralModel::registerLaserISOSuccess); 
	connect(m_LightCenterHanlder, &LightCenterHandler::registerLightCenter, this, &CentralModel::registerLightCenter);
	connect(m_EpidPositionHandler, &EpidHandler::epidPlanePointPosition, this, &CentralModel::epidPointPosition);
	connect(m_EpidPositionHandler, &EpidHandler::registerToolTrackError, this, &CentralModel::registerToolTrackError);
	connect(m_EpidPositionHandler, &EpidHandler::epidPlaneResult, this, &CentralModel::epidPlaneResult);
	connect(m_CbctPositionHandler, &CbctPositionHandler::cbctPlanePointPosition, this, &CentralModel::cbctPointPosition);
	connect(m_CbctPositionHandler, &CbctPositionHandler::registerToolTrackError, this, &CentralModel::registerToolTrackError);
	connect(m_CbctPositionHandler, &CbctPositionHandler::cbctPlaneResult, this, &CentralModel::cbctPlaneResult);
	
	connect(m_ISOCenterHanlder, &ISOCenterHandler::registerLaserISO, this, &CentralModel::laserISOCenterPosition);
	connect(m_LightCenterHanlder, &LightCenterHandler::registerLightCenter, this, &CentralModel::lightCenterPosition);
	connect(m_CbctPositionHandler, &CbctPositionHandler::cbctPlaneResult, this, &CentralModel::cbctPlanePosition);
	connect(m_EpidPositionHandler, &EpidHandler::epidPlaneResult, this, &CentralModel::epidPlanePosition);
	connect(m_BedHandler, &BedHandler::circleResult, this, &CentralModel::softISOCenter);
	connect(m_GantryHandler, &GantryHandler::circleResult, this, &CentralModel::softISOCenter);
}
//
//因为槽函数中不能发射信号，所以把sendreport动作放在定时器事件中
//
void CentralModel::timerEvent(QTimerEvent* event)
{
	Q_UNUSED(event);

	if (m_ReadyToReport){
		m_ReadyToReport = false;
		emit sendReport(reportData);
	}
}
void CentralModel::setHandlerToNone()
{
	m_Handler = nullptr;
}

void CentralModel::setHandlerToHorizontalRegister()
{
	m_Handler = m_HorizontalRegisterHandler;
	//clear history data
	m_Handler->reset();
}

void CentralModel::setHandlerToGantry()
{
	m_Handler = m_GantryHandler;
	//clear history data
	m_Handler->reset();
}

void CentralModel::setHandlerToCollimator()
{
	m_Handler = m_CollimatorHandler;
	//clear  history data
	m_Handler->reset();
}

void CentralModel::setHandlerToBed(int mode)
{
	m_Handler = m_BedHandler;
	m_BedHandler->setMode(mode);
	//clear history data
	m_Handler->reset();
}

void CentralModel::setHandlerToISOCenter()
{
	m_Handler = m_ISOCenterHanlder;
	//clear history data
	m_Handler->reset();
}

void CentralModel::setHandlerToLightCenter()
{
	m_Handler = m_LightCenterHanlder;
	//clear history data
	m_Handler->reset();
}

void CentralModel::setHandlerToCbctPosition()
{
	m_Handler = m_CbctPositionHandler;
	m_Handler->reset();
}

void CentralModel::setHandlerToEpidPosition()
{
	m_Handler = m_EpidPositionHandler;
	m_Handler->reset();
}


int CentralModel::getHandler()
{
	if (m_Handler == m_HorizontalRegisterHandler){
		return  HORIZONTALREGISTER_HANDLER;
	}
	else{
		if (m_Handler == m_GantryHandler){
			return  GANTRY_HANDLER;
		}
		else{
			if (m_Handler == m_CollimatorHandler){
				return  COLLIMATOR_HANDLER;
			}
			else{
				if (m_Handler == m_BedHandler){
					return  BED_HANDLER;
				}
				else{
					if (m_Handler == m_ISOCenterHanlder){
						return  ISOCENTER_HANDLER;
					}else{
						if (m_Handler == m_EpidPositionHandler)
							return EPID_POSITION_HANDLER;
						else
							if (m_Handler == m_CbctPositionHandler) {
								return CBCT_POSITION_HANDLER;
							}
							else
							{
								return LIGHTCENTER_HANDLER;
							}
							
					}
				}
			}
		}
	}
}
//
//当没有接收到拟合/计算信号时，只显示球的数量
//
void CentralModel::handle(MarkerPointContainerType &positions)
{
	if (m_Handler) {
		m_Handler = m_Handler->handle(positions);
	}
	else {
		emit markerSize(positions.size());
	}
}

void CentralModel::handle(Point3D &point)
{
	if (m_Handler) {
		m_Handler = m_Handler->handle(point);
	}
}
//
//记录激光灯的等中心
//
void CentralModel::laserISOCenterPosition(Point3D& point)
{
	reportData.laserCenter[0] = point[0];
	reportData.laserCenter[1] = point[1];
	reportData.laserCenter[2] = point[2];

	m_LaserISODetected = true;
}
//
//记录模拟光野中心
//
void CentralModel::lightCenterPosition(Point3D& point)
{
	reportData.laserCenter[0] = point[0];
	reportData.laserCenter[1] = point[1];
	reportData.laserCenter[2] = point[2];

	m_LaserISODetected = true;
}
void CentralModel::cbctPlanePosition(Plane_T& plane)
{
	reportData.cbctPlaneVerticalty = plane.angleBetweenP2H;

}

void CentralModel::epidPlanePosition(Plane_T& plane)
{
	reportData.epidPlaneVerticalty = plane.angleBetweenP2H;

}
//
//记录软件结算的等中心
//
void CentralModel::softISOCenter(Circle* circle)
{
	int handler = getHandler();
	if (handler == GANTRY_HANDLER){
		m_GantryFitted = true;
		memcpy(gantryCircle, circle, sizeof(Circle));
	}
	if (handler == BED_HANDLER){
		m_BedFitted = true;
		memcpy(bedCircle, circle, sizeof(Circle));
	}

	if (m_GantryFitted && m_BedFitted){
		calSoftISOCenterAndFoots(gantryCircle, bedCircle);
	}
}
//Gantry Center Line Equation: 
//							x-Center[0]    y-Center[1]     z-Center[2]
//							___________ =  ___________  =  ___________
//						     Normal[0]      Normal[1]       Normal[2]
//Bed Center Line Equation:
//							x-Center[0]    y-Center[1]     z-Center[2]
//							___________ =  ___________  =  ___________
//						     Normal[0]      Normal[1]       Normal[2]
void CentralModel::calSoftISOCenterAndFoots(Circle* gantry, Circle* bed)
{
	double gantryCenter_x = gantry->Center[0];
	double gantryCenter_y = gantry->Center[1];
	double gantryCenter_z = gantry->Center[2];
	double gantryNormal_x = gantry->Normal[0];
	double gantryNormal_y = gantry->Normal[1];
	double gantryNormal_z = gantry->Normal[2];
	double bedCenter_x = bed->Center[0];
	double bedCenter_y = bed->Center[1];
	double bedCenter_z = bed->Center[2];
	double bedNormal_x = bed->Normal[0];
	double bedNormal_y = bed->Normal[1];
	double bedNormal_z = bed->Normal[2];

	double l0[3] = { gantryCenter_x, gantryCenter_y, gantryCenter_z };
	double l1[3] = { gantryCenter_x + gantryNormal_x, gantryCenter_y+gantryNormal_y, gantryCenter_z+gantryNormal_z};
	double m0[3] = { bedCenter_x, bedCenter_y, bedCenter_z};
	double m1[3] = { bedCenter_x+bedNormal_x, bedCenter_y+bedNormal_y, bedCenter_z+bedNormal_z};
	double t1;//垂足A的参数坐标
	double t2;//垂足B的参数坐标
	double footA[3], footB[3];
	//
	vtkLine::DistanceBetweenLines(l0,l1, m0, m1, footA, footB, t1, t2);
	reportData.softCenter[0] = 0.5 * (footA[0] + footB[0]);
	reportData.softCenter[1] = 0.5 * (footA[1] + footB[1]);
	reportData.softCenter[2] = 0.5 * (footA[2] + footB[2]);
	memcpy(reportData.footA, footA, sizeof(footA));
	memcpy(reportData.footB, footB, sizeof(footB));

	m_softCenterIsCal = true;
}

//
//处理生成报告的请求， 报告结果在定时事件中发送
//
void CentralModel::handleReport()
{
	double distance = 0;
	double gantryVariance =0, gantryMean = 0, gantryAngle =0;
	double bedVariance=0, bedMean=0, bedAngle = 0;

	if (m_softCenterIsCal && m_LaserISODetected ){  // use light center or not ?
		//calculate distance between softcenter and laserCenter
		distance = sqrt(vtkMath::Distance2BetweenPoints(reportData.softCenter, reportData.laserCenter));
		reportData.distanceLaser2Soft = distance;
	}
	else {
		if (!m_softCenterIsCal)
		{
			emit softISONotCalibrated();
			return ;
		}
		else
		{
			emit laserISONotCalibrated();
			return ;
		}
	}

	bool retg = m_GantryHandler->getRotateStatistical(gantryVariance, gantryMean,gantryAngle);
	bool retb = m_BedHandler->getRotateStatistical(bedVariance, bedMean, bedAngle);
	//bool retc = m_CollimatorHandler->getRotateStatistical(cbctVariance, cbctMean,cbctAngle);

	if (retg && retb ){  // use collomator circle result or not ?
		reportData.gantryVar = gantryVariance;
		reportData.gantryMean = gantryMean;
		reportData.gantryAngle = gantryAngle;
		reportData.bedVar = bedVariance;
		reportData.bedMean = bedMean;
		reportData.bedAngle = bedAngle;
		//reportData.cbctVar = cbctVariance;
		//reportData.cbctMean = cbctMean;
		//reportData.cbctAngle = cbctAngle;

		m_ReadyToReport = true;
		emit editReportSuccessfully();
	}else{
		m_ReadyToReport = false;
		emit editReportFailed();
		return;
	}
}
//
//复位加速器，所有数据置初值
//
void CentralModel::resetAccelerator()
{ 
	setHandlerToNone();
	initData();
}