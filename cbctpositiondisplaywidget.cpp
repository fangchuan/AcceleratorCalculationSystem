#include "cbctpositiondisplaywidget.h"
#include "cbctpositionhandler.h"
#include <QLabel>
#include <QVBoxLayout>

CbctPositionDisplayWidget::CbctPositionDisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_point1Label(new QLabel("0, 0, 0")),
	m_point2Label(new QLabel("0, 0, 0")),
	m_point3Label(new QLabel("0, 0, 0")),
	m_normalLabel(new QLabel("0, 0, 0")),
	m_angleLabel(new QLabel("N/A"))
{
	initUi();
}

CbctPositionDisplayWidget::~CbctPositionDisplayWidget()
{
}

void CbctPositionDisplayWidget::setPoint1Position(double point[3])
{
	QString str = QString::number(point[0], 'f', 2) + ", " + QString::number(point[1], 'f', 2) + ", "
		+ QString::number(point[2], 'f', 2);
	m_point1Label->setText(str);
}

void CbctPositionDisplayWidget::setPoint2Position(double point[3])
{
	QString str = QString::number(point[0], 'f', 2) + ", " + QString::number(point[1], 'f', 2) + ", "
		+ QString::number(point[2], 'f', 2);
	m_point2Label->setText(str);
}

void CbctPositionDisplayWidget::setPoint3Position(double point[3])
{
	QString str = QString::number(point[0], 'f', 2) + ", " + QString::number(point[1], 'f', 2) + ", "
		+ QString::number(point[2], 'f', 2);
	m_point3Label->setText(str);
}

void CbctPositionDisplayWidget::setPlaneResult(Plane_T & plane)
{
	QString str = QString::number(plane.normal[0], 'f', 2) + ", " + QString::number(plane.normal[1], 'f', 2) + ", "
		+ QString::number(plane.normal[2], 'f', 2);
	m_normalLabel->setText(str);

	m_angleLabel->setText(QString::number(plane.angleBetweenP2H, 'f', 2));
}

void CbctPositionDisplayWidget::initUi()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *layout_1 = new QHBoxLayout;
	QLabel* labelOne = new QLabel("Point 1:");
	labelOne->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_1->addWidget(labelOne);
	m_point1Label->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_1->addWidget(m_point1Label);
	mainLayout->addLayout(layout_1);

	QHBoxLayout *layout_2 = new QHBoxLayout;
	QLabel* labelSecond = new QLabel("Point 2:");
	labelSecond->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_2->addWidget(labelSecond);
	m_point2Label->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_2->addWidget(m_point2Label);
	mainLayout->addLayout(layout_2);

	QHBoxLayout *layout_3 = new QHBoxLayout;
	QLabel* labelThree = new QLabel("Point 3:");
	labelThree->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_3->addWidget(labelThree);
	m_point3Label->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_3->addWidget(m_point3Label);
	mainLayout->addLayout(layout_3);

	QHBoxLayout* layout_4 = new QHBoxLayout();
	QLabel* labelNormal = new QLabel(QObject::tr("Plane Normal:"));
	labelNormal->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_4->addWidget(labelNormal);
	m_normalLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_4->addWidget(m_normalLabel);
	mainLayout->addLayout(layout_4);

	QHBoxLayout* layout_5 = new QHBoxLayout();
	QLabel* labelAngle = new QLabel(QObject::tr("Plane Angle:"));
	labelAngle->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_5->addWidget(labelAngle);
	m_angleLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_5->addWidget(m_angleLabel);
	mainLayout->addLayout(layout_5);
}


void CbctPositionDisplayWidget::reset()
{
	m_point1Label->setText("0, 0, 0");
	m_point2Label->setText("0, 0, 0");
	m_point3Label->setText("0, 0, 0");
	m_normalLabel->setText("0, 0, 0");
	m_angleLabel->setText("N/A");
}
