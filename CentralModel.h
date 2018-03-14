#pragma once

#include <QObject>

#include "trackingtypes.h"
#include "vpsvector.h"
#include "circle.h"

class AbstractMonitorHandler;
class HorizontalRegisterHandler;
class GantryHandler;
class CollimatorHandler;
class BedHandler;
class ISOCenterHandler;
class LightCenterHandler;
class CbctHandler;


#define  HORIZONTALREGISTER_HANDLER  1
#define	 GANTRY_HANDLER              2
#define  COLLIMATOR_HANDLER			 3
#define  BED_HANDLER				 4
#define  ISOCENTER_HANDLER			 5
#define  LIGHTCENTER_HANDLER         6
#define  CBCT_HANDLER				 7

typedef struct ReportData{
	double softCenter[3];
	double laserCenter[3];
	double lightCenter[3];
	double footA[3];
	double footB[3];
	double distanceLaser2Soft;
	double gantryVar;
	double gantryMean;
	double gantryAngle;//gantry circle angle between horizontal plane
	double bedVar;
	double bedMean;
	double bedAngle;//bed circle angle between horizontal plane
	double cbctVar;
	double cbctMean;
	double cbctAngle;//cbct circle angle between horizontal plane
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
	void setHandlerToLightCenter();
	void setHandlerToCbct();
	int  getHandler();
	void handle(MarkerPointContainerType &positions);
	void handle(Point3D &point);
	void handleReport();
	void resetAccelerator();
signals:
	void markerSize(int size);
	void pseudoMarkerSize(int size);
	void registerLaserISO(Point3D &point);
	void registerLightCenter(Point3D &point);
	void horizontalRegisterRecorded();
	void horizontalRegisterFailed();
	void loadHorizontalRegisterData(int index);
	void markerPosition(MarkerPointType &point);
	void circleResult(Circle *circle);
	void translateResult(double bias[3]);
	void sendReport(const ReportData &reportData);
private:
	bool initTimer();
	void initData();
	void buildConnections();

	virtual void timerEvent(QTimerEvent* event);
private slots:
	void laserISOCenterPosition(Point3D& point);
	void lightCenterPosition(Point3D& point);
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
	LightCenterHandler* m_LightCenterHanlder;
	CbctHandler*   m_CbctHandler;

	bool m_GantryFitted;
	bool m_BedFitted;
	bool m_softCenterIsCal;
	bool m_LaserISODetected;
	bool m_LightCenterDetected;
	bool m_ReadyToReport;
	Circle* gantryCircle;
	Circle* bedCircle;
	ReportData reportData;
};
