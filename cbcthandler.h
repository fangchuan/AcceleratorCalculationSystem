#pragma once

#include "abstractmonitorhandler.h"

class Fit3DCircle;
class Circle;

class CbctHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	CbctHandler(QObject *parent = Q_NULLPTR);
	virtual ~CbctHandler();

	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();
	bool getRotateStatistical(double& variance, double& mean);
signals:
	void pseudoMarkerSize(int size);
	void markerPosition(MarkerPointType &point);
	void circleResult(Circle *circle);

private:
	Fit3DCircle *m_FitCircle;
};