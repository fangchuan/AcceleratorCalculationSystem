#include "bedhandler.h"
#include "circle.h"
#include "fit3dcircle.h"
#include "horizontalregister.h"
#include "vtkPoints.h"
#include <qdebug.h>
#include <qlogging.h>

#include "vtkMath.h"

BedHandler::BedHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_FitCircle(new Fit3DCircle(this)),
	m_Mode(0),
	m_HasBasePoint(false),
	m_BaseAngle(0),
	m_HasBaseAngle(false),
	m_angleC2HContainer(0)
{
}

BedHandler::~BedHandler()
{
}

void BedHandler::setMode(int mode)
{
	m_Mode = mode;
}

AbstractMonitorHandler *BedHandler::handle(MarkerPointContainerType &positions)
{
	int size = positions.size();
	if (size != 1) {
		emit pseudoMarkerSize(size);
		return this;
	}
	//else {
	//	emit markerPosition(positions.at(0));
	//}
	switch (m_Mode) {
	case 0:
		handleRotation(positions);
		break;
	case 1:
		handleTranslation(positions);
		break;
	case  2:
		handleRotation(positions);
		break;
	case 3:
		handleTranslation(positions);
		break;
	default:
		break;
	}

	return this;
}

AbstractMonitorHandler *BedHandler::handle(Point3D &point)
{
	emit markerPosition(point);
	return this;
}

void BedHandler::reset()
{
	m_FitCircle->clearPoints();
	m_angleC2HContainer = 0;
	m_HasBasePoint = false;
	m_HasBaseAngle = false;
}
//测量旋转:圆、角度(绝对)、半径、法向量
//
//
void BedHandler::handleRotation(MarkerPointContainerType &positions)
{
	MarkerPointType &p = positions.at(0);
	double point[3];
	
	point[0] = p[0]; point[1] = p[1]; point[2] = p[2];
	double out[3];
	//transform to Accelerator Coordinate(==Scene Coordinate)
	m_Register->transform(point, out);
	MarkerPointType  marker(out);
	emit markerPosition(marker);

	if (!m_HasBaseAngle)
	{
		memcpy(m_BasePoint, out, sizeof(out));
		m_HasBaseAngle = true;
	}
	m_FitCircle->addPoint(marker);
	double center[3], normal[3], horizontalPlaneNormal[3], radius;
	if (m_FitCircle->getCircle(center, normal, radius)) {

		double deltZ = out[2] - center[2];
		double deltX = out[0] - center[0];
		double angle = atan2(deltX, deltZ) *RAD2DEGREE;
		
		//随着拟合结果越来越精确，初始位置所在的初始角度应该每次循环都进行一次计算
		double baseDeltZ = m_BasePoint[2] - center[2];
		double baseDeltX = m_BasePoint[0] - center[0];
		m_BaseAngle = atan2(baseDeltX, baseDeltZ) *RAD2DEGREE;

		Circle circle;
		memcpy(circle.Center, center, sizeof(center));
		memcpy(circle.Normal, normal, sizeof(normal));
		circle.Radius = radius;
		circle.Angle = -(angle - m_BaseAngle);
		if (circle.Angle < 0)
			circle.Angle += 360.0;

		//QString str = QString("BedPlaneNormal : ( %1, %2, %3)").arg(normal[0], 0, 'f', 2).arg(normal[1], 0, 'f', 2).arg(normal[2], 0, 'f', 2);
		//qDebug() << str;

		if (m_Register->getHorizontalPlaneNormal(horizontalPlaneNormal))
		{
			circle.angleBettwenC2H = vtkMath::AngleBetweenVectors(normal, horizontalPlaneNormal)*RAD2DEGREE;
			m_angleC2HContainer = circle.angleBettwenC2H;
		}

		emit circleResult(&circle);
	}
}

//测量位移
//以初始的静止小球为零点，每次返回绝对位移
void BedHandler::handleTranslation(MarkerPointContainerType &positions)
{
	MarkerPointType &p = positions.at(0);
	double point[3];
	point[0] = p[0]; point[1] = p[1]; point[2] = p[2];

	double out[3];
	m_Register->transform(point, out);

	emit markerPosition(MarkerPointType(out));

	if (!m_HasBasePoint) {
		memcpy(m_BasePoint, out, sizeof(m_BasePoint));
		m_HasBasePoint = true;
	}

	double bias[3] = { out[0] - m_BasePoint[0], out[1] - m_BasePoint[1], out[2] - m_BasePoint[2] };
	emit translateResult(bias);
}

bool BedHandler::getRotateStatistical(double& variance, double& mean, double& angleMean)
{
	if (m_FitCircle->calRotateError(variance, mean))
	{
		angleMean = m_angleC2HContainer;
		return true;
		
	}
	else {
	
		angleMean = 0;
		return false;
	}
}
