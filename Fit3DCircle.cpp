#include "Fit3DCircle.h"
#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkLandmarkTransform.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include <vtkPlane.h>
#include "vtkSmartPointer.h"


Fit3DCircle::Fit3DCircle(QObject *parent)
	: QObject(parent),
	m_IsCalculated(false)
{
	K = vtkMatrix3x3::New();
	KT = vtkMatrix3x3::New();
	KTK = vtkMatrix3x3::New();
	m_Source = vtkPoints::New();
	m_Source->SetNumberOfPoints(3);
	m_Target = vtkPoints::New();
	m_Target->SetNumberOfPoints(3);
	m_Transform = vtkLandmarkTransform::New();
	m_Transform->SetModeToRigidBody();
	m_Transform->SetSourceLandmarks(m_Source);
	m_Transform->SetTargetLandmarks(m_Target);
}

Fit3DCircle::~Fit3DCircle()
{
	K->Delete();
	KT->Delete();
	KTK->Delete();
	m_Transform->Delete();
	m_Source->Delete();
	m_Target->Delete();
}

void Fit3DCircle::clearPoints()
{
	m_IsCalculated = false;
	m_Positions.clear();
}

bool Fit3DCircle::getCircle(double center[3], double normal[3], double& radius)
{
	if (m_IsCalculated) {
		memcpy(center, m_Center, sizeof(m_Center));
		memcpy(normal, m_Normal, sizeof(m_Normal));
		radius = m_Radius;
	}

	return m_IsCalculated;
}

