#include "CentralModel.h"
#include "OpsTrackingDevice.h"
#include "AbstractMonitorHandler.h"
#include "HorizontalRegisterHandler.h"
#include "GantryHandler.h"
#include "CollimatorHandler.h"
#include "BedHandler.h"
#include "ISOCenterHandler.h"

CentralModel::CentralModel(QObject *parent)
	: QObject(parent),
	m_Handler(nullptr),
	m_HorizontalRegisterHandler(new HorizontalRegisterHandler(this)),
	m_GantryHandler(new GantryHandler(this)),
	m_CollimatorHandler(new CollimatorHandler(this)),
	m_BedHandler(new BedHandler(this)),
	m_ISOCenterHanlder(new ISOCenterHandler(this))
{
	buildConnections();
}

CentralModel::~CentralModel()
{
}

void CentralModel::buildConnections()
{
	connect(m_HorizontalRegisterHandler, &HorizontalRegisterHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_HorizontalRegisterHandler, &HorizontalRegisterHandler::horizontalRegisterRecorded, this, &CentralModel::horizontalRegisterRecorded);
	connect(m_GantryHandler, &GantryHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_GantryHandler, &GantryHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_GantryHandler, &GantryHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_CollimatorHandler, &CollimatorHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_CollimatorHandler, &CollimatorHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_CollimatorHandler, &CollimatorHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_BedHandler, &BedHandler::markerSize, this, &CentralModel::markerSize);
	connect(m_BedHandler, &BedHandler::markerPosition, this, &CentralModel::markerPosition);
	connect(m_BedHandler, &BedHandler::circleResult, this, &CentralModel::circleResult);
	connect(m_BedHandler, &BedHandler::translateResult, this, &CentralModel::translateResult);
	connect(m_ISOCenterHanlder, &ISOCenterHandler::registerPosition, this, &CentralModel::registerPosition);
}

void CentralModel::setHandlerToNone()
{
	m_Handler = nullptr;
}

void CentralModel::setHandlerToHorizontalRegister()
{
	m_Handler = m_HorizontalRegisterHandler;
	m_Handler->reset();
}

void CentralModel::setHandlerToGantry()
{
	m_Handler = m_GantryHandler;
	m_Handler->reset();
}

void CentralModel::setHandlerToCollimator()
{
	m_Handler = m_CollimatorHandler;
	m_Handler->reset();
}

void CentralModel::setHandlerToBed(int mode)
{
	m_Handler = m_BedHandler;
	m_BedHandler->setMode(mode);
	m_Handler->reset();
}

void CentralModel::setHandlerToISOCenter()
{
	m_Handler = m_ISOCenterHanlder;
	m_Handler->reset();
}
//
//当没有接收到拟合/计算信号时，只显示球的数量
//
void CentralModel::handle(MarkerPointContainerType &positions)
{
	if (m_Handler) {
		m_Handler = m_Handler->handle(positions);
	}
	else {
		emit markerSize(positions.size());
	}
}
//
//
//
void CentralModel::handle(Point3D &point)
{
	if (m_Handler) {
		m_Handler = m_Handler->handle(point);
	}
}