#pragma once

#include "abstractmonitorhandler.h"

class Fit3DCircle;
class Circle;

class GantryHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	GantryHandler(QObject *parent = Q_NULLPTR);
	virtual ~GantryHandler();
	
	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();
	bool getRotateStatistical(double& variance, double& mean, double& angleMean);
signals:
	void pseudoMarkerSize(int size);
	void markerPosition(MarkerPointType &point);
	void circleResult(Circle *circle);

private:
	Fit3DCircle *m_FitCircle;
	double m_BaseAngle;
	bool m_HasBaseAngle;
	double m_BasePoint[3];
	double  m_angleC2HContainer;
};
