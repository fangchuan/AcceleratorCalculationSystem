#include "cbcthandler.h"
#include "horizontalregister.h"
#include "fit3dcircle.h"
#include "circle.h"

#include "vtkPoints.h"
#include "vtkMath.h"

CbctHandler::CbctHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_FitCircle(new Fit3DCircle(this))
{

}

CbctHandler::~CbctHandler()
{
}

AbstractMonitorHandler *CbctHandler::handle(MarkerPointContainerType &positions)
{
	int size = positions.size();
	if (size != 1) {
		//看到多个标定球则直接返回
		emit pseudoMarkerSize(size);
		return NULL;
	}

	//只取第一个标定球位置参与圆拟合
	MarkerPointType p = positions.at(0);
	double point[3];
	point[0] = p[0]; point[1] = p[1]; point[2] = p[2];
	double out[3];
	m_Register->transform(point, out);
	MarkerPointType  marker(out);

	emit markerPosition(marker);

	m_FitCircle->addPoint(marker);
	double center[3], normal[3], horizontalPlaneNormal[3], radius;
	if (m_FitCircle->getCircle(center, normal, radius)) {

		double rad = (out[1] - center[1]) / radius;
		rad = rad > 1 ? 1 : rad;
		rad = rad < -1 ? -1 : rad;
		double angle = acos(rad) * RAD2DEGREE;

		angle *= out[0] < center[0] ? 1 : -1;//逆时针旋转为+

		Circle circle;
		memcpy(circle.Center, center, sizeof(center));
		memcpy(circle.Normal, normal, sizeof(normal));
		circle.Radius = radius;
		circle.Angle = angle;
		if (m_Register->getHorizontalPlaneNormal(horizontalPlaneNormal))
		{
			circle.angleBettwenC2H = vtkMath::AngleBetweenVectors(normal, horizontalPlaneNormal);
			m_angleC2HContainer.push_back(circle.angleBettwenC2H);
		}
		emit circleResult(&circle);
	}

	return this;
}

AbstractMonitorHandler *CbctHandler::handle(Point3D &point)
{
	emit markerPosition(point);
	return this;
}

void CbctHandler::reset()
{
	m_FitCircle->clearPoints();
}

bool CbctHandler::getRotateStatistical(double& variance, double& mean)
{
	if (m_FitCircle->calRotateError(variance, mean))
	{
		int size = m_angleC2HContainer.size();
		if (size > 0)
		{
			double sum;
			for (int i = 0; i < size; i++)
			{
				sum += m_angleC2HContainer.at(i);
			}
			//angleMean = sum / size;
		}
		else
		{
			//angleMean = 0;
		}
		return true;
	}
	else
		return false;
}