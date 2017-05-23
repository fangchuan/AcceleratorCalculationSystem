#include "HorizontalRegisterHandler.h"
#include "HorizontalRegister.h"

#include "vtkPoints.h"
#include "vtkMath.h"

#include <QFile>
#include <QDataStream>

HorizontalRegisterHandler::HorizontalRegisterHandler(QObject *parent)
	: AbstractMonitorHandler(parent),
	m_SourcePoints(vtkPoints::New()),
	m_Tolerence(1.0)
{
	loadHorizontalRegister();
}

HorizontalRegisterHandler::~HorizontalRegisterHandler()
{
	if (m_Register) {
		delete m_Register;
	}
	if (m_SourcePoints) {
		m_SourcePoints->Delete();
	}
}

void HorizontalRegisterHandler::loadHorizontalRegister()
{
	QFile file("./Resources/dat/HorizontalRegister.dat");
	QDataStream stream(&file);
	if (file.open(QFile::ReadOnly)) {
		double point[3];
		m_SourcePoints->SetNumberOfPoints(3);
		for (int i = 0; i < 3; ++i) {
			stream >> point[0] >> point[1] >> point[2];
			m_SourcePoints->SetPoint(i, point);
		}
		file.close();
	}
}

void HorizontalRegisterHandler::setTolerence(double tolerence)
{
	m_Tolerence = tolerence;
}
//注册水平面
//将NDI相机看到的三个点与事先确定好的水平面注册点(NDI Coordinate)作比较,只比较每个点之间的两两距离
//若距离差在1mm内，则注册水平仪当前平面为加速器水平面，生成NDI->Accelerator坐标系转换矩阵
AbstractMonitorHandler *HorizontalRegisterHandler::handle(MarkerPointContainerType &positions)
{
	int targetSize = positions.size();
	if (targetSize < 3) {
		return this;
	}
	int index[3] = { -1, -1, -1 };
	double sourceDis[3] = { 0.0, 0.0, 0.0 };
	double point1[3], point2[3], point3[3];
	m_SourcePoints->GetPoint(0, point1);
	m_SourcePoints->GetPoint(1, point2);
	m_SourcePoints->GetPoint(2, point3);
	sourceDis[0] = sqrt(vtkMath::Distance2BetweenPoints(point1, point2));
	sourceDis[1] = sqrt(vtkMath::Distance2BetweenPoints(point1, point3));
	sourceDis[2] = sqrt(vtkMath::Distance2BetweenPoints(point2, point3));
	double pt1[3], pt2[3];
	for (int i = 0; i < targetSize; ++i)
	{
		MarkerPointType &p = positions.at(i);
		pt1[0] = p[0]; pt1[1] = p[1]; pt1[2] = p[2];
		for (int k = 0; k < targetSize; ++k)
		{
			p = positions.at(k);
			pt2[0] = p[0]; pt2[1] = p[1]; pt2[2] = p[2];
			double dis1 = sqrt(vtkMath::Distance2BetweenPoints(pt1, pt2));
			if (fabs(dis1 - sourceDis[0]) < m_Tolerence)
			{
				for (int j = 0; j < targetSize; ++j)
				{
					p = positions.at(i);
					pt1[0] = p[0]; pt1[1] = p[1]; pt1[2] = p[2];
					p = positions.at(j);
					pt2[0] = p[0]; pt2[1] = p[1]; pt2[2] = p[2];
					double dis2 = sqrt(vtkMath::Distance2BetweenPoints(pt1, pt2));
					p = positions.at(k);
					pt1[0] = p[0]; pt1[1] = p[1]; pt1[2] = p[2];
					double dis3 = sqrt(vtkMath::Distance2BetweenPoints(pt1, pt2));
					if (fabs(dis2 - sourceDis[1]) < m_Tolerence)
					{
						if (fabs(dis3 - sourceDis[2]) < m_Tolerence)
						{
							index[0] = i;
							index[1] = k;
							index[2] = j;
							goto label;
						}
					}
					if (fabs(dis3 - sourceDis[1]) < m_Tolerence)
					{
						if (fabs(dis2 - sourceDis[2]) < m_Tolerence)
						{
							index[0] = k;
							index[1] = i;
							index[2] = j;
							goto label;
						}
					}
				}
			}
		}
	}
	emit markerSize(targetSize);
	return this;
label:
	vtkPoints *points = vtkPoints::New();
	points->SetNumberOfPoints(3);
	double point[3];
	for (int i = 0; i < 3; ++i) {
		MarkerPointType &p = positions.at(index[i]);
		point[0] = p[0];
		point[1] = p[1];
		point[2] = p[2];
		points->SetPoint(i, point);
	}
	m_Register->setPoints(points);
	points->Delete();
	emit markerSize(3);
	emit horizontalRegisterRecorded();
	return nullptr;
}

AbstractMonitorHandler *HorizontalRegisterHandler::handle(Point3D &point)
{
	return this;
}
