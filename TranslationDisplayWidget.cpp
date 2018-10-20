#include "TranslationDisplayWidget.h"

#include <QLabel>
#include <QVBoxLayout>

TranslationDisplayWidget::TranslationDisplayWidget(QWidget *parent)
	: QWidget(parent),
	m_MarkerSizeOrPosition(new QLabel("0")),
	m_XLabel(new QLabel("N/A")),
	m_YLabel(new QLabel("N/A")),
	m_ZLabel(new QLabel("N/A"))
{
	initUi();
}

TranslationDisplayWidget::~TranslationDisplayWidget()
{
}

void TranslationDisplayWidget::initUi()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *layout_1 = new QHBoxLayout;
	QLabel* sizeLabel = new QLabel(QObject::tr("Sizes/Coordinate:"));
	sizeLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_1->addWidget(sizeLabel);//
	m_MarkerSizeOrPosition->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_1->addWidget(m_MarkerSizeOrPosition);
	mainLayout->addLayout(layout_1);
	QHBoxLayout *layout_2 = new QHBoxLayout;
	QLabel* xLabel = new QLabel(QObject::tr("X axis translation distance:"));
	xLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_2->addWidget(xLabel);//
	m_XLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_2->addWidget(m_XLabel);
	mainLayout->addLayout(layout_2);
	QHBoxLayout *layout_3 = new QHBoxLayout;
	QLabel* yLabel = new QLabel(QObject::tr("Y axis translation distance:"));
	yLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_3->addWidget(yLabel);//
	m_YLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_3->addWidget(m_YLabel);
	mainLayout->addLayout(layout_3);
	QHBoxLayout *layout_4 = new QHBoxLayout;
	QLabel* zLabel = new QLabel(QObject::tr("Z axis translation distance:"));
	zLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_4->addWidget(zLabel);//
	m_ZLabel->setStyleSheet("QFrame, QLabel, QToolTip{font-family:'Microsoft YaHei';"
		"font-size:16px;"
		"border:1px solid white;"
		"border-radius:8px;"
		"border-style: inset;}");
	layout_4->addWidget(m_ZLabel);
	mainLayout->addLayout(layout_4);
}

void TranslationDisplayWidget::setMarkerSize(int size)
{
	m_MarkerSizeOrPosition->setText(QString::number(size));
}

void TranslationDisplayWidget::setMarkerPosition(MarkerPointType &point)
{
	QString str = QString::number(point[0], 'f', 1) + "," + QString::number(point[1], 'f', 1) + ","
		+ QString::number(point[2], 'f', 1);
	m_MarkerSizeOrPosition->setText(str);
}

void TranslationDisplayWidget::setTranslateResult(double bias[3])
{
	QString str = QStringLiteral(" %1 mm").arg(bias[0], 0, 'f', 2);
	m_XLabel->setText(str);
	str = QStringLiteral(" %1 mm").arg(bias[1], 0, 'f', 2);
	m_YLabel->setText(str);
	str = QStringLiteral(" %1 mm").arg(bias[2], 0, 'f', 2);
	m_ZLabel->setText(str);
}

void TranslationDisplayWidget::reset()
{
	m_MarkerSizeOrPosition->setText("0");
	m_XLabel->setText("N/A");
	m_YLabel->setText("N/A");
	m_ZLabel->setText("N/A");
}