#include "CentralModel.h"
#include "OpsTrackingDevice.h"
#include "AbstractMonitorHandler.h"
#include "HorizontalRegisterHandler.h"
#include "GantryHandler.h"
#include "CollimatorHandler.h"
#include "BedHandler.h"
#include "ISOCenterHandler.h"
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
	m_ISOCenterHanlder(new ISOCenterHandler(this))
{
	initData();
	buildConnections();
}

void CentralModel::initData()
{
	m_BedFitted = false;
	m_GantryFitted = false;
	m_softCenterIsCal = false;
	m_LaserISODetected = false;
	gantryCircle = new Circle;
	bedCircle = new Circle;
	reportData.softCenter[0] = 0; reportData.softCenter[1] = 0; reportData.softCenter[2] = 0;
	reportData.laserCenter[0] = 0; reportData.laserCenter[1] = 0; reportData.laserCenter[2] = 0;
	reportData.footA[0] = 0; reportData.footA[1] = 0; reportData.footA[2] = 0;
	reportData.footB[0] = 0; reportData.footB[1] = 0; reportData.footB[2] = 0;
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
	connect(m_GantryHandler, &GantryHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_GantryHandler, &GantryHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_GantryHandler, &GantryHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_CollimatorHandler, &CollimatorHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_CollimatorHandler, &CollimatorHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_CollimatorHandler, &CollimatorHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_BedHandler, &BedHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_BedHandler, &BedHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_BedHandler, &BedHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_BedHandler, &BedHandler::translateResult, this, &CentralModel::translateResult);
	connect(m_ISOCenterHanlder, &ISOCenterHandler::registerPosition, this, &CentralModel::registerPosition);

	connect(m_ISOCenterHanlder, &ISOCenterHandler::registerPosition, this, &CentralModel::laserISOCenter);
	connect(m_BedHandler, &BedHandler::circleResult, this, &CentralModel::softISOCenter);
	connect(m_GantryHandler, &GantryHandler::circleResult, this, &CentralModel::softISOCenter);
}

void CentralModel::setHandlerToNone()
{
	m_Handler = nullptr;
}

void CentralModel::setHandlerToHorizontalRegister()
{
	m_Handler = m_HorizontalRegisterHandler;
	m_Handler->reset();
}

void CentralModel::setHandlerToGantry()
{
	m_Handler = m_GantryHandler;
	m_Handler->reset();
}

void CentralModel::setHandlerToCollimator()
{
	m_Handler = m_CollimatorHandler;
	m_Handler->reset();
}

void CentralModel::setHandlerToBed(int mode)
{
	m_Handler = m_BedHandler;
	m_BedHandler->setMode(mode);
	m_Handler->reset();
}

void CentralModel::setHandlerToISOCenter()
{
	m_Handler = m_ISOCenterHanlder;
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
						return 0;
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
//
//记录激光灯的等中心
//
void CentralModel::handle(Point3D &point)
{
	if (m_Handler) {
		m_Handler = m_Handler->handle(point);
	}
}

void CentralModel::laserISOCenter(Point3D& point)
{
	reportData.laserCenter[0] = point[0];
	reportData.laserCenter[1] = point[1];
	reportData.laserCenter[2] = point[2];

	m_LaserISODetected = true;
}

void CentralModel::softISOCenter(Circle* circle)
{
	QString sender = this->sender()->objectName();
	qDebug() << "sender name:" << sender;

	if (sender == "m_GantryHandler"){
		m_GantryFitted = true;
		memcpy(gantryCircle, circle, sizeof(Circle));
	}
	if (sender == "m_BedHandler"){
		m_BedFitted = true;
		memcpy(bedCircle, circle, sizeof(circle));
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

void CentralModel::handleReport()
{
	double distance = 0;
	double gantryVariance =0, gantryMean =0;
	double bedVariance=0, bedMean=0;

	if (m_softCenterIsCal && m_LaserISODetected){
		//calculate distance between softcenter and laserCenter
		distance = sqrt(vtkMath::Distance2BetweenPoints(reportData.softCenter, reportData.laserCenter));
		reportData.distanceLaser2Soft = distance;
	}

	bool retg = m_GantryHandler->getRotateStatistical(gantryVariance, gantryMean);
	bool retb = m_BedHandler->getRotateStatistical(bedVariance, bedMean);

	if (retg && retb){
		reportData.gantryVar = gantryVariance;
		reportData.gantryMean = gantryMean;
		reportData.bedVar = bedVariance;
		reportData.bedMean = bedMean;

		emit sendReport(reportData);
	}else{
		return;
	}
}