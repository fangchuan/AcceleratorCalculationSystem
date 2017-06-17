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
	
	m_BedGroup = new QButtonGroup;
	QRadioButton *rotationButton = new QRadioButton(QString::fromLocal8Bit("��ת���ƴ�"));
	m_BedGroup->addButton(rotationButton, 0);
	rotationButton->setChecked(true);
	gridLayout->addWidget(rotationButton, 0, 0);
	QRadioButton *translateButton = new QRadioButton(QString::fromLocal8Bit("ƽ�����ƴ�"));
	m_BedGroup->addButton(translateButton, 1);
	gridLayout->addWidget(translateButton, 0, 1);

	m_HorizontalRegisterButton = new QPushButton(QString::fromLocal8Bit("ע��ˮƽ��"));
	gridLayout->addWidget(m_HorizontalRegisterButton, 1, 0);

	m_GantryButton = new QPushButton(QString::fromLocal8Bit("У׼����"));
	gridLayout->addWidget(m_GantryButton, 1, 1);

	m_CollimatorButton = new QPushButton(QString::fromLocal8Bit("У׼׼ֱ��"));
	gridLayout->addWidget(m_CollimatorButton, 2, 0);

	m_LaserButton = new QPushButton(QString::fromLocal8Bit("��⼤�������"));
	gridLayout->addWidget(m_LaserButton, 2, 1);

	m_BedButton = new QPushButton(QString::fromLocal8Bit("У׼���ƴ�"));
	gridLayout->addWidget(m_BedButton, 3, 0);

	m_LightButton = new QPushButton(QString::fromLocal8Bit("���ģ���Ұ����"));
	gridLayout->addWidget(m_LightButton, 3, 1);

	m_ResetButton = new  QPushButton(QString::fromLocal8Bit("��������λ"));
	gridLayout->addWidget(m_ResetButton, 4, 0);

	m_ReportButton = new QPushButton(QString::fromLocal8Bit("���ɱ���"));
	gridLayout->addWidget(m_ReportButton, 4, 1);

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

	connect(m_ResetButton, &QPushButton::clicked, this, &ControlWidget::resetRequest);
	connect(m_ReportButton, &QPushButton::clicked, this, &ControlWidget::reportRequest);
}

void ControlWidget::handleButtonClick()
{
	QPushButton *button = qobject_cast<QPushButton*>(sender());
	QString buttonText = button->text();
	if (button == m_HorizontalRegisterButton) {
		if (buttonText == QString::fromLocal8Bit("ע��ˮƽ��")) {
			emit switchToHorizontalRegister();
		}else {
			emit recordingHorizontalRegister();
		}
	}
	else if (button == m_GantryButton) {
		if (buttonText == QString::fromLocal8Bit("У׼����")) {
			emit switchToGantry();
		}else {
			emit recordingGantry();
		}
	}
	else if (button == m_CollimatorButton) {
		if (buttonText == QString::fromLocal8Bit("У׼׼ֱ��")) {
			emit switchToCollimator();
		}else{
			emit recordingCollimator();
		}
	}
	else if (button == m_BedButton) {
		if (buttonText == QString::fromLocal8Bit("У׼���ƴ�")) {
			emit switchToBed(m_BedGroup->checkedId());
		}else{
			emit recordingBed(m_BedGroup->checkedId());
		}
	}
	else if (button == m_LaserButton) {
		if (buttonText == QString::fromLocal8Bit("��⼤�������")) {
			emit switchToLaserISO();
		}else{
			emit recordingLaserISO();
		}
	}
	else if (button == m_LightButton){
		if (buttonText == QString::fromLocal8Bit("���ģ���Ұ����")){
			emit switchToLightCenter();
		}else{
			emit recordingLightCenter();
		}
	}

}

void ControlWidget::handleButtonGroupClick(int id)
{
	if (m_BedButton->text() == QString::fromLocal8Bit("�ƶ����ƴ�")) {
		emit switchToBed(id);
	}
}

void ControlWidget::doSwitchToHorizontalRegister()
{
	m_HorizontalRegisterButton->setText(QString::fromLocal8Bit("��¼ˮƽ��"));
	resetGantry();
	resetCollimator();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToGantry()
{
	resetHorizontalRegister();
	m_GantryButton->setText(QString::fromLocal8Bit("��ת����"));
	resetCollimator();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToCollimator()
{
	resetHorizontalRegister();
	resetGantry();
	m_CollimatorButton->setText(QString::fromLocal8Bit("��ת׼ֱ��"));
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToBed()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	m_BedButton->setText(QString::fromLocal8Bit("�ƶ����ƴ�"));
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToLaserISO()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	resetLightCenter();
	m_LaserButton->setText(QString::fromLocal8Bit("��¼���������"));
}

void ControlWidget::doSwitchToLightCenter()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	resetLaserISO();
	m_LightButton->setText(QString::fromLocal8Bit("��¼ģ���Ұ����"));
}
void ControlWidget::resetHorizontalRegister()
{
	m_HorizontalRegisterButton->setText(QString::fromLocal8Bit("ע��ˮƽ��"));
}

void ControlWidget::resetGantry()
{
	m_GantryButton->setText(QString::fromLocal8Bit("У׼����"));
}

void ControlWidget::resetCollimator()
{
	m_CollimatorButton->setText(QString::fromLocal8Bit("У׼׼ֱ��"));
}

void ControlWidget::resetBed()
{
	
	m_BedButton->setText(QString::fromLocal8Bit("У׼���ƴ�"));
}

void ControlWidget::resetLaserISO()
{
	m_LaserButton->setText(QString::fromLocal8Bit("��⼤�������"));
}

void ControlWidget::resetLightCenter()
{
	m_LightButton->setText(QString::fromLocal8Bit("���ģ���Ұ����"));
}
void ControlWidget::reset()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	resetLaserISO();
	resetLightCenter();
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