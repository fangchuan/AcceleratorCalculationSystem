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

	static HorizontalRegister *getInstance();

private:
	static HorizontalRegister *m_Instance;
	vtkLandmarkTransform *m_Transform;
};

