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
	QLabel* lableNumber = new QLabel(QObject::tr("Numbers:"));
	lableNumber->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
								"font-size:16px;"
								"border:1px solid white;"
								"border-radius:8px;"
								"border-style: inset;}");
	layout_1->addWidget(lableNumber);//
	m_MarkerSizeLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
									"font-size:16px;"
									"border:1px solid white;"
									"border-radius:8px;"
									"border-style: inset;}");
	layout_1->addWidget(m_MarkerSizeLabel);
	layout_1->setStretchFactor(lableNumber, 3);
	layout_1->setStretchFactor(m_MarkerSizeLabel, 7);
	mainLayout->addLayout(layout_1);
	QHBoxLayout *layout_2 = new QHBoxLayout;
	QLabel* lableLevel = new QLabel(QObject::tr("Level statu:"));
	lableLevel->setStyleSheet("QFrame, QLabel, QToolTip{ font - family:'Microsoft YaHei'; "
								"font-size:16px;"
								"border:1px solid white;"
								"border-radius:8px;"
								"border-style: inset;}");
	layout_2->addWidget(lableLevel);//
	m_StatusLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
									"font-size:16px;"
									"border:1px solid white;"
									"border-radius:8px;"
									"border-style: inset;}");
	layout_2->addWidget(m_StatusLabel);
	layout_2->setStretchFactor(lableLevel, 3);
	layout_2->setStretchFactor(m_StatusLabel, 7);
	mainLayout->addLayout(layout_2);
}

void HorizontalRegisterDisplayWidget::setMarkerSize(int number)
{
	m_MarkerSizeLabel->setText(QString::number(number));
	if(number >= 3)
		m_StatusLabel->setText(tr("Valid"));
	else
		m_StatusLabel->setText(tr("Invalid"));
}

void HorizontalRegisterDisplayWidget::horizontalRegisterRecorded()
{
	m_StatusLabel->setText(QObject::tr("Recorded"));//
}

void HorizontalRegisterDisplayWidget::reset()
{
	m_MarkerSizeLabel->setText(QString::number(0));
	m_StatusLabel->setText("N/A");
}