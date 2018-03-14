#pragma once

#include <QWidget>

#include "trackingtypes.h"

class QStackedWidget;
class HorizontalRegisterDisplayWidget;
class RotationDisplayWidget;
class TranslationDisplayWidget;
class ISOCenterDisplayWidget;
class Circle;

class DisplayWidget : public QWidget
{
	Q_OBJECT

public:
	DisplayWidget(QWidget *parent = Q_NULLPTR);
	~DisplayWidget();
	//复位显示面板
	void reset();
	//显示面板切换到水平面注册
	void doSwitchToHorizontalRegister();
	//显示面板切换到机架旋转
	void doSwitchToGantry();
	//显示面板切换到准直器旋转
	void doSwitchToCollimator();
	void doSwitchToCbct();
	//显示面板切换到治疗床
	void doSwitchToBed(int mode);
	//显示面板切换到激光等中心
	void doSwitchToISOCenter();

	void setMarkerSize(int size);
	void setMarkerPosition(MarkerPointType &point);
	void horizontalRegisterRecorded();
	void setCircleResult(Circle *circle);
	void setTranslateResult(double bias[3]);
	void setRegisteredPosition(double point[3]);

private:
	void initUi();

private:
	QStackedWidget *m_StackedWidget;
	HorizontalRegisterDisplayWidget *m_HorizontalWidget;
	RotationDisplayWidget *m_RotationWidget;
	TranslationDisplayWidget *m_TranslationWidget;
	ISOCenterDisplayWidget *m_ISOCenterDisplayWidget;
};