void Fit3DCircle::addPoint(MarkerPointType &point)
{
	m_Positions.push_back(point);
	if (!m_IsCalculated) {
		if (m_Positions.size() > 3 && (fabs(point[0] - m_Positions.at(0)[0]) > 100 )) {   // threshold 100
			calculate();
		}
	}
	else {
		if (!isPointOnCircle(point)) {
			calculate();
		}
	}
}
//calculate circle in 3D
//https://www.zhihu.com/question/23427820
//
void Fit3DCircle::calculate()
{
	int size = m_Positions.size();
	double x2 = 0, xy = 0, xz = 0, y2 = 0, yz = 0, z2 = 0, X = 0, Y = 0, Z = 0;

	for (int i = 0; i < size; ++i)
	{
		const MarkerPointType& point = m_Positions.at(i);
		double x = point[0], y = point[1], z = point[2];
		x2 += x * x;
		xy += x * y;
		xz += x * z;
		y2 += y * y;
		yz += y * z;
		z2 += z * z;
		X += x;
		Y += y;
		Z += z;
	}

	K->SetElement(0, 0, x2); K->SetElement(0, 1, xy); K->SetElement(0, 2, xz);
	K->SetElement(1, 0, xy); K->SetElement(1, 1, y2); K->SetElement(1, 2, yz);
	K->SetElement(2, 0, xz); K->SetElement(2, 1, yz); K->SetElement(2, 2, z2);
	vtkMatrix3x3::Transpose(K, KT);
	vtkMatrix3x3::Multiply3x3(KT, K, KTK);
	KTK->Invert();
	vtkMatrix3x3::Multiply3x3(KTK, KT, K);
	double u[3], result[3];
	u[0] = X, u[1] = Y, u[2] = Z;
	K->MultiplyPoint(u, result);

	//平面方程即为 Ax + By + Cz - 1 = 0
	double A = result[0], B = result[1], C = result[2];

	m_Normal[0] = A;
	m_Normal[1] = B;
	m_Normal[2] = C;
	// normalize
	double length = sqrt(A*A + B*B + C*C);
	m_Normal[0] /= length;
	m_Normal[1] /= length;
	m_Normal[2] /= length;

	double Radius, Center[3];
	if (fabs(C) > 1e-15)
	{
		double P1[3], P2[3], P3[3], P4[3];
		P1[0] = 1, P1[1] = 1, P1[2] = (1 - A - B) / C;

		double xAxis[3], yAxis[3], zAxis[3];
		xAxis[0] = xAxis[1] = 1;
		xAxis[2] = (A + B) / C * -1;
		zAxis[0] = A, zAxis[1] = B, zAxis[2] = C;
		yAxis[0] = zAxis[1] * xAxis[2] - zAxis[2] * xAxis[1];
		yAxis[1] = zAxis[2] * xAxis[0] - zAxis[0] * xAxis[2];
		yAxis[2] = zAxis[0] * xAxis[1] - zAxis[1] * xAxis[0];
		double disX = 0, disY = 0, disZ = 0;
		for (int i = 0; i < 3; ++i)
		{
			disX += xAxis[i] * xAxis[i];
			disY += yAxis[i] * yAxis[i];
			disZ += zAxis[i] * zAxis[i];
		}
		disX = sqrt(disX);
		disY = sqrt(disY);
		disZ = sqrt(disZ);
		for (int i = 0; i < 3; ++i)
		{
			xAxis[i] /= disX;
			P2[i] = P1[i] + xAxis[i];
			yAxis[i] /= disY;
			P3[i] = P1[i] + yAxis[i];
			zAxis[i] /= disZ;
			P4[i] = P1[i] + zAxis[i];
		}

		m_Source->SetPoint(0, P2[0], P2[1], P2[2]);
		m_Source->SetPoint(1, P3[0], P3[1], P3[2]);
		m_Source->SetPoint(2, P4[0], P4[1], P4[2]);
		m_Source->Modified();
		m_Target->SetPoint(0, 1, 0, 0);
		m_Target->SetPoint(1, 0, 1, 0);
		m_Target->SetPoint(2, 0, 0, 1);
		m_Target->Modified();
		m_Transform->Update();
		vtkMatrix4x4 *matrix = m_Transform->GetMatrix();
		double derminant = matrix->Determinant();
		if (fabs(derminant - 1) < 1e-3){
			double X1 = 0;
			double Y1 = 0;
			double X2 = 0;
			double Y2 = 0;
			double X3 = 0;
			double Y3 = 0;
			double X1Y1 = 0;
			double X1Y2 = 0;
			double X2Y1 = 0;
			for (int i = 0; i < size; ++i){
				double in[4], out[4];
				in[0] = m_Positions.at(i)[0]; in[1] = m_Positions.at(i)[1]; in[2] = m_Positions.at(i)[2]; in[3] = 1.0;
				matrix->MultiplyPoint(in, out);
				X1 += out[0];
				Y1 += out[1];
				X2 += out[0] * out[0];
				Y2 += out[1] * out[1];
				X3 += out[0] * out[0] * out[0];
				Y3 += out[1] * out[1] * out[1];
				X1Y1 += out[0] * out[1];
				X1Y2 += out[0] * out[1] * out[1];
				X2Y1 += out[0] * out[0] * out[1];
			}
			double C, D, E, G, H, N;
			double a, b, c;
			N = size;
			C = N*X2 - X1*X1;
			D = N*X1Y1 - X1*Y1;
			E = N*X3 + N*X1Y2 - (X2 + Y2)*X1;
			G = N*Y2 - Y1*Y1;
			H = N*X2Y1 + N*Y3 - (X2 + Y2)*Y1;
			a = (H*D - E*G) / (C*G - D*D);
			b = (H*C - E*D) / (D*D - G*C);
			c = -(a*X1 + b*Y1 + X2 + Y2) / N;

			double AA, BB, R;
			AA = a / (-2);
			BB = b / (-2);
			R = sqrt(a*a + b*b - 4 * c) / 2;
			double center[4];
			center[0] = AA;
			center[1] = BB;
			center[2] = 0;
			center[3] = 1;
			Radius = R;

			m_Radius = Radius;

			matrix->Invert();
			double out[4];
			matrix->MultiplyPoint(center, out);
			Center[0] = out[0];
			Center[1] = out[1];
			Center[2] = out[2];

			m_Center[0] = out[0];
			m_Center[1] = out[1];
			m_Center[2] = out[2];

			m_IsCalculated = true;
		}
		else{
			m_IsCalculated = false;
		}
	}
	else
	{
		double X1 = 0;
		double Y1 = 0;
		double X2 = 0;
		double Y2 = 0;
		double X3 = 0;
		double Y3 = 0;
		double X1Y1 = 0;
		double X1Y2 = 0;
		double X2Y1 = 0;
		for (int i = 0; i < size; ++i)
		{
			double out[2];
			out[0] = m_Positions.at(i)[0]; out[1] = m_Positions.at(i)[1];
			X1 += out[0];
			Y1 += out[1];
			X2 += out[0] * out[0];
			Y2 += out[1] * out[1];
			X3 += out[0] * out[0] * out[0];
			Y3 += out[1] * out[1] * out[1];
			X1Y1 += out[0] * out[1];
			X1Y2 += out[0] * out[1] * out[1];
			X2Y1 += out[0] * out[0] * out[1];
		}
		double C, D, E, G, H, N;
		double a, b, c;
		N = size;
		C = N*X2 - X1*X1;
		D = N*X1Y1 - X1*Y1;
		E = N*X3 + N*X1Y2 - (X2 + Y2)*X1;
		G = N*Y2 - Y1*Y1;
		H = N*X2Y1 + N*Y3 - (X2 + Y2)*Y1;
		a = (H*D - E*G) / (C*G - D*D);
		b = (H*C - E*D) / (D*D - G*C);
		c = -(a*X1 + b*Y1 + X2 + Y2) / N;

		double AA, BB, R;
		AA = a / (-2);
		BB = b / (-2);
		R = sqrt(a*a + b*b - 4 * c) / 2;
		Center[0] = AA;
		Center[1] = BB;
		Center[2] = m_Positions.at(0)[2];
		Radius = R;

		m_Center[0] = Center[0];
		m_Center[1] = Center[1];
		m_Center[2] = Center[2];
		m_Radius = Radius;
		m_IsCalculated = true;
	}
}
//若点距圆心距离跟半径的差1.0，则认为点在圆上
bool Fit3DCircle::isPointOnCircle(const MarkerPointType &point)
{
	double p[3];
	p[0] = point[0]; p[1] = point[1]; p[2] = point[2];
	double dis = sqrt(vtkMath::Distance2BetweenPoints(p, m_Center));
	return fabs(dis - m_Radius) < 1.0;
}

