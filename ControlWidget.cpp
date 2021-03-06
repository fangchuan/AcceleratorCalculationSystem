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
	QRadioButton *rotationButton = new QRadioButton(QObject::tr("Rotating empty treatment bed"));//
	m_BedGroup->addButton(rotationButton, 0);
	rotationButton->setChecked(true);
	gridLayout->addWidget(rotationButton, 0, 0);
	QRadioButton *translateButton = new QRadioButton(QObject::tr("Translating empty treatment bed"));//
	m_BedGroup->addButton(translateButton, 1);
	gridLayout->addWidget(translateButton, 0, 1);
	QRadioButton *emptyButton = new QRadioButton(QObject::tr("Rotating payload treatment bed"));//
	m_BedGroup->addButton(emptyButton, 2);
	gridLayout->addWidget(emptyButton, 1, 0);
	QRadioButton *payloadButton = new QRadioButton(QObject::tr("Translating payload treatment bed"));//
	m_BedGroup->addButton(payloadButton, 3);
	gridLayout->addWidget(payloadButton, 1, 1);

	m_HorizontalRegisterButton = new QPushButton(QObject::tr("Register Horizontal plane"));//
	gridLayout->addWidget(m_HorizontalRegisterButton, 2, 0);

	m_GantryButton = new QPushButton(QObject::tr("Calibrate Gantry"));//
	gridLayout->addWidget(m_GantryButton, 2, 1);

	m_CollimatorButton = new QPushButton(QObject::tr("Calibrate Collimator"));//
	gridLayout->addWidget(m_CollimatorButton, 3, 0);

	m_BedButton = new QPushButton(QObject::tr("Calibrate treatment bed"));//
	gridLayout->addWidget(m_BedButton, 3, 1);

	m_EpidButton = new QPushButton(QObject::tr("Calibrate EPID position"));//
	gridLayout->addWidget(m_EpidButton, 4, 0);

	m_CbctPositionButton = new QPushButton(QObject::tr("Calibrate CBCT position"));//
	gridLayout->addWidget(m_CbctPositionButton, 4, 1);

	m_LaserButton = new QPushButton(QObject::tr("Detecte Laser ISOcenter"));//
	gridLayout->addWidget(m_LaserButton, 5, 0);

	m_LightButton = new QPushButton(QObject::tr("Detecte Light ISOcenter"));//
	gridLayout->addWidget(m_LightButton, 5, 1);

	m_ResetButton = new  QPushButton(QObject::tr("Reset"));//
	gridLayout->addWidget(m_ResetButton, 6, 0);

	m_ReportButton = new QPushButton(QObject::tr("Generate Report"));//
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
	connect(m_EpidButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_CbctPositionButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_ResetButton, &QPushButton::clicked, this, &ControlWidget::resetRequest);
	connect(m_ReportButton, &QPushButton::clicked, this, &ControlWidget::reportRequest);
}

void ControlWidget::handleButtonClick()
{
	QPushButton *button = qobject_cast<QPushButton*>(sender());
	QString buttonText = button->text();
	if (button == m_HorizontalRegisterButton) {
		if (buttonText == QObject::tr("Register Horizontal plane")) { 
			emit switchToHorizontalRegister();
		}else {
			emit recordingHorizontalRegister();
		}
	}
	else if (button == m_GantryButton) {
		if (buttonText == QObject::tr("Calibrate Gantry")) {
			emit switchToGantry();
		}else {
			emit recordingGantry();
		}
	}
	else if (button == m_CollimatorButton) {
		if (buttonText == QObject::tr("Calibrate Collimator")) {
			emit switchToCollimator();
		}else{
			emit recordingCollimator();
		}
	}
	else if (button == m_BedButton) {
		if (buttonText == QObject::tr("Calibrate treatment bed")) {
			emit switchToBed(m_BedGroup->checkedId());
		}else{
			emit recordingBed(m_BedGroup->checkedId());
		}
	}
	else if (button == m_EpidButton) {
		if (buttonText == QObject::tr("Calibrate EPID position")) {
			emit switchToEpidPosition();
		}
		else {
			emit recordingEpidPosition();
		}
	}
	else if (button == m_CbctPositionButton) {
		if (buttonText == QObject::tr("Calibrate CBCT position")) {
			emit switchToCbctPosition();
			}
			else {
				emit recordingCbctPosition();
			}
	}
	else if (button == m_LaserButton) {
		if (buttonText == QObject::tr("Detecte Laser ISOcenter")) {
			emit switchToLaserISO();
		}else{
			emit recordingLaserISO();
		}
	}
	else if (button == m_LightButton){
		if (buttonText == QObject::tr("Detecte Light ISOcenter")){
			emit switchToLightCenter();
		}else{
			emit recordingLightCenter();
		}
	}

}

