#pragma once

class vtkLandmarkTransform;
class vtkPoints;

class HorizontalRegister
{
public:
	HorizontalRegister();
	virtual ~HorizontalRegister();

	void setPoints(vtkPoints *points);
	bool transform(double in[3], double out[3]);
	bool getHorizontalPlaneNormal(double normal[3]);
	static HorizontalRegister *getInstance();

private:
	static HorizontalRegister *m_Instance;
	vtkLandmarkTransform *m_Transform;
	double m_HorizontalPlaneNormal[3];

};