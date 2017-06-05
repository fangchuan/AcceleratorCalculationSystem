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
//
class DistancePlot : public QwtPlot
{
	Q_OBJECT
public:
	DistancePlot(QWidget* parent = NULL) :QwtPlot(parent),
		mBedXDistance(NULL), mBedYDistance(NULL), mBedZDistance(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("�����˶�λ��"));
		this->setAutoDelete(true);
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Distance [mm]");
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
		//curve
		mBedXDistance = new QwtPlotCurve("X");
		mBedXDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXDistance->setCurveFitter(fitterX);//�������߲�ֵ
		mBedXDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedXDistance->setPen(QPen(Qt::red, 1));//���û���
		mBedXDistance->attach(this);//�����߸��ӵ�plot��

		mBedYDistance = new QwtPlotCurve("Y");
		mBedYDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYDistance->setCurveFitter(fitterY);//�������߲�ֵ
		mBedYDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedYDistance->setPen(QPen(Qt::yellow, 1));//���û���
		mBedYDistance->attach(this);//�����߸��ӵ�plot��

		mBedZDistance = new QwtPlotCurve("Z");
		mBedZDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZDistance->setCurveFitter(fitterZ);//�������߲�ֵ
		mBedZDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedZDistance->setPen(QPen(Qt::green, 1));//���û���
		mBedZDistance->attach(this);//�����߸��ӵ�plot��

		connect(legend, &QwtLegend::checked, this, &DistancePlot::legendChecked);

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

private:
	QwtPlotCurve*  mBedXDistance;
	QwtPlotCurve*  mBedYDistance;
	QwtPlotCurve*  mBedZDistance;

};
//
class VelocityPlot : public QwtPlot
{
	Q_OBJECT
public:
	VelocityPlot(QWidget* parent = NULL) :QwtPlot(parent),
		mBedXVelocity(NULL), mBedYVelocity(NULL), mBedZVelocity(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("�����˶��ٶ�"));
		this->setAutoDelete(true);
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Velocity [mm/s]");
		//set axis autp resize
		this->setAxisAutoScale(QwtPlot::xBottom, true);
		this->setAxisAutoScale(QwtPlot::yLeft, true);
		//set canva
		QwtPlotCanvas *canvas = new QwtPlotCanvas();
		canvas->setBorderRadius(10);
		this->setCanvas(canvas);
		this->setCanvasBackground(Qt::black);
		//set zoom wheel
		(void) new QwtPlotMagnifier(this->canvas());
		//set translate by mouse left
		(void) new QwtPlotPanner(this->canvas());
		// grid
		QwtPlotGrid *grid = new QwtPlotGrid;
		grid->enableXMin(true);
		grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
		grid->attach(this);
		//legend
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::Checkable);
		this->insertLegend(legend, QwtPlot::BottomLegend);
		//curve
		mBedXVelocity = new QwtPlotCurve("X");
		mBedXVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterX = new QwtSplineCurveFitter();
		fitterX->setSplineSize(150);
		mBedXVelocity->setCurveFitter(fitterX);//�������߲�ֵ
		mBedXVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedXVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedXVelocity->setPen(QPen(Qt::red, 1));//���û���
		mBedXVelocity->attach(this);//�����߸��ӵ�plot��

		mBedYVelocity = new QwtPlotCurve("Y");
		mBedYVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterY = new QwtSplineCurveFitter();
		fitterY->setSplineSize(150);
		mBedYVelocity->setCurveFitter(fitterY);//�������߲�ֵ
		mBedYVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedYVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedYVelocity->setPen(QPen(Qt::yellow, 1));//���û���
		mBedYVelocity->attach(this);//�����߸��ӵ�plot��

		mBedZVelocity = new QwtPlotCurve("Z");
		mBedZVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterZ = new QwtSplineCurveFitter();
		fitterZ->setSplineSize(150);
		mBedZVelocity->setCurveFitter(fitterZ);//�������߲�ֵ
		mBedZVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedZVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedZVelocity->setPen(QPen(Qt::green, 1));//���û���
		mBedZVelocity->attach(this);//�����߸��ӵ�plot��

		connect(legend, &QwtLegend::checked, this, &VelocityPlot::legendChecked);

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
				qobject_cast<QwtLegendLabel *>(legendWidgets[0]);

			if (legendLabel)
				legendLabel->setChecked(on);
		}
		this->replot();
	}
