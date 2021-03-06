#include "cbctpositionhandler.h"
#include "horizontalregister.h"

#include "vtkPlaneSource.h"
#include "vtkPoints.h"
#include "vtkMath.h"

CbctPositionHandler::CbctPositionHandler(QObject *parent)
	: AbstractMonitorHandler(parent)
{
	m_CbctPlane = vtkSmartPointer<vtkPlaneSource>::New();
	m_PointCount = 0;
	m_registerPoints[0] = m_registerPoints[1] = m_registerPoints[2] = 0;
}

CbctPositionHandler::~CbctPositionHandler()
{
}

AbstractMonitorHandler *CbctPositionHandler::handle(MarkerPointContainerType &positions)
{
	return this;
}

AbstractMonitorHandler *CbctPositionHandler::handle(Point3D &point)
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
	emit cbctPlanePointPosition(Point3D(out));

	if (m_PointCount == 0)
	{
		m_CbctPlane->SetOrigin(out);
	}	
	if (m_PointCount == 1)
	{
		m_CbctPlane->SetPoint1(out);
	}
	if (m_PointCount == 2)
	{
		double horizontalPlaneNormal[3];
		m_CbctPlane->SetPoint2(out);

		m_CbctPlane->Update();
		m_CbctPlane->GetNormal(m_PlaneData.normal);
		vtkMath::Normalize(m_PlaneData.normal);

		if (m_Register->getHorizontalPlaneNormal(horizontalPlaneNormal))
		{
			m_PlaneData.angleBetweenP2H = vtkMath::AngleBetweenVectors(m_PlaneData.normal, horizontalPlaneNormal) * 57.29564;
			m_angleP2HContainer.push_back(m_PlaneData.angleBetweenP2H);

			emit cbctPlaneResult(m_PlaneData);
		}

		m_PointCount = 0;
		return NULL;
	}
	
	m_PointCount++;
	
	
	return NULL;
}

bool CbctPositionHandler::getRotateStatistical(double& angleMean)
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

void CbctPositionHandler::reset()
{
	m_PlaneData.normal[0] = 0;
	m_PlaneData.normal[1] = 0;
	m_PlaneData.normal[2] = 0;
	m_PlaneData.angleBetweenP2H = 0;
	m_angleP2HContainer.clear();
}


