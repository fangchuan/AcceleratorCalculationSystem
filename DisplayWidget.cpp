#include "displaywidget.h"

#include "horizontalregisterdisplaywidget.h"
#include "rotationdisplaywidget.h"
#include "translationdisplaywidget.h"
#include "isocenterdisplaywidget.h"
#include "cbctpositiondisplaywidget.h"

#include <QStackedWidget>
#include <QVBoxLayout>

DisplayWidget::DisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_StackedWidget(new QStackedWidget),
	m_HorizontalWidget(new HorizontalRegisterDisplayWidget),
	m_RotationWidget(new RotationDisplayWidget),
	m_TranslationWidget(new TranslationDisplayWidget),
	m_ISOCenterDisplayWidget(new ISOCenterDisplayWidget),
	m_CbctPositionDisplayWidget(new CbctPositionDisplayWidget)
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
	m_StackedWidget->addWidget(m_CbctPositionDisplayWidget);
}

void DisplayWidget::reset()
{
	int index = m_StackedWidget->currentIndex();
	
	switch (index)
	{
	case 0:
		m_HorizontalWidget->reset();
		break;
	case 1:
		m_RotationWidget->reset();
		break;
	case 2:
		m_TranslationWidget->reset();
		break;
	case 3:
		m_ISOCenterDisplayWidget->reset();
		break;
	case 4:
		m_CbctPositionDisplayWidget->reset();
		break;
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

void DisplayWidget::horizontalRegisterRecorded(double normal[3])
{
	if (m_StackedWidget->currentIndex() == 0) {
		m_HorizontalWidget->horizontalRegisterRecorded(normal);
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

void DisplayWidget::setCbctPointPosition(int index, double point[3])
{
	if (m_StackedWidget->currentIndex() == 4)
	{
		switch (index)
		{
		case 1:
			m_CbctPositionDisplayWidget->setPoint1Position(point);
			break;
		case 2:
			m_CbctPositionDisplayWidget->setPoint2Position(point);
			break;
		case 3:
			m_CbctPositionDisplayWidget->setPoint3Position(point);
			break;
		}
	}
}

void DisplayWidget::setCbctPlaneResult(Plane_T & plane)
{
	if (m_StackedWidget->currentIndex() == 4)
	{
		m_CbctPositionDisplayWidget->setPlaneResult(plane);
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

void DisplayWidget::doSwitchToCbctPosition()
{
	m_StackedWidget->setCurrentIndex(4);
	m_CbctPositionDisplayWidget->reset();
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