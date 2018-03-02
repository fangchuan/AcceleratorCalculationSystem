#include "horizontalregisterdisplaywidget.h"

#include <QVBoxLayout>
#include <QLabel>

HorizontalRegisterDisplayWidget::HorizontalRegisterDisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_MarkerSizeLabel(new QLabel("0")),
	m_StatusLabel(new QLabel("N/A"))
{
	initUi();
}

HorizontalRegisterDisplayWidget::~HorizontalRegisterDisplayWidget()
{
}

void HorizontalRegisterDisplayWidget::initUi()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *layout_1 = new QHBoxLayout;
	layout_1->addWidget(new QLabel(QString::fromLocal8Bit("���ƴ�����:")));
	layout_1->addWidget(m_MarkerSizeLabel);
	mainLayout->addLayout(layout_1);
	QHBoxLayout *layout_2 = new QHBoxLayout;
	layout_2->addWidget(new QLabel(QString::fromLocal8Bit("���ƴ�ˮƽ״̬:")));
	layout_2->addWidget(m_StatusLabel);
	mainLayout->addLayout(layout_2);
}

void HorizontalRegisterDisplayWidget::setMarkerSize(int number)
{
	m_MarkerSizeLabel->setText(QString::number(number));
	m_StatusLabel->setText(QString::fromLocal8Bit(number >= 3 ? "��Ч": "��Ч"));
}

void HorizontalRegisterDisplayWidget::horizontalRegisterRecorded()
{
	m_StatusLabel->setText(QString::fromLocal8Bit("�Ѽ�¼"));
}

void HorizontalRegisterDisplayWidget::reset()
{
	m_MarkerSizeLabel->setText(QString::number(0));
	m_StatusLabel->setText("N/A");
}