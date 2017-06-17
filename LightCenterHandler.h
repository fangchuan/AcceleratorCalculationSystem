#pragma once
#include "AbstractMonitorHandler.h"

class LightCenterHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	LightCenterHandler(QObject *parent = Q_NULLPTR);
	virtual ~LightCenterHandler();

	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();

signals:
	void  registerLightCenter(Point3D& point);

private:
	double m_RegisteredPoint[3];
	bool m_IsRegistered;
};
