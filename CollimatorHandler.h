#pragma once

#include "abstractmonitorhandler.h"

class Circle;
class Fit3DCircle;

class CollimatorHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	CollimatorHandler(QObject *parent = Q_NULLPTR);
	virtual ~CollimatorHandler();

	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();

signals:
	void pseudoMarkerSize(int size);
	void markerPosition(MarkerPointType &point);
	void circleResult(Circle *circle);

private:
	Fit3DCircle *m_FitCircle;
};
