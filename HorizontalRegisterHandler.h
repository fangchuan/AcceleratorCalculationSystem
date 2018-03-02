#pragma once

#include "abstractmonitorhandler.h"

class HorizontalRegister;
class vtkPoints;

class HorizontalRegisterHandler : public AbstractMonitorHandler
{
	Q_OBJECT

public:
	HorizontalRegisterHandler(QObject *parent);
	virtual ~HorizontalRegisterHandler();

	void setTolerence(double tolerence);
	virtual AbstractMonitorHandler *handle(MarkerPointContainerType &positions);
	virtual AbstractMonitorHandler *handle(Point3D &point);
	virtual void reset() {}

signals:
	void markerSize(int size);
	void horizontalRegisterRecorded();
	void horizontalRegisterFailed();
	void loadHorizontalRegisterData(int index);
	
private:
	void loadHorizontalRegister();
	void loadHorizontalRegister(MarkerPointContainerType &positions);
private:
	vtkPoints *m_SourcePoints;
	double m_Tolerence;
};
