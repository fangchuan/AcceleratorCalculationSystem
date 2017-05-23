#pragma once

#include <QObject>

#include "trackingtypes.h"
#include "vpsvector.h"

class HorizontalRegister;

class AbstractMonitorHandler : public QObject
{
	Q_OBJECT

public:
	AbstractMonitorHandler(QObject *parent);
	virtual ~AbstractMonitorHandler();

	virtual AbstractMonitorHandler *handle(MarkerPointContainerType &positions) = 0;
	virtual AbstractMonitorHandler *handle(Point3D &point) = 0;
	virtual void reset() = 0;

protected:
	static HorizontalRegister *m_Register;
};
