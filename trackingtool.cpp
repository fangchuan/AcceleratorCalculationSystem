#include "trackingtool.h"
#include <itkMutexLockHolder.h>

typedef itk::MutexLockHolder<itk::FastMutexLock> MutexLockHolder;


TrackingTool::TrackingTool()
: itk::Object(), m_ToolName(""), m_ErrorMessage(""), m_IGTTimeStamp(0)
{
  m_MyMutex = itk::FastMutexLock::New();
}


TrackingTool::~TrackingTool()
{
  m_MyMutex->Unlock();
  m_MyMutex = NULL;
}

void TrackingTool::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "ToolName: " << m_ToolName << std::endl;
  os << indent << "ErrorMesage: " << m_ErrorMessage << std::endl;
}

const char* TrackingTool::GetToolName() const
{
  MutexLockHolder lock(*m_MyMutex); // lock and unlock the mutex
  return this->m_ToolName.c_str();
}


const char* TrackingTool::GetErrorMessage() const
{
 MutexLockHolder lock(*m_MyMutex); // lock and unlock the mutex
 return this->m_ErrorMessage.c_str();
}
