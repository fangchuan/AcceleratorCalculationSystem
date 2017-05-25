#pragma once

#include <QObject>

#include "trackingtypes.h"
#include "vpsvector.h"
#include "Circle.h"

class AbstractMonitorHandler;
class HorizontalRegisterHandler;
class GantryHandler;
class CollimatorHandler;
class BedHandler;
class ISOCenterHandler;


#define  HORIZONTALREGISTER_HANDLER  1
#define	 GANTRY_HANDLER              2
#define  COLLIMATOR_HANDLER			 3
#define  BED_HANDLER				 4
#define  ISOCENTER_HANDLER			 5


class CentralModel : public QObject
{
	Q_OBJECT

public:
	CentralModel(QObject *parent = Q_NULLPTR);
	~CentralModel();

	void setHandlerToNone();
	void setHandlerToHorizontalRegister();
	void setHandlerToGantry();
	void setHandlerToCollimator();
	void setHandlerToBed(int mode);
	void setHandlerToISOCenter();
	int  getHandler();
	void handle(MarkerPointContainerType &positions);
	void handle(Point3D &point);

signals:
	void markerSize(int size);
	void registerPosition(Point3D &point);
	void horizontalRegisterRecorded();
	void markerPosition(MarkerPointType &point);
	void circleResult(Circle *circle);
	void translateResult(double bias[3]);

private:
	void buildConnections();

private slots:
	
private:
	AbstractMonitorHandler *m_Handler;
	HorizontalRegisterHandler *m_HorizontalRegisterHandler;
	GantryHandler *m_GantryHandler;
	CollimatorHandler *m_CollimatorHandler;
	BedHandler *m_BedHandler;
	ISOCenterHandler* m_ISOCenterHanlder;
};
