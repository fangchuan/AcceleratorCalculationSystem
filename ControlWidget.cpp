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
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(10);
	m_HorizontalRegisterButton = new QPushButton(QString::fromLocal8Bit("ˮƽ��"));
	mainLayout->addWidget(m_HorizontalRegisterButton);
	m_GantryButton = new QPushButton(QString::fromLocal8Bit("����"));
	mainLayout->addWidget(m_GantryButton);
	m_CollimatorButton = new QPushButton(QString::fromLocal8Bit("׼ֱ��"));
	mainLayout->addWidget(m_CollimatorButton);
	m_BedButton = new QPushButton(QString::fromLocal8Bit("���ƴ�"));
	mainLayout->addWidget(m_BedButton);
	m_BedGroup = new QButtonGroup;
	QHBoxLayout *bedLayout = new QHBoxLayout;
	QRadioButton *rotationButton = new QRadioButton(QString::fromLocal8Bit("��ת"));
	m_BedGroup->addButton(rotationButton, 0);
	rotationButton->setChecked(true);
	bedLayout->addWidget(rotationButton);
	QRadioButton *translateButton = new QRadioButton(QString::fromLocal8Bit("ƽ��"));
	m_BedGroup->addButton(translateButton, 1);
	bedLayout->addWidget(translateButton);
	mainLayout->addLayout(bedLayout);
	m_LaserButton = new QPushButton(QString::fromLocal8Bit("��������"));
	mainLayout->addWidget(m_LaserButton);
}

void ControlWidget::buildConnections()
{
	connect(m_HorizontalRegisterButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_GantryButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_CollimatorButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_BedButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_LaserButton, &QPushButton::clicked, this, &ControlWidget::handleButtonClick);
	connect(m_BedGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ControlWidget::handleButtonGroupClick);
}

void ControlWidget::handleButtonClick()
{
	QPushButton *button = qobject_cast<QPushButton*>(sender());
	QString buttonText = button->text();
	if (button == m_HorizontalRegisterButton) {
		if (buttonText == QString::fromLocal8Bit("ˮƽ��")) {
			emit switchToHorizontalRegister();
		}
		else {
			emit recordingHorizontalRegister();
		}
	}
	else if (button == m_GantryButton) {
		if (buttonText == QString::fromLocal8Bit("����")) {
			emit switchToGantry();
		}
		else {
			emit recordingGantry();
		}
	}
	else if (button == m_CollimatorButton) {
		if (buttonText == QString::fromLocal8Bit("׼ֱ��")) {
			emit switchToCollimator();
		}
		else {
			emit recordingCollimator();
		}
	}
	else if (button == m_BedButton) {
		if (buttonText == QString::fromLocal8Bit("���ƴ�")) {
			emit switchToBed(m_BedGroup->checkedId());
		}
		else {
			emit recordingBed(m_BedGroup->checkedId());
		}
	}
	else if (button == m_LaserButton) {
		if (buttonText == QString::fromLocal8Bit("��������")) {
			emit switchToISOCenter();
		}
		else {
			emit recordingISOCenter();
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
	resetISOCenter();
}

void ControlWidget::doSwitchToGantry()
{
	resetHorizontalRegister();
	m_GantryButton->setText(QString::fromLocal8Bit("��ת����"));
	resetCollimator();
	resetBed();
	resetISOCenter();
}

void ControlWidget::doSwitchToCollimator()
{
	resetHorizontalRegister();
	resetGantry();
	m_CollimatorButton->setText(QString::fromLocal8Bit("��ת׼ֱ��"));
	resetBed();
	resetISOCenter();
}

void ControlWidget::doSwitchToBed()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	m_BedButton->setText(QString::fromLocal8Bit("�ƶ����ƴ�"));
	resetISOCenter();
}

void ControlWidget::doSwitchToISOCenter()
{
	resetHorizontalRegister();
	resetGantry();
	resetCollimator();
	resetBed();
	m_LaserButton->setText(QString::fromLocal8Bit("��¼��������"));
}

void ControlWidget::resetHorizontalRegister()
{
	m_HorizontalRegisterButton->setText(QString::fromLocal8Bit("ˮƽ��"));
}

void ControlWidget::resetGantry()
{
	m_GantryButton->setText(QString::fromLocal8Bit("����"));
}

void ControlWidget::resetCollimator()
{
	m_CollimatorButton->setText(QString::fromLocal8Bit("׼ֱ��"));
}

void ControlWidget::resetBed()
{
	m_BedButton->setText(QString::fromLocal8Bit("���ƴ�"));
}

void ControlWidget::resetISOCenter()
{
	m_LaserButton->setText(QString::fromLocal8Bit("��������"));
}