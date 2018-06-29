#pragma once

#include "abstractmonitorhandler.h"

#include "vtkSmartPointer.h"

class vtkPlaneSource;

struct Plane_T {
	double normal[3];  // ƽ�淨����
	double angleBetweenP2H;  //ƽ����ˮƽ��н�
};

class CbctPositionHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	CbctPositionHandler(QObject *parent = Q_NULLPTR);
	virtual ~CbctPositionHandler();

	virtual AbstractMonitorHandler* handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler* handle(Point3D &point);

	virtual void reset();
	bool getRotateStatistical(double& angleMean);
signals:

	void cbctPlanePointPosition(Point3D& point);;
	void planeResult(Plane_T& plane);

private:
	vtkSmartPointer<vtkPlaneSource> m_CbctPlane;
	int m_PointCount;
	Plane_T m_PlaneData;

	std::vector<double>  m_angleP2HContainer;
};