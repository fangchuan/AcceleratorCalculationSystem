#include "GantryHandler.h"
#include "HorizontalRegister.h"
#include "Fit3DCircle.h"
#include "Circle.h"

#include "vtkPoints.h"

GantryHandler::GantryHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_FitCircle(new Fit3DCircle(this))
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
		emit markerSize(size);
		return this;
	}
	else {
		emit markerPosition(positions.at(0));
	}

	//只取第一个标定球位置参与圆拟合
	MarkerPointType &p = positions.at(0);
	double point[3];
	point[0] = p[0]; point[1] = p[1]; point[2] = p[2];
	double out[3];
	m_Register->transform(point, out);
	m_FitCircle->addPoint(MarkerPointType(out));
	double center[3], normal[3], radius;
	if (m_FitCircle->getCircle(center, normal, radius)) {
		// normalize
		double length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
		double angle = acos((out[1] - center[1]) / radius) * 180 / 3.1416;
		angle *= out[0] < center[0] ? 1 : -1;
		Circle circle;
		memcpy(circle.Center, center, sizeof(center));
		memcpy(circle.Normal, normal, sizeof(normal));
		circle.Radius = radius;
		circle.Angle = angle;
		circle.IsParallelOrPerpendicular = normal[0] < 1e-5 && normal[1] < 1e-5;
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
}