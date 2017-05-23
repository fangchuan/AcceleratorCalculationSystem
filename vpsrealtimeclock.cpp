#include "vpsrealtimeclock.h"


// check if VPS runs on a Windows-System
#ifdef _WIN32
  #include "vpswindowsrealtimeclock.h"
#else // should be Linux or Mac OSX
  #include "vpsLinuxRealTimeClock.h"
#endif

RealTimeClock::Pointer RealTimeClock::New()
{
  RealTimeClock::Pointer smartPtr;

#ifdef _WIN32
  smartPtr = WindowsRealTimeClock::New();
#else
  smartPtr = LinuxRealTimeClock::New();
#endif

  return smartPtr;
}
