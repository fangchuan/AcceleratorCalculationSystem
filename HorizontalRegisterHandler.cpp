#include "horizontalregisterhandler.h"
#include "horizontalregister.h"

#include "vtkPoints.h"
#include "vtkMath.h"

#include <QFile>
#include <QDataStream>
#include <qmessagebox.h>
#include <QTextCodec>
#include <QCoreApplication>
#include <QDebug>


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
	if (QFile::exists("./Resources/data/HorizontalRegister.dat")) {
		QFile file("./Resources/data/HorizontalRegister.dat");
		QDataStream out(&file);

		if (file.open(QIODevice::ReadOnly)) {
			double point[3];
			m_SourcePoints->SetNumberOfPoints(3);
			for (int i = 0; i < 3; ++i) {
				out >> point[0] >> point[1] >> point[2];
				m_SourcePoints->SetPoint(i, point);

			}
		}
		file.close();
	}
	else {
		QMessageBox::warning(Q_NULLPTR, QCoreApplication::applicationName(), QObject::tr("No registration data found!"));//
	}
}
//当配套水平注册仪的三点距离发生变化时，直接以当前水平注册仪的三点坐标作为水平注册文件
void HorizontalRegisterHandler::loadHorizontalRegister(MarkerPointContainerType &positions)
{
	QFile file("HorizontalRegister.dat");
	QDataStream in(&file);

	if (!QFile::exists("HorizontalRegister.dat")) {
		if (file.open(QIODevice::WriteOnly)) {
			double p[3] = { positions.at(0)[0], positions.at(0)[1], positions.at(0)[2] };
			in << p[0] << p[1] << p[2];
			m_SourcePoints->InsertNextPoint(p);
			file.close();
		}
	}
	else {
		if (file.open(QIODevice::Append)) {
			double p[3] = { positions.at(0)[0], positions.at(0)[1], positions.at(0)[2] };
			in << p[0] << p[1] << p[2];
			m_SourcePoints->InsertNextPoint(p);
			file.close();
		}
	}
}

void HorizontalRegisterHandler::setTolerence(double tolerence)
{
	m_Tolerence = tolerence;
}
//注册水平面
//将NDI相机看到的三个点与事先确定好的水平面注册点(NDI Coordinate)作比较,只比较每个点之间的两两距离
//若距离差在1mm内，则注册水平仪当前平面为加速器水平面，生成NDI->Accelerator坐标系转换矩阵
//m_SourcePoints即为原始三个点，只为计算三点间距离。
//用原始距离比较当前相机看到的三个点间距离，从而选择正确的三个点作为水平面上的点，然后SetPoints得到NDI-->加速器变换矩阵
AbstractMonitorHandler *HorizontalRegisterHandler::handle(MarkerPointContainerType &positions)
{
	int targetSize = positions.size();
	if (targetSize == 1) {
		static int counts = 1;
		loadHorizontalRegister(positions);
		emit loadHorizontalRegisterData(counts++);

		return NULL;
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
		MarkerPointType p = positions.at(i);
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
	emit horizontalRegisterFailed();
	return NULL;
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

	double horizontalNormal[3];
	m_Register->getHorizontalPlaneNormal(horizontalNormal);

	emit markerSize(3);
	emit horizontalRegisterRecorded(horizontalNormal);

	return nullptr;
}

AbstractMonitorHandler *HorizontalRegisterHandler::handle(Point3D &point)
{
	return this;
}