#ifndef WINDOWSREALTIMECLOCK_H_HEADER_INCLUDED_
#define WINDOWSREALTIMECLOCK_H_HEADER_INCLUDED_

#include "vpsrealtimeclock.h"

/**
* \brief realtimeclock implementation for windows-systems
*
* This class provides a RealTimeClock for windows-systems.
* Internally, it uses the QueryPerformanceCounter and the QueryPerformaceFrequency.
* It polls the current tick-counter, that counts from bootup.
* is supposed to be the most accurate time you can get on a windows-system.
*
* \ingroup Navigation
*/

class WindowsRealTimeClock : public RealTimeClock
{
public:
	vpsClassMacro(WindowsRealTimeClock, RealTimeClock);
	itkNewMacro(Self);

	/**
	* \brief basic contructor
	*/
	WindowsRealTimeClock();

	/**
	* \brief basic destructor
	*/
	virtual ~WindowsRealTimeClock();

	/**
	* \brief returns the current time in milliseconds as a double
	*
	* Uses the QueryPerformanceCounter to acquire an accurate time.
	* (Ticks counted / Frequency) = time in ms
	*/
	virtual double GetCurrentStamp();

	/**
	* \brief returns the QueryPerformanceFrequency
	*/
	virtual LARGE_INTEGER GetFrequency();

protected:

	void SetFrequency();

	/**
	* \brief Frequency needed to calculate time from tick-counter
	*/
	LARGE_INTEGER m_Frequency;
};

#endif /* MITKWINDOWSREALTIMECLOCK_H_HEADER_INCLUDED_ */
