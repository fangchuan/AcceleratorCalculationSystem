#include "HorizontalRegister.h"

#include "vtkLandmarkTransform.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"

HorizontalRegister* HorizontalRegister::m_Instance = nullptr;

HorizontalRegister::HorizontalRegister()
{
	m_Instance = this;
	m_Transform = vtkLandmarkTransform::New();
	m_Transform->SetModeToRigidBody();
}


HorizontalRegister::~HorizontalRegister()
{
	if (m_Transform) {
		m_Transform->Delete();
	}
}

HorizontalRegister *HorizontalRegister::getInstance()
{
	return m_Instance == nullptr ? new HorizontalRegister : m_Instance;
}
//
void HorizontalRegister::setPoints(vtkPoints *points)
{
	int numberOfPoints = points->GetNumberOfPoints();
	if (numberOfPoints != 3) {
		return;
	}
    //计算空间中一点到一条直线垂足
    //http://blog.sina.com.cn/s/blog_648868460100o4ar.html
	double p1[3], p2[3], p3[3];
	points->GetPoint(0, p1);
	points->GetPoint(1, p2);
	points->GetPoint(2, p3);
	double m = p2[0] - p1[0];
	double n = p2[1] - p1[1];
	double p = p2[2] - p1[2];
	double t = (m*(p3[0] - p1[0]) + n*(p3[1] - p1[1]) + p*(p3[2] - p1[2])) / (m*m + n*n + p*p);
	double footPoint[3] = { m*t + p1[0], n*t + p1[1], p*t + p1[2] };
	double dis = sqrt(vtkMath::Distance2BetweenPoints(p3, footPoint));
	p3[0] = p3[1] = 0; p3[2] = -dis;
	dis = sqrt(vtkMath::Distance2BetweenPoints(footPoint, p1));
	p1[0] = -dis; p1[1] = p1[2] = 0;
	dis = sqrt(vtkMath::Distance2BetweenPoints(footPoint, p2));
	p2[0] = dis; p2[1] = p2[2] = 0;
	vtkPoints *target = vtkPoints::New();
	target->SetNumberOfPoints(3);
	target->SetPoint(0, p1);
	target->SetPoint(1, p2);
	target->SetPoint(2, p3);
	vtkPoints *source = vtkPoints::New();
	source->DeepCopy(points);
	m_Transform->SetSourceLandmarks(source);
	source->Delete();
	m_Transform->SetTargetLandmarks(target);
	target->Delete();
	m_Transform->Update();
}

bool HorizontalRegister::transform(double in[3], double out[3])
{
	vtkMatrix4x4 *matrix = m_Transform->GetMatrix();
	if (matrix) {
		double source[4];
		memcpy(source, in, sizeof(double)*3);
		source[3] = 1.0;
		double target[4];
		matrix->MultiplyPoint(source, target);
		memcpy(out, target, sizeof(double)*3);
	}
	return matrix != nullptr;
}
