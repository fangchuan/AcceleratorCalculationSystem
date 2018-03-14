#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <QWidget>
#include "qwt_plot.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_curve_fitter.h>
#include <qwt_legend.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_legend_label.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qwt_point_3d.h>
#include <QTimer>
#include <QMutex>

enum curveIndex{
	CURVE_X = 0,
	CURVE_Y,
	CURVE_Z,
	CURVE_X_PAYLOAD,
	CURVE_Y_PAYLOAD,
	CURVE_Z_PAYLOAD,
	CURVE_BED,
	CURVE_GANTRY,
	CURVE_COLLIMATOR,
	CURVE_CBCT,
	CURVE_BED_PAYLOAD
};
enum updateFlag{
	UPDATE_GANTRY_DEGREE = 1,
	UPDATE_COLLIMATOR_DEGREE,
	UPDATE_BED_EMPTY_DEGREE,
	UPDATE_BED_EMPTY_DISTANCE,
	UPDATE_BED_PAYLOAD_DEGREE,
	UPDATE_BED_PAYLOAD_DISTANCE,
	UPDATE_CBCT_DEGREE
};

class BasePlot :public QwtPlot
{
	Q_OBJECT
public:
	BasePlot(QWidget* parent = NULL) :QwtPlot(parent)
	{
		this->setAutoDelete(true);
		//set axis autp resize
		this->setAxisAutoScale(QwtPlot::xBottom, true);
		this->setAxisAutoScale(QwtPlot::yLeft, true);
		//set canva
		QwtPlotCanvas *canvas = new QwtPlotCanvas();
		canvas->setBorderRadius(10);
		this->setCanvas(canvas);
		this->setCanvasBackground(Qt::black);
		//set zoom wheel
		(void) new QwtPlotMagnifier(canvas);
		//set translate by mouse left
		(void) new QwtPlotPanner(canvas);
		// grid
		QwtPlotGrid *grid = new QwtPlotGrid;
		grid->enableXMin(true);
		grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
		grid->attach(this);
		//legend
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::Checkable);
		this->insertLegend(legend, QwtPlot::BottomLegend);

