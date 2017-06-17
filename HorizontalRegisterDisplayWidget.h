#pragma once

#include <QWidget>

class QLabel;

class HorizontalRegisterDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	HorizontalRegisterDisplayWidget(QWidget *parent = Q_NULLPTR);
	~HorizontalRegisterDisplayWidget();

	void setMarkerSize(int number);
	void horizontalRegisterRecorded();
	void reset();
private:
	void initUi();

private:
	QLabel *m_MarkerSizeLabel;
	QLabel *m_StatusLabel;
};
