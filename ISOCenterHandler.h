#pragma once

#include "abstractmonitorhandler.h"

class ISOCenterHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	ISOCenterHandler(QObject *parent = Q_NULLPTR);
	virtual ~ISOCenterHandler();

	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();

signals:
	void  registerLaserISO(Point3D& point);

private:
	double m_RegisteredPoint[3];
	bool m_IsRegistered;
};
