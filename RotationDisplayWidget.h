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

private:
	void initUi();

private:
	QLabel *m_MarkerSizeOrPosition;
	QLabel *m_ParallelOrPerpendicular;
	QLabel *m_CurrentAngle;
};
