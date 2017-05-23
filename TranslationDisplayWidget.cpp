#include "TranslationDisplayWidget.h"

#include <QLabel>
#include <QVBoxLayout>

TranslationDisplayWidget::TranslationDisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_MarkerSizeOrPosition(new QLabel("0")),
	m_XLabel(new QLabel("N/A")),
	m_YLabel(new QLabel("N/A")),
	m_ZLabel(new QLabel("N/A"))
{
	initUi();
}

TranslationDisplayWidget::~TranslationDisplayWidget()
{
}

void TranslationDisplayWidget::initUi()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *layout_1 = new QHBoxLayout;
	layout_1->addWidget(new QLabel(QString::fromLocal8Bit("Æ½ÒÆÇòÊý/×ø±ê:")));
	layout_1->addWidget(m_MarkerSizeOrPosition);
	mainLayout->addLayout(layout_1);
	QHBoxLayout *layout_2 = new QHBoxLayout;
	layout_2->addWidget(new QLabel(QString::fromLocal8Bit("XÖáÆ½ÒÆ¾àÀë:")));
	layout_2->addWidget(m_XLabel);
	mainLayout->addLayout(layout_2);
	QHBoxLayout *layout_3 = new QHBoxLayout;
	layout_3->addWidget(new QLabel(QString::fromLocal8Bit("YÖáÆ½ÒÆ¾àÀë:")));
	layout_3->addWidget(m_YLabel);
	mainLayout->addLayout(layout_3);
	QHBoxLayout *layout_4 = new QHBoxLayout;
	layout_4->addWidget(new QLabel(QString::fromLocal8Bit("ZÖáÆ½ÒÆ¾àÀë:")));
	layout_4->addWidget(m_ZLabel);
	mainLayout->addLayout(layout_4);
}

void TranslationDisplayWidget::setMarkerSize(int size)
{
	m_MarkerSizeOrPosition->setText(QString::number(size));
}

void TranslationDisplayWidget::setMarkerPosition(MarkerPointType &point)
{
	QString str = QString::number(point[0], 'f', 1) + "," + QString::number(point[1], 'f', 1) + ","
		+ QString::number(point[2], 'f', 1);
	m_MarkerSizeOrPosition->setText(str);
}

void TranslationDisplayWidget::setTranslateResult(double bias[3])
{
	m_XLabel->setText(QString::number(bias[0], 'f', 2));
	m_YLabel->setText(QString::number(bias[1], 'f', 2));
	m_ZLabel->setText(QString::number(bias[2], 'f', 2));
}