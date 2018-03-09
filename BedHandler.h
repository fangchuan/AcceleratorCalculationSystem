#pragma once

#include "abstractmonitorhandler.h"

class Circle;
class Fit3DCircle;

class BedHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	BedHandler(QObject *parent = Q_NULLPTR);
	virtual ~BedHandler();
	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);
	virtual void reset();
	void setMode(int mode);
	bool getRotateStatistical(double& variance, double& mean);
signals:
	void pseudoMarkerSize(int size);
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
	std::vector<double> m_angleC2HContainer;
};
