#ifndef WREALTIMECLOCK_H_HEADER_INCLUDED_
#define WREALTIMECLOCK_H_HEADER_INCLUDED_


#include <itkObject.h>
#include <itkObjectFactory.h>
#include "vpscommon.h"

/**Documentation
* \brief RealTimeClock is a superclass to WindowsRealTimeClock, LinuxRealTimeClock, etc.
*
* When RealTimeClock::New() is called, a new instance of a RealTimeClock will be
* instantiated. WindowsRealTimeClock on a Windows-system and a LinuxRealTimeClock on a
* Linux-system
*
* \ingroup IGT
*/

class RealTimeClock : public itk::Object
{
public:
	vpsClassMacro(RealTimeClock, itk::Object);

	/**
	*\brief instanciates a new, operating-system dependant, instance of RealTimeClock.
	*/
	static Pointer New(void);

	virtual double GetCurrentStamp() = 0;
};

#endif /* MITKWREALTIMECLOCK_H_HEADER_INCLUDED_ */
