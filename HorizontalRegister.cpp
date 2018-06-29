#include "horizontalregister.h"

#include "vtkLandmarkTransform.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"
#include "vtkPlaneSource.h"

HorizontalRegister* HorizontalRegister::m_Instance = nullptr;

HorizontalRegister::HorizontalRegister()
{
	m_Instance = this;
	m_Transform = vtkLandmarkTransform::New();
	m_Transform->SetModeToRigidBody();
	m_HorizontalPlaneNormal[0] = 0;
	m_HorizontalPlaneNormal[1] = 0;
	m_HorizontalPlaneNormal[2] = 0;
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
	//     
	vtkSmartPointer<vtkPlaneSource> horizontalPlane = vtkSmartPointer<vtkPlaneSource>::New();
	double p1[3], p2[3], p3[3];
	points->GetPoint(0, p1);
	points->GetPoint(1, p2);
	points->GetPoint(2, p3);
	double m = p2[0] - p1[0];//x2 - x1
	double n = p2[1] - p1[1];//y2 - y1
	double p = p2[2] - p1[2];//z2 - z1
	double t = (m*(p3[0] - p1[0]) + n*(p3[1] - p1[1]) + p*(p3[2] - p1[2])) / (m*m + n*n + p*p);//由向量共线得出的比例K
	double footPoint[3] = { m*t + p1[0], n*t + p1[1], p*t + p1[2] };//垂足
	double dis = sqrt(vtkMath::Distance2BetweenPoints(p3, footPoint));
	p3[0] = p3[1] = 0; p3[2] = -dis;
	dis = sqrt(vtkMath::Distance2BetweenPoints(footPoint, p1));
	p1[0] = -dis; p1[1] = p1[2] = 0;
	dis = sqrt(vtkMath::Distance2BetweenPoints(footPoint, p2));
	p2[0] = dis; p2[1] = p2[2] = 0;

	//make sure the horizontal plane
	horizontalPlane->SetOrigin(footPoint);
	horizontalPlane->SetPoint1(p3);
	horizontalPlane->SetPoint2(p1);
	horizontalPlane->Update();
	horizontalPlane->GetNormal(m_HorizontalPlaneNormal);//update the horizontal plane normal
	double maguitude = sqrt( m_HorizontalPlaneNormal[0]*m_HorizontalPlaneNormal[0]
					  + m_HorizontalPlaneNormal[1]*m_HorizontalPlaneNormal[1]
					  + m_HorizontalPlaneNormal[2]*m_HorizontalPlaneNormal[2]);
	m_HorizontalPlaneNormal[0] /= maguitude;
	m_HorizontalPlaneNormal[1] /= maguitude;
	m_HorizontalPlaneNormal[2] /= maguitude;

	//make sure the source points and the target points
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
		memcpy(source, in, sizeof(double) * 3);
		source[3] = 1.0;
		double target[4];
		matrix->MultiplyPoint(source, target);
		memcpy(out, target, sizeof(double) * 3);
	}
	return matrix != nullptr;
}

bool HorizontalRegister::getHorizontalPlaneNormal(double normal[3])
{
	if (m_HorizontalPlaneNormal[2] != 0 || m_HorizontalPlaneNormal[1] != 0 || m_HorizontalPlaneNormal[0] != 0)
	{
		memcpy(normal, m_HorizontalPlaneNormal, sizeof(m_HorizontalPlaneNormal));
		return true;
	}
	else
	{
		return false;
	}
}