		connect(legend, &QwtLegend::checked, this, &BasePlot::legendChecked);
	}

	void legendChecked(const QVariant &itemInfo, bool on)
	{
		QwtPlotItem *plotItem = infoToItem(itemInfo);
		if (plotItem)
			showCurve(plotItem, on);
	}

	void showCurve(QwtPlotItem *item, bool on)
	{
		item->setVisible(on);

		QwtLegend *lgd = qobject_cast<QwtLegend *>(legend());

		QList<QWidget *> legendWidgets =
			lgd->legendWidgets(itemToInfo(item));

		if (legendWidgets.size() == 1)
		{
			QwtLegendLabel *legendLabel =
				qobject_cast<QwtLegendLabel*>(legendWidgets[0]);

			if (legendLabel)
				legendLabel->setChecked(on);
		}

		this->replot();
	}

	virtual void updateSample(int index, const QVector<double>&xData, const QVector<double>&yData){	};
};
//
class DistancePlot : public BasePlot
{
	Q_OBJECT
public:
	DistancePlot(QWidget* parent = NULL) :BasePlot(parent),
		mBedXDistance(NULL), mBedYDistance(NULL), mBedZDistance(NULL),
		mPayloadBedXDistance(NULL), mPayloadBedYDistance(NULL),mPayloadBedZDistance(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("���ƴ�λ��"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Distance [mm]");
		//curve
		mBedXDistance = new QwtPlotCurve("X(Empty)");
		mBedXDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXDistance->setCurveFitter(fitterX);//�������߲�ֵ
		mBedXDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedXDistance->setPen(QPen(Qt::red, 1));//���û���
		mBedXDistance->attach(this);//�����߸��ӵ�plot��

		mBedYDistance = new QwtPlotCurve("Y(Empty)");
		mBedYDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYDistance->setCurveFitter(fitterY);//�������߲�ֵ
		mBedYDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedYDistance->setPen(QPen(Qt::yellow, 1));//���û���
		mBedYDistance->attach(this);//�����߸��ӵ�plot��

		mBedZDistance = new QwtPlotCurve("Z(Empty)");
		mBedZDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZDistance->setCurveFitter(fitterZ);//�������߲�ֵ
		mBedZDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedZDistance->setPen(QPen(Qt::green, 1));//���û���
		mBedZDistance->attach(this);//�����߸��ӵ�plot��

		mPayloadBedXDistance = new QwtPlotCurve("X(Payload)");
		mPayloadBedXDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterXPayload = new QwtSplineCurveFitter();
		fitterXPayload->setSplineSize(150);
		mPayloadBedXDistance->setCurveFitter(fitterXPayload);//�������߲�ֵ
		mPayloadBedXDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedXDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedXDistance->setPen(QPen(Qt::darkRed, 1));//���û���
		mPayloadBedXDistance->attach(this);//�����߸��ӵ�plot��

		mPayloadBedYDistance = new QwtPlotCurve("Y(Payload)");
		mPayloadBedYDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterYPayload = new QwtSplineCurveFitter();
		fitterYPayload->setSplineSize(150);
		mPayloadBedYDistance->setCurveFitter(fitterYPayload);//�������߲�ֵ
		mPayloadBedYDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedYDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedYDistance->setPen(QPen(Qt::darkYellow, 1));//���û���
		mPayloadBedYDistance->attach(this);//�����߸��ӵ�plot��

		mPayloadBedZDistance = new QwtPlotCurve("Z(Payload)");
		mPayloadBedZDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterZPayload = new QwtSplineCurveFitter();
		fitterZPayload->setSplineSize(150);
		mPayloadBedZDistance->setCurveFitter(fitterZPayload);//�������߲�ֵ
		mPayloadBedZDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedZDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedZDistance->setPen(QPen(Qt::darkGreen, 1));//���û���
		mPayloadBedZDistance->attach(this);//�����߸��ӵ�plot��

		showCurve(mBedXDistance, true);
		showCurve(mBedYDistance, false);
		showCurve(mBedZDistance, false);
		showCurve(mPayloadBedXDistance, false);
		showCurve(mPayloadBedYDistance, false);
		showCurve(mPayloadBedZDistance, false);

	}

	void updateSample(int index, const QVector<double>&xData, const QVector<double>&yData)
	{
		switch (index){
		case CURVE_X:
			if (NULL != mBedXDistance)
				mBedXDistance->setSamples(xData, yData);
			break;
		case CURVE_Y:
			if (NULL != mBedYDistance)
				mBedYDistance->setSamples(xData, yData);
			break;
		case CURVE_Z:
			if (NULL != mBedZDistance)
				mBedZDistance->setSamples(xData, yData);
			break;
		case CURVE_X_PAYLOAD:
			if (NULL != mPayloadBedXDistance)
				mPayloadBedXDistance->setSamples(xData, yData);
			break;
		case CURVE_Y_PAYLOAD:
			if (NULL != mPayloadBedYDistance)
				mPayloadBedYDistance->setSamples(xData, yData);
			break;
		case CURVE_Z_PAYLOAD:
			if (NULL != mPayloadBedZDistance)
				mPayloadBedZDistance->setSamples(xData, yData);
			break;
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve*  mBedXDistance;
	QwtPlotCurve*  mBedYDistance;
	QwtPlotCurve*  mBedZDistance;
	QwtPlotCurve*  mPayloadBedXDistance;
	QwtPlotCurve*  mPayloadBedYDistance;
	QwtPlotCurve*  mPayloadBedZDistance;

};
//
class VelocityPlot : public BasePlot
{
	Q_OBJECT
public:
	VelocityPlot(QWidget* parent = NULL) :BasePlot(parent),
		mBedXVelocity(NULL), mBedYVelocity(NULL), mBedZVelocity(NULL),
		mPayloadBedXVelocity(NULL), mPayloadBedYVelocity(NULL),mPayloadBedZVelocity(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("���ƴ��ٶ�"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Velocity [mm/s]");
		//curve
		mBedXVelocity = new QwtPlotCurve("X(Empty)");
		mBedXVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXVelocity->setCurveFitter(fitterX);//�������߲�ֵ
		mBedXVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedXVelocity->setPen(QPen(Qt::red, 1));//���û���
		mBedXVelocity->attach(this);//�����߸��ӵ�plot��

		mBedYVelocity = new QwtPlotCurve("Y(Empty)");
		mBedYVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYVelocity->setCurveFitter(fitterY);//�������߲�ֵ
		mBedYVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedYVelocity->setPen(QPen(Qt::yellow, 1));//���û���
		mBedYVelocity->attach(this);//�����߸��ӵ�plot��

		mBedZVelocity = new QwtPlotCurve("Z(Empty)");
		mBedZVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZVelocity->setCurveFitter(fitterZ);//�������߲�ֵ
		mBedZVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedZVelocity->setPen(QPen(Qt::green, 1));//���û���
		mBedZVelocity->attach(this);//�����߸��ӵ�plot��

		mPayloadBedXVelocity = new QwtPlotCurve("X(Payload)");
		mPayloadBedXVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterXPayload = new QwtSplineCurveFitter();
		fitterXPayload->setSplineSize(150);
		mPayloadBedXVelocity->setCurveFitter(fitterXPayload);//�������߲�ֵ
		mPayloadBedXVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedXVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedXVelocity->setPen(QPen(Qt::darkRed, 1));//���û���
		mPayloadBedXVelocity->attach(this);//�����߸��ӵ�plot��

		mPayloadBedYVelocity = new QwtPlotCurve("Y(Payload)");
		mPayloadBedYVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterYPayload = new QwtSplineCurveFitter();
		fitterYPayload->setSplineSize(150);
		mPayloadBedYVelocity->setCurveFitter(fitterYPayload);//�������߲�ֵ
		mPayloadBedYVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedYVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedYVelocity->setPen(QPen(Qt::darkYellow, 1));//���û���
		mPayloadBedYVelocity->attach(this);//�����߸��ӵ�plot��

		mPayloadBedZVelocity = new QwtPlotCurve("Z(Payload)");
		mPayloadBedZVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterZPayload = new QwtSplineCurveFitter();
		fitterZPayload->setSplineSize(150);
		mPayloadBedZVelocity->setCurveFitter(fitterZPayload);//�������߲�ֵ
		mPayloadBedZVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedZVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedZVelocity->setPen(QPen(Qt::darkGreen, 1));//���û���
		mPayloadBedZVelocity->attach(this);//�����߸��ӵ�plot��

		showCurve(mBedXVelocity, true);
		showCurve(mBedYVelocity, false);
		showCurve(mBedZVelocity, false);
		showCurve(mPayloadBedXVelocity, false);
		showCurve(mPayloadBedYVelocity, false);
		showCurve(mPayloadBedZVelocity, false);
	}

	void updateSample(int index, const QVector<double>&xData, const QVector<double>&yData)
	{
		switch (index){
		case CURVE_X:
			if (NULL != mBedXVelocity)
				mBedXVelocity->setSamples(xData, yData);
			break;
		case CURVE_Y:
			if (NULL != mBedYVelocity)
				mBedYVelocity->setSamples(xData, yData);
			break;
		case CURVE_Z:
			if (NULL != mBedZVelocity)
				mBedZVelocity->setSamples(xData, yData);
			break;
		case CURVE_X_PAYLOAD:
			if (NULL != mPayloadBedXVelocity)
				mPayloadBedXVelocity->setSamples(xData, yData);
			break;
		case CURVE_Y_PAYLOAD:
			if (NULL != mPayloadBedYVelocity)
				mPayloadBedYVelocity->setSamples(xData, yData);
			break;
		case CURVE_Z_PAYLOAD:
			if (NULL != mPayloadBedZVelocity)
				mPayloadBedZVelocity->setSamples(xData, yData);
			break;
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve* mBedXVelocity;
	QwtPlotCurve* mBedYVelocity;
	QwtPlotCurve* mBedZVelocity;
	QwtPlotCurve* mPayloadBedXVelocity;
	QwtPlotCurve* mPayloadBedYVelocity;
	QwtPlotCurve* mPayloadBedZVelocity;

};
//
class DegreeDistancePlot : public BasePlot
{
	Q_OBJECT
public:
	DegreeDistancePlot(QWidget* parent = NULL) :BasePlot(parent),
		mGantryDistance(NULL), mCollimatorDistance(NULL), mBedDistance(NULL),
		mCbctDistance(NULL), mPayloadBedDistance(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("��λ��"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Displacement [degree]");
		//curve
		mGantryDistance = new QwtPlotCurve(QObject::tr("Gantry"));
		mGantryDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryDistance->setCurveFitter(NULL);//�������߲�ֵ
		mGantryDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mGantryDistance->setPen(QPen(Qt::red, 1));//���û���
		mGantryDistance->attach(this);//�����߸��ӵ�plot��

		mCollimatorDistance = new QwtPlotCurve(QObject::tr("Collimator"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorDistance->setCurveFitter(fitterCollimator);//�������߲�ֵ
		mCollimatorDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mCollimatorDistance->setPen(QPen(Qt::yellow, 1));//���û���
		mCollimatorDistance->attach(this);//�����߸��ӵ�plot��

		mBedDistance = new QwtPlotCurve(QObject::tr("Bed(Empty)"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();//����������ֵ���������
		fitterBed->setSplineSize(50);
		mBedDistance->setCurveFitter(fitterBed);//�������߲�ֵ
		mBedDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedDistance->setPen(QPen(Qt::green, 1));//���û���
		mBedDistance->attach(this);//�����߸��ӵ�plot��

		mCbctDistance = new QwtPlotCurve(QObject::tr("CBCT"));
		mCbctDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterCBCT = new QwtSplineCurveFitter();
		fitterCBCT->setSplineSize(50);
		mCbctDistance->setCurveFitter(fitterCBCT);//�������߲�ֵ
		mCbctDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCbctDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mCbctDistance->setPen(QPen(Qt::white, 1));//���û���
		mCbctDistance->attach(this);//�����߸��ӵ�plot��

		mPayloadBedDistance = new QwtPlotCurve(QObject::tr("Bed(Payload)"));
		mPayloadBedDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterBedPayload = new QwtSplineCurveFitter();//����������ֵ���������
		fitterBedPayload->setSplineSize(50);
		mPayloadBedDistance->setCurveFitter(fitterBedPayload);//�������߲�ֵ
		mPayloadBedDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedDistance->setPen(QPen(Qt::cyan, 1));//���û���
		mPayloadBedDistance->attach(this);//�����߸��ӵ�plot��

		showCurve(mGantryDistance, true);
		showCurve(mCollimatorDistance, false);
		showCurve(mBedDistance, false);
		showCurve(mCbctDistance, false);
		showCurve(mPayloadBedDistance, false);
	}

	void updateSample(int index, const QVector<double>&xData, const QVector<double>&yData)
	{
		switch (index){
		case CURVE_GANTRY:
			if (NULL != mGantryDistance)
				mGantryDistance->setSamples(xData, yData);
			break;
		case CURVE_COLLIMATOR:
			if (NULL != mCollimatorDistance)
				mCollimatorDistance->setSamples(xData, yData);
			break;
		case CURVE_BED:
			if (NULL != mBedDistance)
				mBedDistance->setSamples(xData, yData);
			break;
		case CURVE_CBCT:
			if (NULL != mCbctDistance)
				mCbctDistance->setSamples(xData, yData);
			break;
		case CURVE_BED_PAYLOAD:
			if (NULL != mPayloadBedDistance)
				mPayloadBedDistance->setSamples(xData, yData);
			break;
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve*  mGantryDistance;
	QwtPlotCurve*  mCollimatorDistance;
	QwtPlotCurve*  mBedDistance;
	QwtPlotCurve*  mCbctDistance;
	QwtPlotCurve*  mPayloadBedDistance;
};
//
class DegreeVelocityPlot : public BasePlot
{
	Q_OBJECT
public:
	DegreeVelocityPlot(QWidget* parent = NULL) :BasePlot(parent),
		mGantryVelocity(NULL), mCollimatorVelocity(NULL), mBedVelocity(NULL),
		mCbctVelocity(NULL), mPayloadBedVelocity(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("���ٶ�"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "DegreeVelocity [degree/s]");
		//curve
		mGantryVelocity = new QwtPlotCurve(QObject::tr("Gantry"));
		mGantryVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryVelocity->setCurveFitter(fitterGantry);//�������߲�ֵ
		mGantryVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mGantryVelocity->setPen(QPen(Qt::red, 1));//���û���
		mGantryVelocity->attach(this);//�����߸��ӵ�plot��

		mCollimatorVelocity = new QwtPlotCurve(QObject::tr("Collimator"));
		mCollimatorVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorVelocity->setCurveFitter(fitterCollimator);//�������߲�ֵ
		mCollimatorVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mCollimatorVelocity->setPen(QPen(Qt::yellow, 1));//���û���
		mCollimatorVelocity->attach(this);//�����߸��ӵ�plot��

		mBedVelocity = new QwtPlotCurve(QObject::tr("Bed(Empty)"));
		mBedVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();
		fitterBed->setSplineSize(50);
		mBedVelocity->setCurveFitter(fitterBed);//�������߲�ֵ
		mBedVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedVelocity->setPen(QPen(Qt::green, 1));//���û���
		mBedVelocity->attach(this);//�����߸��ӵ�plot��

		mCbctVelocity = new QwtPlotCurve(QObject::tr("CBCT"));
		mCbctVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterCBCT = new QwtSplineCurveFitter();
		fitterCBCT->setSplineSize(50);
		mCbctVelocity->setCurveFitter(fitterCBCT);//�������߲�ֵ
		mCbctVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCbctVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mCbctVelocity->setPen(QPen(Qt::white, 1));//���û���
		mCbctVelocity->attach(this);//�����߸��ӵ�plot��

		mPayloadBedVelocity = new QwtPlotCurve(QObject::tr("Bed(Payload)"));
		mPayloadBedVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterBedPayload = new QwtSplineCurveFitter();//����������ֵ���������
		fitterBedPayload->setSplineSize(50);
		mPayloadBedVelocity->setCurveFitter(fitterBedPayload);//�������߲�ֵ
		mPayloadBedVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mPayloadBedVelocity->setPen(QPen(Qt::cyan, 1));//���û���
		mPayloadBedVelocity->attach(this);//�����߸��ӵ�plot��

		showCurve(mGantryVelocity, true);
		showCurve(mCollimatorVelocity, false);
		showCurve(mBedVelocity, false);
		showCurve(mCbctVelocity, false);
		showCurve(mPayloadBedVelocity, false);
	}

	void updateSample(int index, const QVector<double>&xData, const QVector<double>&yData)
	{
		switch (index){
		case CURVE_GANTRY:
			if (NULL != mGantryVelocity)
				mGantryVelocity->setSamples(xData, yData);
			break;
		case CURVE_COLLIMATOR:
			if (NULL != mCollimatorVelocity)
				mCollimatorVelocity->setSamples(xData, yData);
			break;
		case CURVE_BED:
			if (NULL != mBedVelocity)
				mBedVelocity->setSamples(xData, yData);
			break;
		case CURVE_CBCT:
			if (NULL != mCbctVelocity)
				mCbctVelocity->setSamples(xData, yData);
			break;
		case CURVE_BED_PAYLOAD:
			if (NULL != mPayloadBedVelocity)
				mPayloadBedVelocity->setSamples(xData, yData);
			break;
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve*  mGantryVelocity;
	QwtPlotCurve*  mCollimatorVelocity;
	QwtPlotCurve*  mBedVelocity;
	QwtPlotCurve*  mCbctVelocity;
	QwtPlotCurve*  mPayloadBedVelocity;
};

class PlotView : public QWidget
{

	Q_OBJECT
public:
	explicit PlotView(QWidget *parent = 0);
	~PlotView();
	//��λͼ��
	void resetPlot();
	//���ø�������Ϊ������ת����
	void setGantryUpdateFlag();
	//���ø�������Ϊ׼ֱ����ת����
	void setCollimatorUpdateFlag();
	//���ø�������Ϊcbct��ת����
	void setCBCTUpdateFlag();
	//���ø�������Ϊ���ƴ�(����)��ת����
	void setEmptyBedDegreeUpdateFlag();
	//���ø�������Ϊ���ƴ�(����)�ƶ�����
	void setEmptyBedDistanceUpdateFlag();
	//���ø�������Ϊ���ƴ�(����)��ת����
	void setPayloadBedDegreeUpdateFlag();
	//���ø�������Ϊ���ƴ�(����)�ƶ�����
	void setPayloadBedDistanceUpdateFlag();
	//��ȡ��ǰ���±�־�Ƿ��ǻ�����ת
	bool getGantryUpdateFlag();
	//��ȡ��ǰ���±�־�Ƿ�Ϊ׼ֱ����ת��־
	bool getCollimatorUpdateFlag();
	//��ȡ��ǰ���±�־�Ƿ�ΪCBCT��ת��־
	bool getCBCTUpdateFlag();
	//��ȡ��ǰ���±�־�Ƿ������ƴ���ת��־
	int getBedDegreeUpdateFlag();
	//��ȡ��ǰ���±�־�Ƿ������ƴ��ƶ���־
	int getBedDistanceUpdatFlag();


public slots :
	void exportDistancePlot();
	void exportDistanceVelPlot();
	void exportDegreePlot();
	void exportDegreeVelPlot();
	void updateGantryDegree(const float y);
	void updateGantryDegreeVelocity();
	double getGantryAvrDegreeVelocity();
	void updateCollimatorDegree(const float y);
	void updateCollimatorDegreeVelocity();
	double getCollimatorAvrDegreeVelocity();
	void updateCBCTDegree(const float y);
	void updateCBCTDegreeVelocity();
	double getCBCTAvrDegreeVelocity();
	void updateBedDegree(const float y);
	void updateBedDegreeVelocity();
	double getBedAvrDegreeVelocity();
	void updateBedDistance(const float x, const float y, const float z);
	void updateBedDistanceVelocity();
	void clearCurveDataVector();

private:
	virtual void closeEvent(QCloseEvent *event);
	void initUi();
	void initCurveDataVector();
	void initTimer();
	void update();
	void stopTimer();

	DistancePlot*  distancePlot;
	VelocityPlot*  velocityPlot;
	DegreeDistancePlot*  degreeDistancePlot;
	DegreeVelocityPlot*  degreeVelocityPlot;
	int  mUpdateFlag;

	QMutex  mCurveDataMutex;
	QTimer*    mTimer;
	QVector<double>  mDegreeTimeData;
	QVector<double>  mDistanceTimeData;
	QVector<double>* mBedXDistance;
	QVector<double>* mPayloadBedXDistance;
	QVector<double>* mBedYDistance;
	QVector<double>* mPayloadBedYDistance;
	QVector<double>* mBedZDistance;
	QVector<double>* mPayloadBedZDistance;
	QVector<double>* mBedXVelocity;
	QVector<double>* mPayloadBedXVelocity;
	QVector<double>* mBedYVelocity;
	QVector<double>* mPayloadBedYVelocity;
	QVector<double>* mBedZVelocity;
	QVector<double>* mPayloadBedZVelocity;
	QVector<double>* mGantryDegree;
	QVector<double>* mGantryDegreeVelocity;
	QVector<double>* mCollimatorDegree;
	QVector<double>* mCollimatorDegreeVelocity;
	QVector<double>* mCbctDegree;
	QVector<double>* mCbctDegreeVelocity;
	QVector<double>* mBedDegree;
	QVector<double>* mBedDegreeVelocity;
	QVector<double>* mPayloadBedDegree;
	QVector<double>* mPayloadBedDegreeVelocity;

};

#endif // PLOTVIEW_H