void ControlWidget::handleButtonGroupClick(int id)
{
	if (m_BedButton->text() == QObject::tr("Move treatment bed")) {  //
		emit switchToBed(id);
	}
}

void ControlWidget::doSwitchToHorizontalRegister()
{
	m_HorizontalRegisterButton->setText(QObject::tr("Record Horizontal plane"));  //
	resetGantry();
	resetCollimator();
	resetEpidPosition();
	resetCbctPosition();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToGantry()
{
	resetHorizontalRegister();
	m_GantryButton->setText(QObject::tr("Rotate Gantry"));//
	resetCollimator();
	resetEpidPosition();
	resetCbctPosition();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToCollimator()
{
	resetHorizontalRegister();
	resetGantry();
	m_CollimatorButton->setText(QObject::tr("Rotate Collimator"));//
	resetEpidPosition();
	resetCbctPosition();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToEpidPosition()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	m_EpidButton->setText(QObject::tr("Record EPID Position"));//
	resetCbctPosition();
	resetBed();
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToCbctPosition()
{
	resetBed();
	resetEpidPosition();
	resetGantry();
	resetCollimator();
	resetHorizontalRegister();
	resetLaserISO();
	resetLightCenter();
	m_CbctPositionButton->setText(QObject::tr("Record CBCT Position"));
}

void ControlWidget::doSwitchToBed()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetEpidPosition();
	resetCbctPosition();
	m_BedButton->setText(QObject::tr("Move treatment bed"));//
	resetLaserISO();
	resetLightCenter();
}

void ControlWidget::doSwitchToLaserISO()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetEpidPosition();
	resetCbctPosition();
	resetBed();
	resetLightCenter();
	m_LaserButton->setText(QObject::tr("Record Laser ISOcenter"));//
}

void ControlWidget::doSwitchToLightCenter()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetEpidPosition();
	resetCbctPosition();
	resetBed();
	resetLaserISO();
	m_LightButton->setText(QObject::tr("Record Light ISOcenter"));//
}


void ControlWidget::resetHorizontalRegister()
{
	m_HorizontalRegisterButton->setText(QObject::tr("Register Horizontal plane"));//注册水平面
}

void ControlWidget::resetGantry()
{
	m_GantryButton->setText(QObject::tr("Calibrate Gantry"));//校准机架
	//m_GantryButton->setDisabled(true);
}

void ControlWidget::resetCollimator()
{
	m_CollimatorButton->setText(QObject::tr("Calibrate Collimator"));
}

void ControlWidget::resetBed()
{
	
	m_BedButton->setText(QObject::tr("Calibrate treatment bed"));
}

void ControlWidget::resetEpidPosition()
{
	m_EpidButton->setText(QObject::tr("Calibrate EPID position"));
}

void ControlWidget::resetCbctPosition()
{
	m_CbctPositionButton->setText(QObject::tr("Calibrate CBCT position"));
}

void ControlWidget::resetLaserISO()
{
	m_LaserButton->setText(QObject::tr("Detecte Laser ISOcenter"));
}

void ControlWidget::resetLightCenter()
{
	m_LightButton->setText(QObject::tr("Detecte Light ISOcenter"));
}
void ControlWidget::reset()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	resetEpidPosition();
	resetLaserISO();
	resetLightCenter();
	resetCbctPosition();
}

void ControlWidget::setButtonStyle()
{
	m_HorizontalRegisterButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
												"font-size:18px;"
												"color: white;"
												"background-color:rgb(0,71,157);"
												"min-height:80;"
												"border:1px solid white;"
												"border-radius:8px}"
											  "QPushButton:pressed{background-color:rgb(92,156,233);"
												"border-style: inset;}");

	m_GantryButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								  "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_CollimatorButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
									  "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_BedButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								"QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_EpidButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
										"QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_CbctPositionButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
										"QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_LaserButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");

	m_LightButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(92,156,233);"
								 "border-style: inset;}");

	m_ResetButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								 "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");


	m_ReportButton->setStyleSheet("QPushButton{font-family:'Microsoft YaHei';"
										"font-size:18px;"
										"color: white;"
										"background-color:rgb(0,71,157);"
										"min-height:80; "
										"border:1px solid white;"
										"border-radius:8px}"
								  "QPushButton:pressed{background-color:rgb(92,156,233);"
										"border-style: inset;}");
}
