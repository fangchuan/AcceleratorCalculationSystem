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


typedef struct ReportData{
	double softCenter[3];
	double laserCenter[3];
	double footA[3];
	double footB[3];
	double distanceLaser2Soft;
	double gantryVar;
	double gantryMean;
	double bedVar;
	double bedMean;

}ReportData;
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
	void handleReport();
	void resetAccelerator();
signals:
	void markerSize(int size);
	void registerPosition(Point3D &point);
	void horizontalRegisterRecorded();
	void markerPosition(MarkerPointType &point);
	void circleResult(Circle *circle);
	void translateResult(double bias[3]);
	void sendReport(const ReportData & reportData);
private:
	bool initTimer();
	void initData();
	void buildConnections();

	virtual void timerEvent(QTimerEvent* event);
private slots:
	void laserISOCenter(Point3D& point);
	void softISOCenter(Circle* circle);
	//计算软件等中心和公垂线垂足
	void calSoftISOCenterAndFoots(Circle* gantry, Circle* bed);
private:
	AbstractMonitorHandler *m_Handler;
	HorizontalRegisterHandler *m_HorizontalRegisterHandler;
	GantryHandler *m_GantryHandler;
	CollimatorHandler *m_CollimatorHandler;
	BedHandler *m_BedHandler;
	ISOCenterHandler* m_ISOCenterHanlder;

	bool m_GantryFitted;
	bool m_BedFitted;
	bool m_softCenterIsCal;
	bool m_LaserISODetected;
	bool m_ReadyToReport;
	Circle* gantryCircle;
	Circle* bedCircle;
	ReportData reportData;
};
