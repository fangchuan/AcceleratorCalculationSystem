#pragma once

#include "abstractmonitorhandler.h"

#include "vtkSmartPointer.h"
#include "cbctpositionhandler.h"

class vtkPlaneSource;

class EpidHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	EpidHandler(QObject *parent = Q_NULLPTR);
	virtual ~EpidHandler();

	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();
	bool getRotateStatistical(double& angleMean);
signals:

	void epidPlanePointPosition(Point3D& point);;
	void epidPlaneResult(Plane_T& plane);
	void registerToolTrackError();
private:
	vtkSmartPointer<vtkPlaneSource> m_EpidPlane;
	int m_PointCount;
	Plane_T m_PlaneData;
	double m_registerPoints[3];

	std::vector<double>  m_angleP2HContainer;
};
