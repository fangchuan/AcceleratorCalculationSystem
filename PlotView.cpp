#include "plotview.h"
#include "QTextCodec"
#include <QTime>
#include <QDebug>

PlotView::PlotView(QWidget *parent) : QWidget(parent),
degreeVelocityPlot(NULL),
degreeDistancePlot(NULL),
distancePlot(NULL),
velocityPlot(NULL),
mUpdateFlag(0)
{
	initUi();
	initCurveDataVector();
	//set timer T = 50ms
	timer_id = startTimer(50);
	
}

PlotView::~PlotView()
{
	//    qDebug()<<"enter PlotView deconstructor";
}
void PlotView::setGantryUpdateFlag()
{
	mUpdateFlag = UPDATE_GANTRY_DEGREE;
}
void PlotView::setCollimatorUpdateFlag()
{
	mUpdateFlag = UPDATE_COLLIMATOR_DEGREE;
}
void PlotView::setBedDegreeUpdateFlag()
{
	mUpdateFlag = UPDATE_BED_DEGREE;
}
void PlotView::setBedDistanceUpdateFlag()
{
	mUpdateFlag = UPDATE_BED_DISTANCE;
}
bool PlotView::getGantryUpdateFlag()
{
	return mUpdateFlag == UPDATE_GANTRY_DEGREE;
}
bool PlotView::getCollimatorUpdateFlag()
{
	return mUpdateFlag == UPDATE_COLLIMATOR_DEGREE;
}
bool PlotView::getBedDegreeUpdateFlag()
{
	return mUpdateFlag == UPDATE_BED_DEGREE;
}
bool PlotView::getBedDistanceUpdatFlag()
{
	return mUpdateFlag == UPDATE_BED_DISTANCE;
}
void PlotView::updateGantryDegreeDistance(const double y)
{
	mGantryDegree->append(y);
}

void PlotView::updateGantryDegreeVelocity(const double y)
{
	static float last_y = (float)y;
	static int last_time = 0;
	int current_time = QTime::currentTime().msec();
	float v = (y - last_y) / (current_time - last_time);

	last_time = current_time;
	last_y = y;
	v *= 0.001;//degree/s
	mGantryDegreeVelocity->append(v);
}

void PlotView::updateCollimatorDegreeDistance(const double y)
{
	mCollimatorDegree->append(y);
}

void PlotView::updateCollimatorDegreeVelocity(const double y)
{
	static float last_y = (float)y;
	static int last_time = 0;
	int current_time = QTime::currentTime().msec();
	float v = (y - last_y) / (current_time - last_time);

	last_time = current_time;
	last_y = y;
	v *= 0.001;//degree/s
	mCollimatorDegreeVelocity->append(v);
}

void PlotView::updateBedDegreeDistance(const double y)
{
	mBedDegree->append(y);
}

void PlotView::updateBedDegreeVelocity(const double y)
{
	static float last_y = (float)y;
	static int last_time = 0;
	int current_time = QTime::currentTime().msec();
	float v = (y - last_y) / (current_time - last_time);

	last_time = current_time;
	last_y = y;
	v *= 0.001;//degree/s
	mBedDegreeVelocity->append(v);
}
void PlotView::updateBedDistance(const double x, const double y, const double z)
{
	mBedXDistance->append(x);
	mBedYDistance->append(y);
	mBedZDistance->append(z);

}

void PlotView::updateBedVelocity(const double x, const double y, const double z)
{
	static float last_x = x;
	static float last_y = y;
	static float last_z = z;

	static int last_time = 0;
	int current_time = QTime::currentTime().msec();

	float v_x = (x - last_x) / (current_time - last_time);
	float v_y = (y - last_y) / (current_time - last_time);
	float v_z = (z - last_z) / (current_time - last_time);

	last_time = current_time;
	last_x = x;
	last_y = y;
	last_z = z;

	v_x *= 0.001;//mm/s
	v_y *= 0.001;
	v_z *= 0.001;
	mBedXVelocity->append(v_x);
	mBedYVelocity->append(v_y);
	mBedZVelocity->append(v_z);
}

