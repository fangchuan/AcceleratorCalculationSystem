#include "trackingdevice.h"
#include "vpsigttimestamp.h"
#include "trackingtool.h"
#include <itkMutexLockHolder.h>
// #include <QtDebug>

typedef itk::MutexLockHolder<itk::FastMutexLock> MutexLockHolder;


TrackingDevice::TrackingDevice() :
m_Data(DeviceDataUnspecified),
m_State(TrackingDevice::Setup),
m_StopTracking(false),
m_RotationMode(TrackingDevice::RotationStandard)

{
	m_StopTrackingMutex = itk::FastMutexLock::New();
	m_StateMutex = itk::FastMutexLock::New();
	m_TrackingFinishedMutex = itk::FastMutexLock::New();
	m_TrackingFinishedMutex->Lock();  // execution rights are owned by the application thread at the beginning
}


TrackingDevice::~TrackingDevice()
{
}


TrackingDevice::TrackingDeviceState TrackingDevice::GetState() const
{
	MutexLockHolder lock(*m_StateMutex);
	return m_State;
}


void TrackingDevice::SetState( TrackingDeviceState state )
{
	itkDebugMacro("setting  m_State to " << state);

	MutexLockHolder lock(*m_StateMutex); // lock and unlock the mutex
	if (m_State == state)
	{
		return;
	}
	m_State = state;
	this->Modified();
}

void TrackingDevice::SetRotationMode(RotationMode r)
{
	//qWarning() << "Rotation mode switching is not implemented for this device. Leaving it at TrackingDevice::RotationStandard";
}

TrackingDeviceType TrackingDevice::GetType() const{
	return m_Data.Line;
}

void TrackingDevice::SetType(TrackingDeviceType deviceType){
	m_Data = GetFirstCompatibleDeviceDataForLine(deviceType);
}

TrackingDeviceData TrackingDevice::GetData() const{
	return m_Data;
}


void TrackingDevice::SetData(TrackingDeviceData data){
	m_Data = data;
}


bool TrackingDevice::StopTracking()
{
	if (this->GetState() == Tracking) // Only if the object is in the correct state
	{
		m_StopTrackingMutex->Lock();  // m_StopTracking is used by two threads, so we have to ensure correct thread handling
		m_StopTracking = true;
		m_StopTrackingMutex->Unlock();
		//we have to wait here that the other thread recognizes the STOP-command and executes it
		m_TrackingFinishedMutex->Lock();
		IGTTimeStamp::GetInstance()->Stop(this); // notify realtime clock
		// StopTracking was called, thus the mode should be changed back
		//   to Ready now that the tracking loop has ended.
		this->SetState(Ready);
	}
	return true;
}


TrackingTool* TrackingDevice::GetToolByName( std::string name ) const
{
	unsigned int toolCount = this->GetToolCount();
	for (unsigned int i = 0; i < toolCount; ++i)
		if (name == this->GetTool(i)->GetToolName())
			return this->GetTool(i);
	return NULL;
}

