#include "opstrackingdevice.h"
#include "vpsigthardwareexception.h"

OpsTrackingDevice *OpsTrackingDevice::m_Instance = NULL;

OpsTrackingDevice::OpsTrackingDevice()
{
	m_Instance = this;

	initTrackingDevice();
}

OpsTrackingDevice::~OpsTrackingDevice()
{
	if (m_TrackingDevice)
	{
		m_TrackingDevice->Delete();
	}
}

OpsTrackingDevice *OpsTrackingDevice::getInstance()
{
	return m_Instance == NULL ? new OpsTrackingDevice : m_Instance;
}

void OpsTrackingDevice::initTrackingDevice()
{
	m_TrackingDevice = NDITrackingDevice::New();
	m_TrackingDevice->SetPortNumber(static_cast<SerialCommunication::PortNumber>(4));
	m_TrackingDevice->SetDeviceName(std::string("COM4"));
	m_TrackingDevice->SetBaudRate(SerialCommunication::BaudRate115200);
	m_TrackingDevice->SetType(NDIPolaris);
	m_TrackingDevice->SetIlluminationActivationRate(Hz20);

	m_TrackingTool = dynamic_cast<NDIPassiveTool*>(m_TrackingDevice->AddTool("UniqueTool", "./Resources/polaris/8700248.rom"));
}

TrackingDeviceType OpsTrackingDevice::findTracker()
{
	TrackingDeviceType returnValue = TrackingSystemInvalid;
	try
	{
		returnValue = m_TrackingDevice->TestConnection();
	}
	catch (IGTException e)
	{}
	m_TrackingDevice->CloseConnection();

	return returnValue;
}

bool OpsTrackingDevice::openConnection()
{
	try
	{
		m_TrackingDevice->OpenConnection();
	}
	catch (IGTException& e)
	{
		throw std::runtime_error(std::string("TrackingDeviceSource: Could not open connection to tracking device. Error: ") + e.GetDescription());
	}
	return true;
}

bool OpsTrackingDevice::closeConnection()
{
	return m_TrackingDevice->CloseConnection();
}

bool OpsTrackingDevice::setOperationMode(OperationMode mode)
{
	if (m_TrackingDevice->GetOperationMode() != mode)
	{
		bool result = m_TrackingDevice->SetOperationMode(mode);
		if (result)
		{
			emit OperationModeChanged(mode);
		}
		return result;
	}

	return true;
}

OperationMode OpsTrackingDevice::getOperationMode()
{
	return m_TrackingDevice->GetOperationMode();
}

bool OpsTrackingDevice::startTracking()
{
	return m_TrackingDevice->StartTracking();
}

bool OpsTrackingDevice::stopTracking()
{
	return m_TrackingDevice->StopTracking();
}

bool OpsTrackingDevice::startTrackingInMode(OperationMode mode)
{
	if (m_TrackingDevice->GetState() < TrackingDevice::Ready)
	{
		openConnection();
	}
	if (m_TrackingDevice->GetState() == TrackingDevice::Ready)
	{
		swithModeAndTracking(mode);
	}
	else if (m_TrackingDevice->GetState() == TrackingDevice::Tracking)
	{
		if (m_TrackingDevice->GetOperationMode() != mode)
		{
			m_TrackingDevice->StopTracking();
			swithModeAndTracking(mode);
		}
	}

	return true;
}

void OpsTrackingDevice::swithModeAndTracking(OperationMode mode)
{
	setOperationMode(mode);
	//等待硬件完成切换
	Sleep(200);
	startTracking();
}

int OpsTrackingDevice::getFrequency() const
{
	return m_TrackingDevice->GetIlluminationActivationRate();
}

TrackingDevice::TrackingDeviceState OpsTrackingDevice::getState() const
{
	return m_TrackingDevice->GetState();
}

void OpsTrackingDevice::getToolPosition(Point3D& position) const
{
	if (m_TrackingDevice->GetState() != TrackingDevice::Tracking || (m_TrackingDevice->GetOperationMode() != ToolTracking6D && m_TrackingDevice->GetOperationMode() != ToolTracking5D))
	{
		return ;
	}

	m_TrackingTool->GetPosition(position);
}

void OpsTrackingDevice::getToolOrientation(Quaternion& orientation) const
{
	if (m_TrackingDevice->GetState() != TrackingDevice::Tracking || (m_TrackingDevice->GetOperationMode() != ToolTracking6D && m_TrackingDevice->GetOperationMode() != ToolTracking5D))
	{
		return ;
	}

	m_TrackingTool->GetOrientation(orientation);
}

void OpsTrackingDevice::getToolPositionAndOrientation(Point3D& position, Quaternion& orientation) const
{
	if (m_TrackingDevice->GetState() != TrackingDevice::Tracking || (m_TrackingDevice->GetOperationMode() != ToolTracking6D && m_TrackingDevice->GetOperationMode() != ToolTracking5D))
	{
		return ;
	}

	m_TrackingTool->GetPosition(position);
	m_TrackingTool->GetOrientation(orientation);
}

bool OpsTrackingDevice::getMarkerPositions(MarkerPointContainerType* markerpositions)
{
	return m_TrackingDevice->GetMarkerPositions(markerpositions);
}