#include "isocenterdisplaywidget.h"

#include <QLabel>
#include <QVBoxLayout>

ISOCenterDisplayWidget::ISOCenterDisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_XLabel(new QLabel("XXX.XX")),
	m_YLabel(new QLabel("XXX.XX")),
	m_ZLabel(new QLabel("XXX.XX"))
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
	QLabel* labelX = new QLabel("X:");
	labelX->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
							"font-size:16px;"
							"border:1px solid white;"
							"border-radius:8px;"
							"border-style: inset;}");
	layout_1->addWidget(labelX);
	m_XLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
							"font-size:16px;"
							"border:1px solid white;"
							"border-radius:8px;"
							"border-style: inset;}");
	layout_1->addWidget(m_XLabel);
	mainLayout->addLayout(layout_1);
	QHBoxLayout *layout_2 = new QHBoxLayout;
	QLabel* labelY = new QLabel("Y:");
	labelY->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
							"font-size:16px;"
							"border:1px solid white;"
							"border-radius:8px;"
							"border-style: inset;}");
	layout_2->addWidget(labelY);
	m_YLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
							"font-size:16px;"
							"border:1px solid white;"
							"border-radius:8px;"
							"border-style: inset;}");
	layout_2->addWidget(m_YLabel);
	mainLayout->addLayout(layout_2);
	QHBoxLayout *layout_3 = new QHBoxLayout;
	QLabel* labelZ = new QLabel("Z:");
	labelZ->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
							"font-size:16px;"
							"border:1px solid white;"
							"border-radius:8px;"
							"border-style: inset;}");
	layout_3->addWidget(labelZ);
	m_ZLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
							"font-size:16px;"
							"border:1px solid white;"
							"border-radius:8px;"
							"border-style: inset;}");
	layout_3->addWidget(m_ZLabel);
	mainLayout->addLayout(layout_3);
}

void ISOCenterDisplayWidget::setRegisteredPosition(double point[3])
{
	m_XLabel->setText(QString::number(point[0], 'f', 2));
	m_YLabel->setText(QString::number(point[1], 'f', 2));
	m_ZLabel->setText(QString::number(point[2], 'f', 2));
}

void ISOCenterDisplayWidget::reset()
{
	m_XLabel->setText(QString::number(0, 'f', 2));
	m_YLabel->setText(QString::number(0, 'f', 2));
	m_ZLabel->setText(QString::number(0, 'f', 2));
}