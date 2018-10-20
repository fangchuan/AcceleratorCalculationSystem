#include "isocenterhandler.h"
#include "horizontalregister.h"
#include <QTimer>

ISOCenterHandler::ISOCenterHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_IsRegistered(false)
{
}

ISOCenterHandler::~ISOCenterHandler()
{
}

AbstractMonitorHandler *ISOCenterHandler::handle(MarkerPointContainerType &positions) 
{
	return this;
}
//
//point是NDI下的中心点坐标
//转换到加速器坐标系下后m_RegisteredPoint是最终等中心点
AbstractMonitorHandler *ISOCenterHandler::handle(Point3D &point)
{
	m_RegisteredPoint[0] = point[0];
	m_RegisteredPoint[1] = point[1];
	m_RegisteredPoint[2] = point[2];

	double out[3];
	m_Register->transform(m_RegisteredPoint, out);

	memcpy(m_RegisteredPoint, out, sizeof(m_RegisteredPoint));
	

	if (!m_IsRegistered)
	{
		QTimer::singleShot(2000, this, [&](){
			m_IsRegistered = true; });
	}
	else
	{
		emit registerLaserISO(MarkerPointType(out));
		emit registerLaserISOSuccess();
		return NULL;
	}

	emit registerLaserISO(MarkerPointType(out));
	return this;
}

void ISOCenterHandler::reset()
{
	m_IsRegistered = false;
}
