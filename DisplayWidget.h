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

	void doSwitchToHorizontalRegister();
	void doSwitchToGantry();
	void doSwitchToCollimator();
	void doSwitchToBed(int mode);
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
