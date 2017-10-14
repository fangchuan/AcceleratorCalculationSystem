#ifndef TRACKINGDEVICE_H_HEADER_INCLUDED_C1C2FCD2
#define TRACKINGDEVICE_H_HEADER_INCLUDED_C1C2FCD2

#include "itkObject.h"
#include "vpscommon.h"
#include "trackingtypes.h"
#include "itkFastMutexLock.h"

class TrackingTool; // interface for a tool that can be tracked by the TrackingDevice

/**Documentation
* \brief Interface for all Tracking Devices
*
* Defines the methods that are common for all tracking devices.
*
*  \ingroup IGT
*/
class TrackingDevice : public itk::Object
{
public:
	vpsClassMacro(TrackingDevice, itk::Object);

	/** Defines the rotation modes of this tracking device which results in different representations
	*  of quaternions.
	*
	*  - Standard: normal representation, raw data from the device is not changed (DEFAULT)
	*
	*  - Transposed: the rotation is stored transposed, which is (by mistake!) expected by some older MITK classes due
	*                to an ambigious method naming in VNL.
	*
	*  CAUTION: The rotation mode can only be changed for backward compatibility of old WRONG code.
	*           PLEASE DO NOT CHANGE THE ROTATION MODE UNLESS YOU ARE KNOWING EXACTLY WHAT YOU ARE DOING!
	*
	*  use SetRotationMode to change the mode.
	*/
	enum RotationMode {RotationStandard, RotationTransposed};

	enum TrackingDeviceState {Setup, Ready, Tracking};   ///< Type for state variable. The trackingdevice is always in one of these states
	/**
	* \brief Opens a connection to the device
	*
	* This may only be called if there is currently no connection to the device.
	* If OpenConnection() is successful, the object will change from Setup state to Ready state
	*/
	virtual bool OpenConnection() = 0;

	/**
	* \brief Closes the connection to the device
	*
	* This may only be called if there is currently a connection to the device, but tracking is
	* not running (e.g. object is in Ready state)
	*/
	virtual bool CloseConnection() = 0; ///< Closes the connection with the device

	/**
	* \brief start retrieving tracking data from the device.
	*
	* This may only be called after the connection to the device has been established
	* with a call to OpenConnection() (E.g. object is in Ready mode). This will change the
	* object state from Ready to Tracking
	*/
	virtual bool StartTracking() = 0;

	/**
	* \brief stop retrieving tracking data from the device.
	* stop retrieving tracking data from the device.
	* This may only be called after StartTracking was called
	* (e.g. the object is in Tracking mode).
	* This will change the object state from Tracking to Ready.
	*/
	virtual bool StopTracking();

	/**
	* \brief Return tool with index toolNumber
	*
	* tools are numbered from 0 to GetToolCount() - 1.
	*/
	virtual TrackingTool* GetTool(unsigned int toolNumber) const = 0;

	/**
	* \brief Returns the tool with the given tool name
	*
	* Note: subclasses can and should implement optimized versions of this method
	* \return the given tool or NULL if no tool with that name exists
	*/
	virtual TrackingTool* GetToolByName(std::string name) const;

	/**
	* \brief Returns number of tracking tools
	*/
	virtual unsigned int GetToolCount() const = 0;

	/** Sets the rotation mode of this class. See documentation of enum RotationMode for details
	*  on the different modes. This method has to be implemented in a deriving class to become
	*  functional / if different rotation modes should be supported.
	*  CAUTION: The rotation mode can only be changed for backward compatibility of old WRONG code.
	*           PLEASE DO NOT CHANGE THE ROTATION MODE UNLESS YOU ARE KNOWING EXACTLY WHAT YOU ARE DOING!
	*/
	virtual void SetRotationMode(RotationMode r);

	/** @return Returns the rotation mode of this class. See documentation of enum
	*         RotationMode for details on the different modes.
	*/
	itkGetConstMacro(RotationMode,RotationMode);

	/**
	* \brief return current object state (Setup, Ready or Tracking)
	*/
	TrackingDeviceState GetState() const;

	/**
	* \brief Deprecated! Use the more specific getDeviceData instead. return device type identifier
	*/
	TrackingDeviceType GetType() const;
	/**
	* \brief Deprecated! Use the more specific setDeviceData instead. set device type
	*/
	void SetType(TrackingDeviceType type);

	/**
	* \brief return device data
	*/
	TrackingDeviceData GetData() const;
	/**
	* \brief set device type
	*/
	void SetData(TrackingDeviceData data);

protected:

	/**
	* \brief  change object state
	*/
	void SetState(TrackingDeviceState state);


	TrackingDevice();
	virtual ~TrackingDevice();

	TrackingDeviceData m_Data; ///< current device Data
	TrackingDeviceState m_State; ///< current object state (Setup, Ready or Tracking)
	bool m_StopTracking;       ///< signal stop to tracking thread
	itk::FastMutexLock::Pointer m_StopTrackingMutex; ///< mutex to control access to m_StopTracking
	itk::FastMutexLock::Pointer m_TrackingFinishedMutex; ///< mutex to manage control flow of StopTracking()
	itk::FastMutexLock::Pointer m_StateMutex; ///< mutex to control access to m_State
	RotationMode m_RotationMode; ///< defines the rotation mode Standard or Transposed, Standard is default
};

#endif /* TRACKINGDEVICE_H_HEADER_INCLUDED_C1C2FCD2 */
