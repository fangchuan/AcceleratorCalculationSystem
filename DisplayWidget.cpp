#include "displaywidget.h"

#include "horizontalregisterdisplaywidget.h"
#include "rotationdisplaywidget.h"
#include "translationdisplaywidget.h"
#include "isocenterdisplaywidget.h"

#include <QStackedWidget>
#include <QVBoxLayout>

DisplayWidget::DisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_StackedWidget(new QStackedWidget),
	m_HorizontalWidget(new HorizontalRegisterDisplayWidget),
	m_RotationWidget(new RotationDisplayWidget),
	m_TranslationWidget(new TranslationDisplayWidget),
	m_ISOCenterDisplayWidget(new ISOCenterDisplayWidget)
{
	initUi();
}

DisplayWidget::~DisplayWidget()
{
}

void DisplayWidget::initUi()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_StackedWidget);
	m_StackedWidget->addWidget(m_HorizontalWidget);
	m_StackedWidget->addWidget(m_RotationWidget);
	m_StackedWidget->addWidget(m_TranslationWidget);
	m_StackedWidget->addWidget(m_ISOCenterDisplayWidget);
}

void DisplayWidget::reset()
{
	int index = m_StackedWidget->currentIndex();
	
	if (index == 0){
		m_HorizontalWidget->reset();
	}else{
		if (index == 1){
			m_RotationWidget->reset();
		}else{
			if (index == 2){
				m_TranslationWidget->reset();
			}
			else{
				m_ISOCenterDisplayWidget->reset();
			}
		}
	}
}

void DisplayWidget::setMarkerSize(int size)
{
	int index = m_StackedWidget->currentIndex();
	if (index == 0) {
		m_HorizontalWidget->setMarkerSize(size);
	}
	else if (index == 1) {
		m_RotationWidget->setMarkerSize(size);
	}
	else if (index == 2) {
		m_TranslationWidget->setMarkerSize(size);
	}
}

void DisplayWidget::setMarkerPosition(MarkerPointType &point)
{
	int index = m_StackedWidget->currentIndex();
	if (index == 1) {
		m_RotationWidget->setMarkerPosition(point);
	}
	else if (index == 2) {
		m_TranslationWidget->setMarkerPosition(point);
	}
}

void DisplayWidget::horizontalRegisterRecorded()
{
	if (m_StackedWidget->currentIndex() == 0) {
		m_HorizontalWidget->horizontalRegisterRecorded();
	}
}

void DisplayWidget::setCircleResult(Circle* circle)
{
	if (m_StackedWidget->currentIndex() == 1) {
		m_RotationWidget->setCircleResult(circle);
	}
}

void DisplayWidget::setTranslateResult(double bias[3])
{
	if (m_StackedWidget->currentIndex() == 2) {
		m_TranslationWidget->setTranslateResult(bias);
	}
}

void DisplayWidget::setRegisteredPosition(double point[3])
{
	if (m_StackedWidget->currentIndex() == 3) {
		m_ISOCenterDisplayWidget->setRegisteredPosition(point);
	}
}

void DisplayWidget::doSwitchToHorizontalRegister()
{
	m_StackedWidget->setCurrentIndex(0);
	m_HorizontalWidget->reset();
}

void DisplayWidget::doSwitchToGantry()
{
	m_StackedWidget->setCurrentIndex(1);
	m_RotationWidget->reset();
}

void DisplayWidget::doSwitchToCollimator()
{
	m_StackedWidget->setCurrentIndex(1);
	m_RotationWidget->reset();
}

void DisplayWidget::doSwitchToCbct()
{
	m_StackedWidget->setCurrentIndex(1);
	m_RotationWidget->reset();
}

void DisplayWidget::doSwitchToBed(int mode)
{
	int index = mode + 1;
	if (index == 1 || index == 3) {
		m_StackedWidget->setCurrentIndex(1);
		m_RotationWidget->reset();
	}
	else {
		m_StackedWidget->setCurrentIndex(2);
		m_TranslationWidget->reset();
	}
}

void DisplayWidget::doSwitchToISOCenter()
{
	m_StackedWidget->setCurrentIndex(3);
	m_ISOCenterDisplayWidget->reset();
}