#pragma once

#include <QWidget>

#include "trackingtypes.h"
#include "vpsvector.h"
#include "DisplayWidget.h"
#include "ControlWidget.h"
#include "OpsTrackingDevice.h"
#include "CentralModel.h"
#include "Circle.h"
#include "RenderView.h"
#include "PlotView.h"
#include "LoggerView.h"
#include "logger.h"

class QStackedWidget;
class DisplayWidget;
class ControlWidget;
class OpsTrackingDevice;
class QTimer;
class CentralModel;
class Circle;
class RenderView;
class PlotView;
class LoggerView;


#define  STACKED_THREED_VIEW_INDEX   0
#define  STACKED_PLOT_VIEW_INDEX     1
#define  STACKED_REPORT_INDEX        2

class CentralWidget : public QWidget
{
	Q_OBJECT

public:
	CentralWidget(QWidget *parent = Q_NULLPTR);
	~CentralWidget();

public slots:
	void show3DPage();
	void showPlotPage();
	void showReportPage();
	void exportReport();
	void printReport();
	void printPreviewReport();
	void saveReport();
	void saveAsReport();
	void clearAllPlot();

private:
	void initUi();
	void initLogger();
	void initCamera();
	void initTimer();
	void buildConnections();
	void monitoringMarker3D();
	void monitoringTool6D();

private slots:
	void TrackingModeChanged(OperationMode mode);
	void monitoring();
	void switchToHorizontalRegister();
	void recordingHorizontalRegister();
	void switchToGantry();
	void recordingGantry();
	void switchToCollimator();
	void recordingCollimator();
	void switchToBed(int mode);
	void recordingBed(int mode);
	void switchToLaserISO();
	void recordingLaserISO();
	void switchToLightCenter();
	void recordingLightCenter();
	void markerSize(int size);
	void markerPosition(MarkerPointType &point);
	void horizontalRegisterRecorded();
	void circleResult(Circle *circle);
	void translateResult(double bias[3]);
	void registerLaserISOPosition(Point3D& point);
	void registerLightCenterPosition(Point3D &point);
	void reportResult( const ReportData & report);
	void handleReset();

private:
	QStackedWidget*			m_StackedWidget;//
	DisplayWidget*			m_DisplayWidget;//3D坐标显示面板 对象
	ControlWidget*			m_ControlWidget;//控制面板对象
	QTimer*							m_Timer;
	OpsTrackingDevice*			  m_Tracker;
	OperationMode			 m_TrackingMode;
	CentralModel*					m_Model;
	QtRenderView::RenderView*  renderWidget;//3维显示面板对象
	PlotView*					 plotWidget;//曲线显示面板对象
	LoggerView*                loggerWidget;  //报告视图 对象
	Logger*							 logger;  //日志 对象
};
