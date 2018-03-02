#ifndef OpsTrackingDevice_H
#define OpsTrackingDevice_H

#include <QObject>

#include "vpsnditrackingdevice.h"

class OpsTrackingDevice : public QObject
{
	Q_OBJECT

public:
	static OpsTrackingDevice *getInstance();
	~OpsTrackingDevice();
	TrackingDeviceType findTracker();
	bool openConnection();
	bool closeConnection();
	bool setOperationMode(OperationMode mode);
	OperationMode getOperationMode();
	bool startTracking();
	bool stopTracking();
	bool startTrackingInMode(OperationMode mode);

	int getFrequency() const;
	TrackingDevice::TrackingDeviceState getState() const;

	void getToolPosition(Point3D& position) const;
	void getToolOrientation(Quaternion& orientation) const;
	void getToolPositionAndOrientation(Point3D& position, Quaternion& orientation) const;

	bool getMarkerPositions(MarkerPointContainerType* markerpositions);

signals:
	void OperationModeChanged(OperationMode mode);

private:
	OpsTrackingDevice();
	void initTrackingDevice();
	//���øú���ʱ�����뱣֤ת��ǰ��ģʽ��һ�����ɵ����߱�֤
	void swithModeAndTracking(OperationMode mode);

private:
	static OpsTrackingDevice *m_Instance;
	NDITrackingDevice::Pointer m_TrackingDevice;
	NDIPassiveTool::Pointer m_TrackingTool;
};

#endif // OpsTrackingDevice_H
