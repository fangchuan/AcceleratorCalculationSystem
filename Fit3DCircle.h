#pragma once

#include <QObject>

#include "trackingtypes.h"

class vtkMatrix3x3;
class vtkPoints;
class vtkLandmarkTransform;

class Fit3DCircle : public QObject
{
	Q_OBJECT

public:
	Fit3DCircle(QObject *parent);
	~Fit3DCircle();

	void addPoint(MarkerPointType &point);
	void clearPoints();
	bool getCircle(double center[3], double normal[3], double& radius);

signals:
	void fitCircle(double center[3], double radius);

private:
	void calculate();
	bool isPointOnCircle(const MarkerPointType &point);

private:
	MarkerPointContainerType m_Positions;
	bool m_IsCalculated;
	vtkMatrix3x3 *K;
	vtkMatrix3x3 *KT;
	vtkMatrix3x3 *KTK;
	vtkPoints *m_Source;
	vtkPoints *m_Target;
	vtkLandmarkTransform *m_Transform;
	double m_Center[3];//圆心坐标
	double m_Normal[3];//法线向量
	double m_Radius;//半径
};
