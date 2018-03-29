#pragma once
#include <QWidget>


class QLabel;
struct Plane_T;

class CbctPositionDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	CbctPositionDisplayWidget(QWidget *parent = Q_NULLPTR);
	~CbctPositionDisplayWidget();

	void setPoint1Position(double point[3]);
	void setPoint2Position(double point[3]);
	void setPoint3Position(double point[3]);

	void setPlaneResult(Plane_T& plane);
	void reset();
private:
	void initUi();

private:
	QLabel *m_point1Label;
	QLabel *m_point2Label;
	QLabel *m_point3Label;
	QLabel* m_normalLabel;
	QLabel* m_angleLabel;
};
