#pragma once

#include <string.h>

class Circle
{
public:
	Circle();
	Circle(const Circle &circle)
	{
		memcpy(Center, circle.Center, sizeof(Center));
		memcpy(Normal, circle.Normal, sizeof(Normal));
		Radius = circle.Radius;
		Angle = circle.Angle;
		angleBettwenC2H = circle.angleBettwenC2H;
	}

	virtual ~Circle();
	void init();

	double Center[3];
	double Normal[3];
	double Radius;
	double Angle;
	double angleBettwenC2H; //angle bettwen circle and horizontal plane
};
