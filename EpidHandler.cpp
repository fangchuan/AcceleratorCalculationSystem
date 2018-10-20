#include "epidhandler.h"
#include "horizontalregister.h"

#include "vtkPlaneSource.h"
#include "vtkPoints.h"
#include "vtkMath.h"

EpidHandler::EpidHandler(QObject *parent)
	: AbstractMonitorHandler(parent)
{
	m_EpidPlane = vtkSmartPointer<vtkPlaneSource>::New();
	m_PointCount = 0;
	m_registerPoints[0] = m_registerPoints[1] = m_registerPoints[2] = 0;
}

EpidHandler::~EpidHandler()
{
}

AbstractMonitorHandler *EpidHandler::handle(MarkerPointContainerType &positions)
{
	return this;
}

AbstractMonitorHandler *EpidHandler::handle(Point3D &point)
{
	if (m_registerPoints[0] != point[0] && m_registerPoints[1] != point[1] && m_registerPoints[2] != point[2])
	{
		m_registerPoints[0] = point[0];
		m_registerPoints[1] = point[1];
		m_registerPoints[2] = point[2];
	}
	else
	{
		emit registerToolTrackError();
		return NULL;
	}
	double out[3];
	// transform points to Horizontal plane
	m_Register->transform(m_registerPoints, out);
	emit epidPlanePointPosition(Point3D(out));

	if (m_PointCount == 0)
	{
		m_EpidPlane->SetOrigin(out);
	}
	if (m_PointCount == 1)
	{
		m_EpidPlane->SetPoint1(out);
	}
	if (m_PointCount == 2)
	{
		double horizontalPlaneNormal[3];
		m_EpidPlane->SetPoint2(out);

		m_EpidPlane->Update();
		m_EpidPlane->GetNormal(m_PlaneData.normal);
		vtkMath::Normalize(m_PlaneData.normal);

		if (m_Register->getHorizontalPlaneNormal(horizontalPlaneNormal))
		{
			m_PlaneData.angleBetweenP2H = vtkMath::AngleBetweenVectors(m_PlaneData.normal, horizontalPlaneNormal) * 57.29564;
			m_angleP2HContainer.push_back(m_PlaneData.angleBetweenP2H);

			emit epidPlaneResult(m_PlaneData);
		}

		m_PointCount = 0;
		return NULL;
	}

	m_PointCount++;
	return NULL;
}

bool EpidHandler::getRotateStatistical(double& angleMean)
{

	int size = m_angleP2HContainer.size();
	if (size > 0)
	{
		double sum;
		for (int i = 0; i < size; i++)
		{
			sum += m_angleP2HContainer.at(i);
		}
		angleMean = sum / size;
	}
	else
	{
		angleMean = 0;
		return false;
	}
	return true;

}

void EpidHandler::reset()
{
	m_PlaneData.normal[0] = 0;
	m_PlaneData.normal[1] = 0;
	m_PlaneData.normal[2] = 0;
	m_PlaneData.angleBetweenP2H = 0;
	m_angleP2HContainer.clear();
}


