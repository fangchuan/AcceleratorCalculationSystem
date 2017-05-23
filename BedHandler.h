#pragma once

#include "AbstractMonitorHandler.h"

class Circle;
class Fit3DCircle;

class BedHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	BedHandler(QObject *parent = Q_NULLPTR);
	virtual ~BedHandler();

	void setMode(int mode);

	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();

signals:
	void markerSize(int size);
	void markerPosition(MarkerPointType &point);
	void circleResult(Circle *circle);
	void translateResult(double bias[3]);

private:
	void handleRotation(MarkerPointContainerType &positions);
	void handleTranslation(MarkerPointContainerType &positions);

private:
	Fit3DCircle *m_FitCircle;
	int m_Mode;
	double m_BasePoint[3];
	bool m_HasBasePoint;
};
