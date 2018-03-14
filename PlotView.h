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
		this->setTitle(QString::fromLocal8Bit("治疗床位移"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Distance [mm]");
		//curve
		mBedXDistance = new QwtPlotCurve("X(Empty)");
		mBedXDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXDistance->setCurveFitter(fitterX);//设置曲线插值
		mBedXDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedXDistance->setPen(QPen(Qt::red, 1));//设置画笔
		mBedXDistance->attach(this);//把曲线附加到plot上

		mBedYDistance = new QwtPlotCurve("Y(Empty)");
		mBedYDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYDistance->setCurveFitter(fitterY);//设置曲线插值
		mBedYDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedYDistance->setPen(QPen(Qt::yellow, 1));//设置画笔
		mBedYDistance->attach(this);//把曲线附加到plot上

		mBedZDistance = new QwtPlotCurve("Z(Empty)");
		mBedZDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZDistance->setCurveFitter(fitterZ);//设置曲线插值
		mBedZDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedZDistance->setPen(QPen(Qt::green, 1));//设置画笔
		mBedZDistance->attach(this);//把曲线附加到plot上

		mPayloadBedXDistance = new QwtPlotCurve("X(Payload)");
		mPayloadBedXDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterXPayload = new QwtSplineCurveFitter();
		fitterXPayload->setSplineSize(150);
		mPayloadBedXDistance->setCurveFitter(fitterXPayload);//设置曲线插值
		mPayloadBedXDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedXDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedXDistance->setPen(QPen(Qt::darkRed, 1));//设置画笔
		mPayloadBedXDistance->attach(this);//把曲线附加到plot上

		mPayloadBedYDistance = new QwtPlotCurve("Y(Payload)");
		mPayloadBedYDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterYPayload = new QwtSplineCurveFitter();
		fitterYPayload->setSplineSize(150);
		mPayloadBedYDistance->setCurveFitter(fitterYPayload);//设置曲线插值
		mPayloadBedYDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedYDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedYDistance->setPen(QPen(Qt::darkYellow, 1));//设置画笔
		mPayloadBedYDistance->attach(this);//把曲线附加到plot上

		mPayloadBedZDistance = new QwtPlotCurve("Z(Payload)");
		mPayloadBedZDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterZPayload = new QwtSplineCurveFitter();
		fitterZPayload->setSplineSize(150);
		mPayloadBedZDistance->setCurveFitter(fitterZPayload);//设置曲线插值
		mPayloadBedZDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedZDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedZDistance->setPen(QPen(Qt::darkGreen, 1));//设置画笔
		mPayloadBedZDistance->attach(this);//把曲线附加到plot上

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
		this->setTitle(QString::fromLocal8Bit("治疗床速度"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Velocity [mm/s]");
		//curve
		mBedXVelocity = new QwtPlotCurve("X(Empty)");
		mBedXVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXVelocity->setCurveFitter(fitterX);//设置曲线插值
		mBedXVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedXVelocity->setPen(QPen(Qt::red, 1));//设置画笔
		mBedXVelocity->attach(this);//把曲线附加到plot上

		mBedYVelocity = new QwtPlotCurve("Y(Empty)");
		mBedYVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYVelocity->setCurveFitter(fitterY);//设置曲线插值
		mBedYVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedYVelocity->setPen(QPen(Qt::yellow, 1));//设置画笔
		mBedYVelocity->attach(this);//把曲线附加到plot上

		mBedZVelocity = new QwtPlotCurve("Z(Empty)");
		mBedZVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZVelocity->setCurveFitter(fitterZ);//设置曲线插值
		mBedZVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedZVelocity->setPen(QPen(Qt::green, 1));//设置画笔
		mBedZVelocity->attach(this);//把曲线附加到plot上

		mPayloadBedXVelocity = new QwtPlotCurve("X(Payload)");
		mPayloadBedXVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterXPayload = new QwtSplineCurveFitter();
		fitterXPayload->setSplineSize(150);
		mPayloadBedXVelocity->setCurveFitter(fitterXPayload);//设置曲线插值
		mPayloadBedXVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedXVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedXVelocity->setPen(QPen(Qt::darkRed, 1));//设置画笔
		mPayloadBedXVelocity->attach(this);//把曲线附加到plot上

		mPayloadBedYVelocity = new QwtPlotCurve("Y(Payload)");
		mPayloadBedYVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterYPayload = new QwtSplineCurveFitter();
		fitterYPayload->setSplineSize(150);
		mPayloadBedYVelocity->setCurveFitter(fitterYPayload);//设置曲线插值
		mPayloadBedYVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedYVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedYVelocity->setPen(QPen(Qt::darkYellow, 1));//设置画笔
		mPayloadBedYVelocity->attach(this);//把曲线附加到plot上

		mPayloadBedZVelocity = new QwtPlotCurve("Z(Payload)");
		mPayloadBedZVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterZPayload = new QwtSplineCurveFitter();
		fitterZPayload->setSplineSize(150);
		mPayloadBedZVelocity->setCurveFitter(fitterZPayload);//设置曲线插值
		mPayloadBedZVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedZVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedZVelocity->setPen(QPen(Qt::darkGreen, 1));//设置画笔
		mPayloadBedZVelocity->attach(this);//把曲线附加到plot上

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
		this->setTitle(QString::fromLocal8Bit("角位移"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Displacement [degree]");
		//curve
		mGantryDistance = new QwtPlotCurve(QObject::tr("Gantry"));
		mGantryDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryDistance->setCurveFitter(NULL);//设置曲线插值
		mGantryDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mGantryDistance->setPen(QPen(Qt::red, 1));//设置画笔
		mGantryDistance->attach(this);//把曲线附加到plot上

		mCollimatorDistance = new QwtPlotCurve(QObject::tr("Collimator"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorDistance->setCurveFitter(fitterCollimator);//设置曲线插值
		mCollimatorDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mCollimatorDistance->setPen(QPen(Qt::yellow, 1));//设置画笔
		mCollimatorDistance->attach(this);//把曲线附加到plot上

		mBedDistance = new QwtPlotCurve(QObject::tr("Bed(Empty)"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();//三次样条插值曲线拟合器
		fitterBed->setSplineSize(50);
		mBedDistance->setCurveFitter(fitterBed);//设置曲线插值
		mBedDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedDistance->setPen(QPen(Qt::green, 1));//设置画笔
		mBedDistance->attach(this);//把曲线附加到plot上

		mCbctDistance = new QwtPlotCurve(QObject::tr("CBCT"));
		mCbctDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterCBCT = new QwtSplineCurveFitter();
		fitterCBCT->setSplineSize(50);
		mCbctDistance->setCurveFitter(fitterCBCT);//设置曲线插值
		mCbctDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCbctDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mCbctDistance->setPen(QPen(Qt::white, 1));//设置画笔
		mCbctDistance->attach(this);//把曲线附加到plot上

		mPayloadBedDistance = new QwtPlotCurve(QObject::tr("Bed(Payload)"));
		mPayloadBedDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterBedPayload = new QwtSplineCurveFitter();//三次样条插值曲线拟合器
		fitterBedPayload->setSplineSize(50);
		mPayloadBedDistance->setCurveFitter(fitterBedPayload);//设置曲线插值
		mPayloadBedDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedDistance->setPen(QPen(Qt::cyan, 1));//设置画笔
		mPayloadBedDistance->attach(this);//把曲线附加到plot上

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
		this->setTitle(QString::fromLocal8Bit("角速度"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "DegreeVelocity [degree/s]");
		//curve
		mGantryVelocity = new QwtPlotCurve(QObject::tr("Gantry"));
		mGantryVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryVelocity->setCurveFitter(fitterGantry);//设置曲线插值
		mGantryVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mGantryVelocity->setPen(QPen(Qt::red, 1));//设置画笔
		mGantryVelocity->attach(this);//把曲线附加到plot上

		mCollimatorVelocity = new QwtPlotCurve(QObject::tr("Collimator"));
		mCollimatorVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorVelocity->setCurveFitter(fitterCollimator);//设置曲线插值
		mCollimatorVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mCollimatorVelocity->setPen(QPen(Qt::yellow, 1));//设置画笔
		mCollimatorVelocity->attach(this);//把曲线附加到plot上

		mBedVelocity = new QwtPlotCurve(QObject::tr("Bed(Empty)"));
		mBedVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();
		fitterBed->setSplineSize(50);
		mBedVelocity->setCurveFitter(fitterBed);//设置曲线插值
		mBedVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedVelocity->setPen(QPen(Qt::green, 1));//设置画笔
		mBedVelocity->attach(this);//把曲线附加到plot上

		mCbctVelocity = new QwtPlotCurve(QObject::tr("CBCT"));
		mCbctVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterCBCT = new QwtSplineCurveFitter();
		fitterCBCT->setSplineSize(50);
		mCbctVelocity->setCurveFitter(fitterCBCT);//设置曲线插值
		mCbctVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCbctVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mCbctVelocity->setPen(QPen(Qt::white, 1));//设置画笔
		mCbctVelocity->attach(this);//把曲线附加到plot上

		mPayloadBedVelocity = new QwtPlotCurve(QObject::tr("Bed(Payload)"));
		mPayloadBedVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterBedPayload = new QwtSplineCurveFitter();//三次样条插值曲线拟合器
		fitterBedPayload->setSplineSize(50);
		mPayloadBedVelocity->setCurveFitter(fitterBedPayload);//设置曲线插值
		mPayloadBedVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mPayloadBedVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mPayloadBedVelocity->setPen(QPen(Qt::cyan, 1));//设置画笔
		mPayloadBedVelocity->attach(this);//把曲线附加到plot上

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
	//复位图表
	void resetPlot();
	//设置更新曲线为机架旋转曲线
	void setGantryUpdateFlag();
	//设置更新曲线为准直器旋转曲线
	void setCollimatorUpdateFlag();
	//设置更新曲线为cbct旋转曲线
	void setCBCTUpdateFlag();
	//设置更新曲线为治疗床(空载)旋转曲线
	void setEmptyBedDegreeUpdateFlag();
	//设置更新曲线为治疗床(空载)移动曲线
	void setEmptyBedDistanceUpdateFlag();
	//设置更新曲线为治疗床(负载)旋转曲线
	void setPayloadBedDegreeUpdateFlag();
	//设置更新曲线为治疗床(负载)移动曲线
	void setPayloadBedDistanceUpdateFlag();
	//获取当前更新标志是否是机架旋转
	bool getGantryUpdateFlag();
	//获取当前更新标志是否为准直器旋转标志
	bool getCollimatorUpdateFlag();
	//获取当前更新标志是否为CBCT旋转标志
	bool getCBCTUpdateFlag();
	//获取当前更新标志是否是治疗床旋转标志
	int getBedDegreeUpdateFlag();
	//获取当前更新标志是否是治疗床移动标志
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

