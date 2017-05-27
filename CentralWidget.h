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


class QStackedWidget;
class DisplayWidget;
class ControlWidget;
class OpsTrackingDevice;
class QTimer;
class CentralModel;
class Circle;
class RenderView;
class PlotView;


#define  STACKED_THREED_VIEW_INDEX   0
#define  STACKED_PLOT_VIEW_INDEX     1
#define  STACKED_PLAINTEXT_INDEX     2

class CentralWidget : public QWidget
{
	Q_OBJECT

public:
	CentralWidget(QWidget *parent = Q_NULLPTR);
	~CentralWidget();

public slots:
	void show3DPage();
	void showPlotPage();
	void showPlainTextPage();
	void clearAllPlot();
private:
	void initUi();
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
	void switchToISOCenter();
	void recordingISOCenter();
	void markerSize(int size);
	void markerPosition(MarkerPointType &point);
	void horizontalRegisterRecorded();
	void circleResult(Circle *circle);
	void translateResult(double bias[3]);
	void registerPosition(Point3D &point);

private:
	QStackedWidget*			m_StackedWidget;
	DisplayWidget*			m_DisplayWidget;
	ControlWidget*			m_ControlWidget;
	QTimer*							m_Timer;
	OpsTrackingDevice*			  m_Tracker;
	OperationMode			 m_TrackingMode;
	CentralModel*					m_Model;
	QtRenderView::RenderView*  renderWidget;
	PlotView*					 plotWidget;
};