private:
	QwtPlotCurve* mBedXVelocity;
	QwtPlotCurve* mBedYVelocity;
	QwtPlotCurve* mBedZVelocity;

};
//
class DegreeDistancePlot : public QwtPlot
{
	Q_OBJECT
public:
	DegreeDistancePlot(QWidget* parent = NULL) :QwtPlot(parent),
		mGantryDistance(NULL), mCollimatorDistance(NULL), mBedDistance(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("��ͷ/�����Ƕ�"));
		this->setAutoDelete(true);
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "Displacement [degree]");
		//set axis autp resize
		this->setAxisAutoScale(QwtPlot::xBottom, true);
		this->setAxisAutoScale(QwtPlot::yLeft, true);
		//set canva
		QwtPlotCanvas *canvas = new QwtPlotCanvas();
		canvas->setBorderRadius(10);
		this->setCanvas(canvas);
		this->setCanvasBackground(Qt::black);
		//set zoom wheel
		(void) new QwtPlotMagnifier(this->canvas());
		//set translate by mouse left
		(void) new QwtPlotPanner(this->canvas());
		// grid
		QwtPlotGrid *grid = new QwtPlotGrid;
		grid->enableXMin(true);
		grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
		grid->attach(this);
		//legend
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::Checkable);
		this->insertLegend(legend, QwtPlot::BottomLegend);
		//curve
		mGantryDistance = new QwtPlotCurve(QString::fromLocal8Bit("����"));
		mGantryDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryDistance->setCurveFitter(NULL);//�������߲�ֵ
		mGantryDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mGantryDistance->setPen(QPen(Qt::red, 1));//���û���
		mGantryDistance->attach(this);//�����߸��ӵ�plot��

		mCollimatorDistance = new QwtPlotCurve(QString::fromLocal8Bit("׼ֱ��"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorDistance->setCurveFitter(fitterCollimator);//�������߲�ֵ
		mCollimatorDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mCollimatorDistance->setPen(QPen(Qt::yellow, 1));//���û���
		mCollimatorDistance->attach(this);//�����߸��ӵ�plot��

		mBedDistance = new QwtPlotCurve(QString::fromLocal8Bit("���ƴ�"));
		mCollimatorDistance->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();//����������ֵ���������
		fitterBed->setSplineSize(50);
		mBedDistance->setCurveFitter(fitterBed);//�������߲�ֵ
		mBedDistance->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedDistance->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedDistance->setPen(QPen(Qt::green, 1));//���û���
		mBedDistance->attach(this);//�����߸��ӵ�plot��

		connect(legend, &QwtLegend::checked, this, &DegreeDistancePlot::legendChecked);

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
				qobject_cast<QwtLegendLabel *>(legendWidgets[0]);

			if (legendLabel)
				legendLabel->setChecked(on);
		}
		this->replot();
	}
