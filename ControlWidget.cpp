#include "controlwidget.h"

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
	
	m_BedGroup = new QButtonGroup;
	QRadioButton *rotationButton = new QRadioButton(QString::fromLocal8Bit("空载旋转治疗床"));
	m_BedGroup->addButton(rotationButton, 0);
	rotationButton->setChecked(true);
	gridLayout->addWidget(rotationButton, 0, 0);
	QRadioButton *translateButton = new QRadioButton(QString::fromLocal8Bit("空载平移治疗床"));
	m_BedGroup->addButton(translateButton, 1);
	gridLayout->addWidget(translateButton, 0, 1);
	QRadioButton *emptyButton = new QRadioButton(QString::fromLocal8Bit("负载旋转治疗床"));
	m_BedGroup->addButton(emptyButton, 2);
	gridLayout->addWidget(emptyButton, 1, 0);
	QRadioButton *payloadButton = new QRadioButton(QString::fromLocal8Bit("负载平移治疗床"));
	m_BedGroup->addButton(payloadButton, 3);
	gridLayout->addWidget(payloadButton, 1, 1);

	m_HorizontalRegisterButton = new QPushButton(QString::fromLocal8Bit("注册水平面"));
	gridLayout->addWidget(m_HorizontalRegisterButton, 2, 0);

	m_GantryButton = new QPushButton(QString::fromLocal8Bit("校准机架"));
	gridLayout->addWidget(m_GantryButton, 2, 1);

	m_CollimatorButton = new QPushButton(QString::fromLocal8Bit("校准准直器"));
	gridLayout->addWidget(m_CollimatorButton, 3, 0);

	m_BedButton = new QPushButton(QString::fromLocal8Bit("校准治疗床"));
	gridLayout->addWidget(m_BedButton, 3, 1);

	m_CbctButton = new QPushButton(QString::fromLocal8Bit("校准CBCT"));
	gridLayout->addWidget(m_CbctButton, 4, 0);

	m_PayloadBedButton = new QPushButton(QString::fromLocal8Bit("负载校准治疗床"));
	gridLayout->addWidget(m_PayloadBedButton, 4, 1);

	m_LaserButton = new QPushButton(QString::fromLocal8Bit("检测激光等中心"));
	gridLayout->addWidget(m_LaserButton, 5, 0);

	m_LightButton = new QPushButton(QString::fromLocal8Bit("检测模拟光野中心"));
	gridLayout->addWidget(m_LightButton, 5, 1);

	m_ResetButton = new  QPushButton(QString::fromLocal8Bit("加速器复位"));
	gridLayout->addWidget(m_ResetButton, 6, 0);

	m_ReportButton = new QPushButton(QString::fromLocal8Bit("生成报告"));
	gridLayout->addWidget(m_ReportButton, 6, 1);

	setButtonStyle();
}

void ControlWidget::buildConnections()
{
	connect(m_HorizontalRegisterButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_GantryButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_CollimatorButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_BedButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_LaserButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_LightButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_BedGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ControlWidget::handleButtonGroupClick);
	connect(m_CbctButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_PayloadBedButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
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
		}else {
			emit recordingHorizontalRegister();
		}
	}
	else if (button == m_GantryButton) {
		if (buttonText == QString::fromLocal8Bit("校准机架")) {
			emit switchToGantry();
		}else {
			emit recordingGantry();
		}
	}
	else if (button == m_CollimatorButton) {
		if (buttonText == QString::fromLocal8Bit("校准准直器")) {
			emit switchToCollimator();
		}else{
			emit recordingCollimator();
		}
	}
	else if (button == m_BedButton) {
		if (buttonText == QString::fromLocal8Bit("校准治疗床")) {
			emit switchToBed(m_BedGroup->checkedId());
		}else{
			emit recordingBed(m_BedGroup->checkedId());
		}
	}
	else if (button == m_CbctButton) {
		if (buttonText == QString::fromLocal8Bit("校准CBCT")) {
			emit switchToCbct();
		}
		else {
			emit recordingCbct();
		}
	}
	else if (button == m_PayloadBedButton) {
		if (buttonText == QString::fromLocal8Bit("负载校准治疗床")) {
				emit switchToBed(m_BedGroup->checkedId());
			}
			else {
				emit recordingBed(m_BedGroup->checkedId());
			}
	}
	else if (button == m_LaserButton) {
		if (buttonText == QString::fromLocal8Bit("检测激光等中心")) {
			emit switchToLaserISO();
		}else{
			emit recordingLaserISO();
		}
	}
	else if (button == m_LightButton){
		if (buttonText == QString::fromLocal8Bit("检测模拟光野中心")){
			emit switchToLightCenter();
		}else{
			emit recordingLightCenter();
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
	resetCbct();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToGantry()
{
	resetHorizontalRegister();
	m_GantryButton->setText(QString::fromLocal8Bit("旋转机架"));
	resetCollimator();
	resetCbct();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToCollimator()
{
	resetHorizontalRegister();
	resetGantry();
	m_CollimatorButton->setText(QString::fromLocal8Bit("旋转准直器"));
	resetCbct();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToCbct()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	m_CbctButton->setText(QString::fromLocal8Bit("旋转CBCT"));
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToBed()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetCbct();
	m_BedButton->setText(QString::fromLocal8Bit("移动治疗床"));
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToLaserISO()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetCbct();
	resetBed();
	resetLightCenter();
	m_LaserButton->setText(QString::fromLocal8Bit("记录激光等中心"));
}

void ControlWidget::doSwitchToLightCenter()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetCbct();
	resetBed();
	resetLaserISO();
	m_LightButton->setText(QString::fromLocal8Bit("记录模拟光野中心"));
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

void ControlWidget::resetCbct()
{
	m_CbctButton->setText(QString::fromLocal8Bit("校准CBCT"));
}

void ControlWidget::resetLaserISO()
{
	m_LaserButton->setText(QString::fromLocal8Bit("检测激光等中心"));
}

void ControlWidget::resetLightCenter()
{
	m_LightButton->setText(QString::fromLocal8Bit("检测模拟光野中心"));
}
void ControlWidget::reset()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	resetCbct();
	resetLaserISO();
	resetLightCenter();
	resetCbct();
}

void ControlWidget::setButtonStyle()
{
	m_HorizontalRegisterButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
												"font-size:18px;"
												"background-color:rgb(0,71,157);"
												"min-height:80;"
												"border:1px solid white;"
												"border-radius:8px}"
											  "QPushButton:pressed{background-color:rgb(92,156,233);"
												"border-style: inset;}");

	m_GantryButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								  "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_CollimatorButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
									  "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_BedButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								"QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_CbctButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
										"QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_PayloadBedButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
										"QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_LaserButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_LightButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(92,156,233);"
								 "border-style: inset;}");

	m_ResetButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");


	m_ReportButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								  "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");
}
