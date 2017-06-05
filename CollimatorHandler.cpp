#include "CollimatorHandler.h"
#include "HorizontalRegister.h"
#include "Fit3DCircle.h"
#include "Circle.h"

#include "vtkPoints.h"

CollimatorHandler::CollimatorHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_FitCircle(new Fit3DCircle(this))
{
}

CollimatorHandler::~CollimatorHandler()
{
}

AbstractMonitorHandler *CollimatorHandler::handle(MarkerPointContainerType &positions)
{
	int size = positions.size();
	if (size != 1) {
		emit markerSize(size);
		return this;
	}
	//else {
	//	emit markerPosition(positions.at(0));
	//}
	MarkerPointType &p = positions.at(0);
	double point[3];
	point[0] = p[0]; point[1] = p[1]; point[2] = p[2];
	double out[3];
	m_Register->transform(point, out);

	MarkerPointType marker(out);
	emit markerPosition(marker);

	m_FitCircle->addPoint(marker);

	double center[3], normal[3], radius;
	if (m_FitCircle->getCircle(center, normal, radius)) {

		double rad = (out[2] - center[2]) / radius;
		rad = rad > 1 ? 1 : rad;
		rad = rad < -1 ? -1 : rad;
		double angle = acos(rad) * RAD2DEGREE;
		angle *= out[0] < center[0] ? 1 : -1;
		
		Circle circle;
		memcpy(circle.Center, center, sizeof(center));
		memcpy(circle.Normal, normal, sizeof(normal));
		circle.Radius = radius;
		circle.Angle = angle;
		circle.IsParallelOrPerpendicular = normal[0] < 1e-5 && normal[2] < 1e-5;
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
}