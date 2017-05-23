#include "AbstractMonitorHandler.h"
#include "HorizontalRegister.h"

HorizontalRegister *AbstractMonitorHandler::m_Register = HorizontalRegister::getInstance();

AbstractMonitorHandler::AbstractMonitorHandler(QObject *parent)
	: QObject(parent)
{
}

AbstractMonitorHandler::~AbstractMonitorHandler()
{
}
