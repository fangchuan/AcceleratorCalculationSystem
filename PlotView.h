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
	CURVE_BED,
	CURVE_GANTRY,
	CURVE_COLLIMATOR
};
enum updateFlag{
	UPDATE_GANTRY_DEGREE = 1,
	UPDATE_COLLIMATOR_DEGREE,
	UPDATE_BED_DEGREE,
	UPDATE_BED_DISTANCE
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
		mBedXDistance(NULL), mBedYDistance(NULL), mBedZDistance(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("治疗床位移"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Distance [mm]");
		//curve
		mBedXDistance = new QwtPlotCurve("X");
		mBedXDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXDistance->setCurveFitter(fitterX);//设置曲线插值
		mBedXDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedXDistance->setPen(QPen(Qt::red, 1));//设置画笔
		mBedXDistance->attach(this);//把曲线附加到plot上

		mBedYDistance = new QwtPlotCurve("Y");
		mBedYDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYDistance->setCurveFitter(fitterY);//设置曲线插值
		mBedYDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedYDistance->setPen(QPen(Qt::yellow, 1));//设置画笔
		mBedYDistance->attach(this);//把曲线附加到plot上

		mBedZDistance = new QwtPlotCurve("Z");
		mBedZDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZDistance->setCurveFitter(fitterZ);//设置曲线插值
		mBedZDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedZDistance->setPen(QPen(Qt::green, 1));//设置画笔
		mBedZDistance->attach(this);//把曲线附加到plot上

		showCurve(mBedXDistance, true);
		showCurve(mBedYDistance, false);
		showCurve(mBedZDistance, false);
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
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve*  mBedXDistance;
	QwtPlotCurve*  mBedYDistance;
	QwtPlotCurve*  mBedZDistance;

};
//
class VelocityPlot : public BasePlot
{
	Q_OBJECT
public:
	VelocityPlot(QWidget* parent = NULL) :BasePlot(parent),
		mBedXVelocity(NULL), mBedYVelocity(NULL), mBedZVelocity(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("治疗床速度"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Velocity [mm/s]");
		//curve
		mBedXVelocity = new QwtPlotCurve("X");
		mBedXVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXVelocity->setCurveFitter(fitterX);//设置曲线插值
		mBedXVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedXVelocity->setPen(QPen(Qt::red, 1));//设置画笔
		mBedXVelocity->attach(this);//把曲线附加到plot上

		mBedYVelocity = new QwtPlotCurve("Y");
		mBedYVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYVelocity->setCurveFitter(fitterY);//设置曲线插值
		mBedYVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedYVelocity->setPen(QPen(Qt::yellow, 1));//设置画笔
		mBedYVelocity->attach(this);//把曲线附加到plot上

		mBedZVelocity = new QwtPlotCurve("Z");
		mBedZVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZVelocity->setCurveFitter(fitterZ);//设置曲线插值
		mBedZVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedZVelocity->setPen(QPen(Qt::green, 1));//设置画笔
		mBedZVelocity->attach(this);//把曲线附加到plot上

		showCurve(mBedXVelocity, true);
		showCurve(mBedYVelocity, false);
		showCurve(mBedZVelocity, false);
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
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve* mBedXVelocity;
	QwtPlotCurve* mBedYVelocity;
	QwtPlotCurve* mBedZVelocity;

};
//
class DegreeDistancePlot : public BasePlot
{
	Q_OBJECT
public:
	DegreeDistancePlot(QWidget* parent = NULL) :BasePlot(parent),
		mGantryDistance(NULL), mCollimatorDistance(NULL), mBedDistance(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("角位移"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Displacement [degree]");
		//curve
		mGantryDistance = new QwtPlotCurve(QString::fromLocal8Bit("机架"));
		mGantryDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryDistance->setCurveFitter(NULL);//设置曲线插值
		mGantryDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mGantryDistance->setPen(QPen(Qt::red, 1));//设置画笔
		mGantryDistance->attach(this);//把曲线附加到plot上

		mCollimatorDistance = new QwtPlotCurve(QString::fromLocal8Bit("准直器"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorDistance->setCurveFitter(fitterCollimator);//设置曲线插值
		mCollimatorDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mCollimatorDistance->setPen(QPen(Qt::yellow, 1));//设置画笔
		mCollimatorDistance->attach(this);//把曲线附加到plot上

		mBedDistance = new QwtPlotCurve(QString::fromLocal8Bit("治疗床"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();//三次样条插值曲线拟合器
		fitterBed->setSplineSize(50);
		mBedDistance->setCurveFitter(fitterBed);//设置曲线插值
		mBedDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedDistance->setPen(QPen(Qt::green, 1));//设置画笔
		mBedDistance->attach(this);//把曲线附加到plot上

		showCurve(mGantryDistance, true);
		showCurve(mCollimatorDistance, false);
		showCurve(mBedDistance, false);
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
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve*  mGantryDistance;
	QwtPlotCurve*  mCollimatorDistance;
	QwtPlotCurve*  mBedDistance;
};
//
class DegreeVelocityPlot : public BasePlot
{
	Q_OBJECT
public:
	DegreeVelocityPlot(QWidget* parent = NULL) :BasePlot(parent),
		mGantryVelocity(NULL), mCollimatorVelocity(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("角速度"));
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "DegreeVelocity [degree/s]");
		//curve
		mGantryVelocity = new QwtPlotCurve(QString::fromLocal8Bit("机架"));
		mGantryVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryVelocity->setCurveFitter(fitterGantry);//设置曲线插值
		mGantryVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mGantryVelocity->setPen(QPen(Qt::red, 1));//设置画笔
		mGantryVelocity->attach(this);//把曲线附加到plot上

		mCollimatorVelocity = new QwtPlotCurve(QString::fromLocal8Bit("准直器"));
		mCollimatorVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorVelocity->setCurveFitter(fitterCollimator);//设置曲线插值
		mCollimatorVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mCollimatorVelocity->setPen(QPen(Qt::yellow, 1));//设置画笔
		mCollimatorVelocity->attach(this);//把曲线附加到plot上

		mBedVelocity = new QwtPlotCurve(QString::fromLocal8Bit("治疗床"));
		mBedVelocity->setStyle(QwtPlotCurve::Lines);//直线形式
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();
		fitterBed->setSplineSize(50);
		mBedVelocity->setCurveFitter(fitterBed);//设置曲线插值
		mBedVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//使曲线更光滑
		mBedVelocity->setPen(QPen(Qt::green, 1));//设置画笔
		mBedVelocity->attach(this);//把曲线附加到plot上

		showCurve(mGantryVelocity, true);
		showCurve(mCollimatorVelocity, false);
		showCurve(mBedVelocity, false);
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
		default:break;
		}
		replot();
		setAxisAutoScale(QwtPlot::xBottom, true);
	}

private:
	QwtPlotCurve*  mGantryVelocity;
	QwtPlotCurve*  mCollimatorVelocity;
	QwtPlotCurve*  mBedVelocity;
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
	//设置更新曲线为治疗床旋转曲线
	void setBedDegreeUpdateFlag();
	//设置更新曲线为治疗床移动曲线
	void setBedDistanceUpdateFlag();
	//获取当前更新标志是否是机架旋转
	bool getGantryUpdateFlag();
	//获取当前更新标志是否为准直器旋转标志
	bool getCollimatorUpdateFlag();
	//获取当前更新标志是否是治疗床旋转标志
	bool getBedDegreeUpdateFlag();
	//获取当前更新标志是否是治疗床移动标志
	bool getBedDistanceUpdatFlag();
signals:

public slots :
	void updateGantryDegree(const float y);
	void updateGantryDegreeVelocity();
	double getGantryAvrDegreeVelocity();
	void updateCollimatorDegree(const float y);
	void updateCollimatorDegreeVelocity();
	double getCollimatorAvrDegreeVelocity();
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
	QVector<double>* mBedYDistance;
	QVector<double>* mBedZDistance;
	QVector<double>* mBedXVelocity;
	QVector<double>* mBedYVelocity;
	QVector<double>* mBedZVelocity;
	QVector<double>* mGantryDegree;
	QVector<double>* mGantryDegreeVelocity;
	QVector<double>* mCollimatorDegree;
	QVector<double>* mCollimatorDegreeVelocity;
	QVector<double>* mBedDegree;
	QVector<double>* mBedDegreeVelocity;
};

#endif // PLOTVIEW_H

