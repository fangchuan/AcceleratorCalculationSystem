#include "vpsigttimestamp.h"
#include <map>

#include "vpsrealtimeclock.h"

IGTTimeStamp::Pointer IGTTimeStamp::s_Instance = NULL;

IGTTimeStamp::IGTTimeStamp() : itk::Object()
, m_Time(-1.0), m_ReferenceTime(0.0)
{
}

IGTTimeStamp::~IGTTimeStamp()
{
}

IGTTimeStamp* IGTTimeStamp::CreateInstance()
{
	return IGTTimeStamp::GetInstance();
}

IGTTimeStamp* IGTTimeStamp::GetInstance()
{
	if (IGTTimeStamp::s_Instance.IsNull())
	{
		IGTTimeStamp::Pointer ts = new IGTTimeStamp;
		s_Instance = ts;
		return s_Instance;
	}
	else
		return s_Instance;
}

void IGTTimeStamp::Start(itk::Object::Pointer device)
{
	if (m_RealTimeClock.IsNull())
	{
		Initialize();
	}
	if ( s_Instance.IsNotNull() )
	{
		if (m_DeviceMap.empty())
		{
			m_ReferenceTime = GetCurrentStamp();
			m_Time = 0.0;
		}
		m_DeviceMap.insert( std::pair<itk::Object::Pointer, double>(device, this->GetElapsed()) );
	}
	else
	{
		itkGenericOutputMacro("Trying to use TimeStamp::Start() "
			<< "without an available singleton instance. Either no instance has "
			<< "been created (use TimeStamp::CreateInstance) or it has already "
			<< "been destroyed.");
	}
}

void IGTTimeStamp::Stop(itk::Object::Pointer device)
{
	if ( s_Instance.IsNotNull() )
	{
		m_MapIterator =  m_DeviceMap.find(device);
		if ( m_MapIterator != m_DeviceMap.end() )
		{
			m_DeviceMap.erase( m_MapIterator );
		}

		if (m_DeviceMap.empty())
		{
			m_ReferenceTime = NULL;
			m_Time = -1;
		}
	}
	else
	{
		itkGenericOutputMacro("Trying to use TimeStamp::Stop() "
			<< "without an available singleton instance. Either no instance has "
			<< "been created (use TimeStamp::CreateInstance) or it has already "
			<< "been destroyed.");
	}
}


double IGTTimeStamp::GetElapsed()
{
	if (m_Time > -1)
	{
		m_Time = GetCurrentStamp();
		m_Time = m_Time - m_ReferenceTime;
	}
	return (double) m_Time;
}


double IGTTimeStamp::GetElapsed(itk::Object::Pointer device)
{
	double offset = this->GetOffset( device );
	if ( offset > -1 )
	{
		double time = this->GetElapsed();
		return (double) time - this->GetOffset(device);
	}
	else
	{
		return (double) -1;
	}
}

double IGTTimeStamp::GetCurrentStamp()
{
	if (m_RealTimeClock.IsNotNull())
	{
		return m_RealTimeClock->GetCurrentStamp();
	}
	else return 0.0;
}

void IGTTimeStamp::SetRealTimeClock(RealTimeClock::Pointer Clock)
{
	m_RealTimeClock = Clock;
}

double IGTTimeStamp::GetOffset(itk::Object::Pointer Device)
{
	m_MapIterator =  m_DeviceMap.find(Device);
	if ( m_MapIterator != m_DeviceMap.end() )
	{
		return m_MapIterator->second;
	}
	else
	{
		return -1.0;
	}
}

void IGTTimeStamp::Initialize()
{
	if ( m_RealTimeClock.IsNull() )
		m_RealTimeClock = RealTimeClock::New();
}
