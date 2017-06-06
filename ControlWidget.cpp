#include "ControlWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>


ControlWidget::ControlWidget(QWidget *parent)
	: QWidget(parent)
{
	initUi();
	buildConnections();
}

ControlWidget::~ControlWidget()
{

}

void ControlWidget::initUi()
{

	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);
	
	m_HorizontalRegisterButton = new QPushButton(QString::fromLocal8Bit("注册水平面"));
	gridLayout->addWidget(m_HorizontalRegisterButton, 0, 0);

	m_GantryButton = new QPushButton(QString::fromLocal8Bit("校准机架"));
	gridLayout->addWidget(m_GantryButton, 0, 1);

	m_CollimatorButton = new QPushButton(QString::fromLocal8Bit("校准准直器"));
	gridLayout->addWidget(m_CollimatorButton, 1, 0);

	m_LaserButton = new QPushButton(QString::fromLocal8Bit("检测激光等中心"));
	gridLayout->addWidget(m_LaserButton, 1, 1);

	m_BedButton = new QPushButton(QString::fromLocal8Bit("校准治疗床"));
	gridLayout->addWidget(m_BedButton, 2, 0);

	m_BedGroup = new QButtonGroup;
	QVBoxLayout *bedLayout = new QVBoxLayout;
	QRadioButton *rotationButton = new QRadioButton(QString::fromLocal8Bit("旋转治疗床"));
	m_BedGroup->addButton(rotationButton, 0);
	rotationButton->setChecked(true);
	bedLayout->addWidget(rotationButton);
	QRadioButton *translateButton = new QRadioButton(QString::fromLocal8Bit("平移治疗床"));
	m_BedGroup->addButton(translateButton, 1);
	bedLayout->addWidget(translateButton);
	gridLayout->addLayout(bedLayout, 2, 1);

	m_ResetButton = new  QPushButton(QString::fromLocal8Bit("加速器复位"));
	gridLayout->addWidget(m_ResetButton, 3, 0);

	m_ReportButton = new QPushButton(QString::fromLocal8Bit("生成报告"));
	gridLayout->addWidget(m_ReportButton, 3, 1);


	setButtonStyle();
}

void ControlWidget::buildConnections()
{
	connect(m_HorizontalRegisterButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_GantryButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_CollimatorButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_BedButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_LaserButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_BedGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ControlWidget::handleButtonGroupClick);
	connect(m_ResetButton, &QPushButton::clicked, this, &ControlWidget::resetAll);
	connect(m_ResetButton, &QPushButton::clicked, this, &ControlWidget::resetRequest);
	connect(m_ReportButton, &QPushButton::clicked, this, &ControlWidget::reportRequest);
}

void ControlWidget::handleButtonClick()
{
	QPushButton *button = qobject_cast<QPushButton*>(sender());
	QString buttonText = button->text();
	if (button == m_HorizontalRegisterButton) {
		if (buttonText == QString::fromLocal8Bit("注册水平面")) {
			emit switchToHorizontalRegister();
		}
		else {
			emit recordingHorizontalRegister();
		}
	}
	else if (button == m_GantryButton) {
		if (buttonText == QString::fromLocal8Bit("校准机架")) {
			emit switchToGantry();
		}
		else {
			emit recordingGantry();
		}
	}
	else if (button == m_CollimatorButton) {
		if (buttonText == QString::fromLocal8Bit("校准准直器")) {
			emit switchToCollimator();
		}
		else {
			emit recordingCollimator();
		}
	}
	else if (button == m_BedButton) {
		if (buttonText == QString::fromLocal8Bit("校准治疗床")) {
			emit switchToBed(m_BedGroup->checkedId());
		}
		else {
			emit recordingBed(m_BedGroup->checkedId());
		}
	}
	else if (button == m_LaserButton) {
		if (buttonText == QString::fromLocal8Bit("检测激光等中心")) {
			emit switchToISOCenter();
		}
		else {
			emit recordingISOCenter();
		}
	}
}

void ControlWidget::handleButtonGroupClick(int id)
{
	if (m_BedButton->text() == QString::fromLocal8Bit("移动治疗床")) {
		emit switchToBed(id);
	}
}

void ControlWidget::doSwitchToHorizontalRegister()
{
	m_HorizontalRegisterButton->setText(QString::fromLocal8Bit("记录水平面"));
	resetGantry();
	resetCollimator();
	resetBed();
	resetISOCenter();
}

void ControlWidget::doSwitchToGantry()
{
	resetHorizontalRegister();
	m_GantryButton->setText(QString::fromLocal8Bit("旋转机架"));
	resetCollimator();
	resetBed();
	resetISOCenter();
}

void ControlWidget::doSwitchToCollimator()
{
	resetHorizontalRegister();
	resetGantry();
	m_CollimatorButton->setText(QString::fromLocal8Bit("旋转准直器"));
	resetBed();
	resetISOCenter();
}

void ControlWidget::doSwitchToBed()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	m_BedButton->setText(QString::fromLocal8Bit("移动治疗床"));
	resetISOCenter();
}

void ControlWidget::doSwitchToISOCenter()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	m_LaserButton->setText(QString::fromLocal8Bit("记录激光等中心"));
}

void ControlWidget::resetHorizontalRegister()
{
	m_HorizontalRegisterButton->setText(QString::fromLocal8Bit("注册水平面"));
}

void ControlWidget::resetGantry()
{
	m_GantryButton->setText(QString::fromLocal8Bit("校准机架"));
}

void ControlWidget::resetCollimator()
{
	m_CollimatorButton->setText(QString::fromLocal8Bit("校准准直器"));
}

void ControlWidget::resetBed()
{
	m_BedButton->setText(QString::fromLocal8Bit("校准治疗床"));
}

void ControlWidget::resetISOCenter()
{
	m_LaserButton->setText(QString::fromLocal8Bit("检测激光等中心"));
}

void ControlWidget::resetAll()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	resetISOCenter();
}
void ControlWidget::setButtonStyle()
{
	m_HorizontalRegisterButton->setStyleSheet("QPushButton{background-color:rgb(0,153,171);"
												"min-height:80;"
												"border:1px solid white;"
												"border-radius:8px}"
											  "QPushButton:pressed{background-color:rgb(0,180,180);"
												"border-style: inset;}");

	m_GantryButton->setStyleSheet("QPushButton{background-color:rgb(217,82,44);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								  "QPushButton:pressed{background-color:rgb(217,100,60);"
										"border-style: inset;}");

	m_CollimatorButton->setStyleSheet("QPushButton{background-color:rgb(164,0,172);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
									  "QPushButton:pressed{background-color:rgb(180,0,180);"
										"border-style: inset;}");

	m_BedButton->setStyleSheet("QPushButton{background-color:rgb(87,54,177);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								"QPushButton:pressed{background-color:rgb(110,54,180);"
										"border-style: inset;}");

	m_LaserButton->setStyleSheet("QPushButton{background-color:rgb(45,137,239);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(65,137,250);"
										"border-style: inset;}");

	m_ResetButton->setStyleSheet("QPushButton{background-color:rgb(10,89,193);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(40,90,200);"
										"border-style: inset;}");


	m_ReportButton->setStyleSheet("QPushButton{background-color:rgb(187,29,72);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								  "QPushButton:pressed{background-color:rgb(200,29,90);"
										"border-style: inset;}");
}