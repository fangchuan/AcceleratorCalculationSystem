#include "RotationDisplayWidget.h"
#include "Circle.h"

#include <QLabel>
#include <QVBoxLayout>

RotationDisplayWidget::RotationDisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_MarkerSizeOrPosition(new QLabel("0")),
	m_ParallelOrPerpendicular(new QLabel("N/A")),
	m_CurrentAngle(new QLabel("N/A"))
{
	initUi();
}

RotationDisplayWidget::~RotationDisplayWidget()
{
}

void RotationDisplayWidget::initUi()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *layout_1 = new QHBoxLayout;
	layout_1->addWidget(new QLabel(QString::fromLocal8Bit("旋转球数/坐标:")));
	layout_1->addWidget(m_MarkerSizeOrPosition);
	mainLayout->addLayout(layout_1);
	QHBoxLayout *layout_2 = new QHBoxLayout;
	layout_2->addWidget(new QLabel(QString::fromLocal8Bit("平行/垂直:")));
	layout_2->addWidget(m_ParallelOrPerpendicular);
	mainLayout->addLayout(layout_2);
	QHBoxLayout *layout_3 = new QHBoxLayout;
	layout_3->addWidget(new QLabel(QString::fromLocal8Bit("旋转角度:")));
	layout_3->addWidget(m_CurrentAngle);
	mainLayout->addLayout(layout_3);
}

void RotationDisplayWidget::setMarkerSize(int size)
{
	m_MarkerSizeOrPosition->setText(QString::number(size));
}

void RotationDisplayWidget::setMarkerPosition(MarkerPointType &point)
{
	QString str = QString::number(point[0], 'f', 1) + "," + QString::number(point[1], 'f', 1) + ","
		+ QString::number(point[2], 'f', 1);
	m_MarkerSizeOrPosition->setText(str);
}

void RotationDisplayWidget::setCurrentAngle(double angle)
{
	m_CurrentAngle->setText(QString::number(angle, 'f', 2));
}

void RotationDisplayWidget::setCircleResult(Circle *circle)
{
	setCurrentAngle(circle->Angle);
	m_ParallelOrPerpendicular->setText(QString::fromLocal8Bit(circle->IsParallelOrPerpendicular ? "是" : "否"));
}

void RotationDisplayWidget::reset()
{
	m_MarkerSizeOrPosition->setText("0");
	m_ParallelOrPerpendicular->setText("N/A");
	m_CurrentAngle->setText("N/A");
}