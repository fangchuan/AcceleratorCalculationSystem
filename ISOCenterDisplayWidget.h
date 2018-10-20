#pragma once

#include <QWidget>


class QLabel;

class ISOCenterDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	ISOCenterDisplayWidget(QWidget *parent = Q_NULLPTR);
	~ISOCenterDisplayWidget();

	void setRegisteredPosition(double point[3]);
	void reset();
private:
	void initUi();

private:

	QLabel *m_XLabel;
	QLabel *m_YLabel;
	QLabel *m_ZLabel;
};