private:
	QwtPlotCurve*  mGantryDistance;
	QwtPlotCurve*  mCollimatorDistance;
	QwtPlotCurve*  mBedDistance;
};
//
class DegreeVelocityPlot : public QwtPlot
{
	Q_OBJECT
public:
	DegreeVelocityPlot(QWidget* parent = NULL) :QwtPlot(parent),
		mGantryVelocity(NULL), mCollimatorVelocity(NULL)
	{
		this->setTitle(QString::fromLocal8Bit("��ͷ/�������ٶ�"));
		this->setAutoDelete(true);
		//set axis name
		this->setAxisTitle(QwtPlot::xBottom, "Time [s]");
		this->setAxisTitle(QwtPlot::yLeft, "DegreeVelocity [degree/s]");
		//set axis autp resize
		this->setAxisAutoScale(QwtPlot::xBottom, true);
		this->setAxisAutoScale(QwtPlot::yLeft, true);
		//set canva
		QwtPlotCanvas *canvas = new QwtPlotCanvas();
		canvas->setBorderRadius(10);
		this->setCanvas(canvas);
		this->setCanvasBackground(Qt::black);
		//set zoom wheel
		(void) new QwtPlotMagnifier(this->canvas());
		//set translate by mouse left
		(void) new QwtPlotPanner(this->canvas());
		// grid
		QwtPlotGrid *grid = new QwtPlotGrid;
		grid->enableXMin(true);
		grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
		grid->attach(this);
		//legend
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::Checkable);
		this->insertLegend(legend, QwtPlot::BottomLegend);
		//curve
		mGantryVelocity = new QwtPlotCurve(QString::fromLocal8Bit("����"));
		mGantryVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterGantry = new QwtSplineCurveFitter();
		fitterGantry->setSplineSize(50);
		mGantryVelocity->setCurveFitter(fitterGantry);//�������߲�ֵ
		mGantryVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mGantryVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mGantryVelocity->setPen(QPen(Qt::red, 1));//���û���
		mGantryVelocity->attach(this);//�����߸��ӵ�plot��

		mCollimatorVelocity = new QwtPlotCurve(QString::fromLocal8Bit("׼ֱ��"));
		mCollimatorVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterCollimator = new QwtSplineCurveFitter();
		fitterCollimator->setSplineSize(50);
		mCollimatorVelocity->setCurveFitter(fitterCollimator);//�������߲�ֵ
		mCollimatorVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mCollimatorVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mCollimatorVelocity->setPen(QPen(Qt::yellow, 1));//���û���
		mCollimatorVelocity->attach(this);//�����߸��ӵ�plot��

		mBedVelocity = new QwtPlotCurve(QString::fromLocal8Bit("���ƴ�"));
		mBedVelocity->setStyle(QwtPlotCurve::Lines);//ֱ����ʽ
		QwtSplineCurveFitter *fitterBed = new QwtSplineCurveFitter();
		fitterBed->setSplineSize(50);
		mBedVelocity->setCurveFitter(fitterBed);//�������߲�ֵ
		mBedVelocity->setRenderHint(QwtPlotItem::RenderAntialiased, true);
		mBedVelocity->setCurveAttribute(QwtPlotCurve::Fitted, false);//ʹ���߸��⻬
		mBedVelocity->setPen(QPen(Qt::green, 1));//���û���
		mBedVelocity->attach(this);//�����߸��ӵ�plot��

		connect(legend, &QwtLegend::checked, this, &DegreeVelocityPlot::legendChecked);

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
				qobject_cast<QwtLegendLabel *>(legendWidgets[0]);

			if (legendLabel)
				legendLabel->setChecked(on);
		}
		this->replot();
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

	void setGantryUpdateFlag();
	void setCollimatorUpdateFlag();
	void setBedDegreeUpdateFlag();
	void setBedDistanceUpdateFlag();
	bool getGantryUpdateFlag();
	bool getCollimatorUpdateFlag();
	bool getBedDegreeUpdateFlag();
	bool getBedDistanceUpdatFlag();
signals:

public slots :
	void updateGantryDegreeDistance(const float y);
	void updateGantryDegreeVelocity(const float y);
	void updateCollimatorDegreeDistance(const float y);
	void updateCollimatorDegreeVelocity(const float y);
	void updateBedDegreeDistance(const float y);
	void updateBedDegreeVelocity(const float y);
	void updateBedDistance(const float x, const float y, const float z);
	void updateBedVelocity(const float x, const float y, const float z);
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

