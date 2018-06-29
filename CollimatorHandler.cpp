#include "collimatorhandler.h"
#include "horizontalregister.h"
#include "fit3dcircle.h"
#include "circle.h"

#include "vtkPoints.h"
#include "vtkMath.h"

CollimatorHandler::CollimatorHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_FitCircle(new Fit3DCircle(this)),
	m_BaseAngle(0),
	m_HasBaseAngle(false)
{
}

CollimatorHandler::~CollimatorHandler()
{
}

AbstractMonitorHandler *CollimatorHandler::handle(MarkerPointContainerType &positions)
{
	int size = positions.size();
	if (size != 1) {
		emit pseudoMarkerSize(size);
		return this;
	}

	MarkerPointType &p = positions.at(0);
	double point[3];
	point[0] = p[0]; point[1] = p[1]; point[2] = p[2];
	double out[3];
	m_Register->transform(point, out);

	MarkerPointType marker(out);
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
		m_BaseAngle = atan2(baseDeltX, baseDeltZ) *RAD2DEGREE;;

		Circle circle;
		memcpy(circle.Center, center, sizeof(center));
		memcpy(circle.Normal, normal, sizeof(normal));
		circle.Radius = radius;
		circle.Angle = angle - m_BaseAngle;

		if (m_Register->getHorizontalPlaneNormal(horizontalPlaneNormal))
		{
			circle.angleBettwenC2H = vtkMath::AngleBetweenVectors(normal, horizontalPlaneNormal)* RAD2DEGREE;
		}
		emit circleResult(&circle);
	}

	return this;
}

AbstractMonitorHandler *CollimatorHandler::handle(Point3D &point)
{
	emit markerPosition(point);
	return this;
}

void CollimatorHandler::reset()
{
	m_FitCircle->clearPoints();
	m_HasBaseAngle == false;
}