void PlotView::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);
	static double time = 0;

	time += 0.01;
	mTimeData.append(time);


	if (NULL != distancePlot){
		if (!mBedXDistance->isEmpty())
			distancePlot->updateSample(CURVE_X, mTimeData, *mBedXDistance);
		if (!mBedYDistance->isEmpty())
			distancePlot->updateSample(CURVE_Y, mTimeData, *mBedYDistance);
		if(!mBedZDistance->isEmpty())
			distancePlot->updateSample(CURVE_Z, mTimeData, *mBedZDistance);
	}
	if (NULL != velocityPlot){
		if (!mBedXVelocity->isEmpty())
			velocityPlot->updateSample(CURVE_X, mTimeData, *mBedXVelocity);
		if(!mBedYVelocity->isEmpty())
			velocityPlot->updateSample(CURVE_Y, mTimeData, *mBedYVelocity);
		if(!mBedZVelocity->isEmpty())
			velocityPlot->updateSample(CURVE_Z, mTimeData, *mBedZVelocity);
	}
	if (NULL != degreeDistancePlot){
		if (!mGantryDegree->isEmpty())
			degreeDistancePlot->updateSample(CURVE_GANTRY, mTimeData, *mGantryDegree);
		if(!mCollimatorDegree->isEmpty())
			degreeDistancePlot->updateSample(CURVE_COLLIMATOR, mTimeData, *mCollimatorDegree);
		if (!mBedDegree->isEmpty())
			degreeDistancePlot->updateSample(CURVE_BED, mTimeData, *mBedDegree);
	}
	if (NULL != degreeVelocityPlot){
		if (!mGantryDegreeVelocity->isEmpty())
			degreeDistancePlot->updateSample(CURVE_GANTRY, mTimeData, *mGantryDegreeVelocity);
		if (!mCollimatorDegreeVelocity->isEmpty())
			degreeDistancePlot->updateSample(CURVE_COLLIMATOR, mTimeData, *mCollimatorDegreeVelocity);
		if (!mBedDegreeVelocity->isEmpty())
			degreeDistancePlot->updateSample(CURVE_BED, mTimeData, *mBedDegreeVelocity);
	}
}

void PlotView::closeEvent(QCloseEvent *event)
{

	Q_UNUSED(event);

	killTimer(timer_id);

	clearCurveDataVector();

}

void PlotView::initUi()
{
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

	QHBoxLayout * hLayout1 = new QHBoxLayout();
	degreeDistancePlot = new DegreeDistancePlot();
	degreeVelocityPlot = new DegreeVelocityPlot();
	hLayout1->addWidget((QWidget*)degreeDistancePlot);
	hLayout1->addWidget((QWidget*)degreeVelocityPlot);
	hLayout1->setSpacing(30);

	QHBoxLayout* hLayout2 = new QHBoxLayout();
	distancePlot = new DistancePlot();
	velocityPlot = new VelocityPlot();
	hLayout2->addWidget((QWidget*)distancePlot);
	hLayout2->addWidget((QWidget*)velocityPlot);
	hLayout2->setSpacing(30);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->addLayout(hLayout1);
	vLayout->addLayout(hLayout2);
	vLayout->setSpacing(60);

}

void PlotView::initCurveDataVector()
{
	mBedXDistance = new QVector<double>();
	mBedYDistance = new QVector<double>();
	mBedZDistance = new QVector<double>();
	mBedXVelocity = new QVector<double>();
	mBedYVelocity = new QVector<double>();
	mBedZVelocity = new QVector<double>();
	mGantryDegree = new QVector<double>();
	mGantryDegreeVelocity = new QVector<double>();
	mCollimatorDegree = new QVector<double>();
	mCollimatorDegreeVelocity = new QVector<double>();
	mBedDegree = new QVector<double>();
	mBedDegreeVelocity = new QVector<double>();
}

void PlotView::clearCurveDataVector()
{
	mTimeData.clear();
	mBedXDistance->clear();
	mBedYDistance->clear();
	mBedZDistance->clear();
	mBedXVelocity->clear();
	mBedYVelocity->clear();
	mBedZVelocity->clear();
	mGantryDegree->clear();
	mGantryDegreeVelocity->clear();
	mCollimatorDegree->clear();
	mCollimatorDegreeVelocity->clear();
}
