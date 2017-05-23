#include "ISOCenterDisplayWidget.h"

#include <QLabel>
#include <QVBoxLayout>

ISOCenterDisplayWidget::ISOCenterDisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_XLabel(new QLabel("等中心")),
	m_YLabel(new QLabel("等中心")),
	m_ZLabel(new QLabel("等中心"))
{
	initUi();
}

ISOCenterDisplayWidget::~ISOCenterDisplayWidget()
{
}

void ISOCenterDisplayWidget::initUi()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *layout_1 = new QHBoxLayout;
	layout_1->addWidget(new QLabel("X:"));
	layout_1->addWidget(m_XLabel);
	mainLayout->addLayout(layout_1);
	QHBoxLayout *layout_2 = new QHBoxLayout;
	layout_2->addWidget(new QLabel("Y:"));
	layout_2->addWidget(m_YLabel);
	mainLayout->addLayout(layout_2);
	QHBoxLayout *layout_3 = new QHBoxLayout;
	layout_3->addWidget(new QLabel("Z:"));
	layout_3->addWidget(m_ZLabel);
	mainLayout->addLayout(layout_3);
}

void ISOCenterDisplayWidget::setRegisteredPosition(double point[3])
{
	m_XLabel->setText(QString::number(point[0], 'f', 2));
	m_YLabel->setText(QString::number(point[1], 'f', 2));
	m_ZLabel->setText(QString::number(point[2], 'f', 2));
}