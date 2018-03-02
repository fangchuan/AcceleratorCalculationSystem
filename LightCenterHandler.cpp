#include "lightcenterhandler.h"
#include "horizontalregister.h"

LightCenterHandler::LightCenterHandler(QObject *parent) :AbstractMonitorHandler(parent),
m_IsRegistered(false)
{
}

LightCenterHandler::~LightCenterHandler()
{
}

AbstractMonitorHandler*  LightCenterHandler::handle(MarkerPointContainerType &positions)
{
	return this;
}
//
//point��NDI�µ����ĵ�����
//ת��������������ϵ�º�m_RegisteredPoint�����յ����ĵ�
AbstractMonitorHandler* LightCenterHandler::handle(Point3D &point)
{
	m_RegisteredPoint[0] = point[0];
	m_RegisteredPoint[1] = point[1];
	m_RegisteredPoint[2] = point[2];

	double out[3];
	m_Register->transform(m_RegisteredPoint, out);

	memcpy(m_RegisteredPoint, out, sizeof(m_RegisteredPoint));
	m_IsRegistered = true;

	emit registerLightCenter(MarkerPointType(out));

	return nullptr;
}

void LightCenterHandler::reset()
{
	m_IsRegistered = false;
}

