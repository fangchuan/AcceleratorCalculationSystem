#pragma once

#include <QWidget>

#include "trackingtypes.h"

class QLabel;
class Circle;

class RotationDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	RotationDisplayWidget(QWidget *parent = Q_NULLPTR);
	virtual ~RotationDisplayWidget();

	void setMarkerSize(int size);
	void setMarkerPosition(MarkerPointType &point);
	void setCurrentAngle(double angle);
	void setCircleResult(Circle *circle);
	void reset();
private:
	void initUi();

private:
	QLabel *m_MarkerSizeOrPosition;
	QLabel *m_AngleBettwenHorizontal;//旋转平面与水平面之间的夹角
	QLabel *m_CurrentAngle;
};