bool Fit3DCircle::calRotateError(double& variance, double& mean)
{
	//若平面法向量为0，则返回
	if (m_Normal[0] == 0 && m_Normal[1] == 0 && m_Normal[2] == 0)
		return false;
	//构建轨迹圆所在的平面
	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	plane->SetNormal(m_Normal);
	plane->SetOrigin(m_Center);

	double sum = 0;
	std::vector<double> m_error;

	for (std::vector<MarkerPointType>::const_iterator itp = m_Positions.begin(); itp != m_Positions.end(); itp++){
		double d[3] = { (*itp)[0], (*itp)[1], (*itp)[2] };
		 double distance = plane->DistanceToPlane(d);
		 sum += distance;
		 m_error.push_back(distance);
	}
	//如果此时点集的大小跟误差集的大小不一致，或点集为0则返回
	if (m_Positions.size() != m_error.size())
		return false;
	if (m_error.size() == 0)
		return false;

	 m_ErrorMean = sum / m_error.size();
	 sum = 0;
	 for (std::vector<double>::const_iterator ite = m_error.begin(); ite != m_error.end(); ite++){
		 double e = *ite - m_ErrorMean;
		 sum += e * e;
	 }
	 m_ErrorVariance = sum / m_error.size();

	 variance = m_ErrorVariance;
	 mean = m_ErrorMean;

	 return true;
}
