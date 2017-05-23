#ifndef TRACKINGTOOL_H_HEADER_INCLUDED_
#define TRACKINGTOOL_H_HEADER_INCLUDED_

#include <itkObject.h>
#include <vpscommon.h>
#include <vpsvector.h>
#include <itkFastMutexLock.h>

/**Documentation
* \brief Interface for all Tracking Tools
*
* Abstract class that defines the methods that are common for all tracking tools.
*
* \ingroup IGT
*/
class TrackingTool : public itk::Object
{
public:
	vpsClassMacro(TrackingTool, itk::Object);

	virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

	virtual void SetToolTip(Point3D toolTipPosition, Quaternion orientation, ScalarType eps=0.0) = 0; ///< defines a tool tip for this tool in tool coordinates. GetPosition() and GetOrientation() return the data of the tool tip if it is defined. By default no tooltip is defined.
	virtual void GetPosition(Point3D& position) const = 0;          ///< returns the current position of the tool as an array of three floats (in the tracking device coordinate system)
	virtual void GetOrientation(Quaternion& orientation) const = 0;  ///< returns the current orientation of the tool as a quaternion in a mitk::Point4D (in the tracking device coordinate system)
	virtual bool Enable() = 0;                       ///< enables the tool, so that it will be tracked
	virtual bool Disable() = 0;                      ///< disables the tool, so that it will not be tracked anymore
	virtual bool IsEnabled() const = 0;              ///< returns whether the tool is enabled or disabled
	virtual bool IsDataValid() const = 0;            ///< returns true if the current position data is valid (no error during tracking, tracking error below threshold, ...)
	virtual float GetTrackingError() const = 0;      ///< returns one value that corresponds to the overall tracking error.
	virtual const char* GetToolName() const;         ///< every tool has a name that can be used to identify it.
	virtual const char* GetErrorMessage() const;     ///< if the data is not valid, ErrorMessage should contain a string explaining why it is invalid (the Set-method should be implemented in subclasses, it should not be accessible by the user)
	itkSetMacro(IGTTimeStamp, double);               ///< Sets the IGT timestamp of the tracking tool object
	itkGetConstMacro(IGTTimeStamp, double);          ///< Gets the IGT timestamp of the tracking tool object. Returns 0 if the timestamp was not set.

protected:
	TrackingTool();
	virtual ~TrackingTool();
	std::string m_ToolName;                          ///< every tool has a name that can be used to identify it.
	std::string m_ErrorMessage;                      ///< if a tool is invalid, this member should contain a human readable explanation of why it is invalid
	double m_IGTTimeStamp;                           ///< contains the time at which the tracking data was recorded
	itk::FastMutexLock::Pointer m_MyMutex;           ///< mutex to control concurrent access to the tool
};

#endif /* MITKTRACKINGTOOL_H_HEADER_INCLUDED_ */
