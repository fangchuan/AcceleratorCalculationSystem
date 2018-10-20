#include "gantryhandler.h"
#include "horizontalregister.h"
#include "fit3dcircle.h"
#include "circle.h"

#include <QDebug>
#include "vtkPoints.h"
#include "vtkPlaneSource.h"
#include "vtkSmartPointer.h"
#include "vtkMath.h"

GantryHandler::GantryHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_FitCircle(new Fit3DCircle(this)),
	m_BaseAngle(0),
	m_HasBaseAngle(false),
	m_angleC2HContainer(0)
{

}

GantryHandler::~GantryHandler()
{
}

AbstractMonitorHandler *GantryHandler::handle(MarkerPointContainerType &positions)
{
	int size = positions.size();
	if (size != 1) {
		//看到多个标定球则直接返回
		emit pseudoMarkerSize(size);
		return this;
	}

	//只取第一个标定球位置参与圆拟合
	MarkerPointType p = positions.at(0);
	double point[3];
	point[0] = p[0]; point[1] = p[1]; point[2] = p[2];
	double out[3];
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

		double deltY = out[1] - center[1];
		double deltX = out[0] - center[0];
		double angle = -atan2(deltX, deltY) *RAD2DEGREE;

		//随着拟合结果越来越精确，初始位置所在的初始角度应该每次循环都进行一次计算
		double baseDeltY = m_BasePoint[1] - center[1];
		double baseDeltX = m_BasePoint[0] - center[0];
		m_BaseAngle = -atan2(baseDeltX, baseDeltY) *RAD2DEGREE;;

		Circle circle;
		memcpy(circle.Center, center, sizeof(center));
		memcpy(circle.Normal, normal, sizeof(normal));
		circle.Radius = radius;
		circle.Angle = -(angle - m_BaseAngle);
		if (circle.Angle < 0)
			circle.Angle += 360.0;

		//QString str = QString("GantryPlaneNormal : ( %1, %2, %3)").arg(normal[0], 0, 'f', 2).arg(normal[1], 0, 'f', 2).arg(normal[2], 0, 'f', 2);
		//qDebug() << str;

		if (m_Register->getHorizontalPlaneNormal(horizontalPlaneNormal))
		{
			circle.angleBettwenC2H = vtkMath::AngleBetweenVectors(normal, horizontalPlaneNormal) * RAD2DEGREE;

			m_angleC2HContainer = circle.angleBettwenC2H;
		}
		emit circleResult(&circle);
	}

	return this;
}

AbstractMonitorHandler *GantryHandler::handle(Point3D &point)
{
	emit markerPosition(point);
	return this;
}

void GantryHandler::reset()
{
	m_FitCircle->clearPoints();
	m_angleC2HContainer = 0;
	m_BaseAngle = 0;
	m_HasBaseAngle = false;
}

bool GantryHandler::getRotateStatistical(double& variance, double& mean, double& angleMean)
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