#pragma once

#include <QObject>

#include "trackingtypes.h"

struct CvMat;
class vtkMatrix3x3;
class vtkPoints;
class vtkLandmarkTransform;

#define  RAD2DEGREE   57.29564f
#define  DEGREE2RAD   0.017453f


class Fit3DCircle : public QObject
{
	Q_OBJECT

public:
	Fit3DCircle(QObject* parent);
	~Fit3DCircle();

	void calculate();
	void addPoint(const MarkerPointType &point);
	void clearPoints();
	bool getCircle(double center[3], double normal[3], double& radius);
	bool calRotateError(double& variance, double& mean);

private:
	bool isValid(void);
	bool isPointOnCircle(const MarkerPointType &point);
private:
	MarkerPointContainerType m_Positions;
	int m_Valid;

	double m_Center[3];//Բ������
	double m_Normal[3];//��������
	double m_Radius;//�뾶
	double m_ErrorVariance;
	double m_ErrorMean;
};
