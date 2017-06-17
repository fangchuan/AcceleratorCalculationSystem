#pragma once

#include <QWidget>

#include "trackingtypes.h"

class QLabel;

class TranslationDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	TranslationDisplayWidget(QWidget *parent = Q_NULLPTR);
	virtual ~TranslationDisplayWidget();

	void setMarkerSize(int size);
	void setMarkerPosition(MarkerPointType &point);
	void setTranslateResult(double bias[3]);
	void reset();
private:
	void initUi();

private:
	QLabel *m_MarkerSizeOrPosition;
	QLabel *m_XLabel;
	QLabel *m_YLabel;
	QLabel *m_ZLabel;
};
