#include "plotview.h"
#include "QTextCodec"
#include <QTime>
#include <QTimerEvent>
#include <QDebug>

PlotView::PlotView(QWidget *parent) : QWidget(parent),
degreeVelocityPlot(NULL),
degreeDistancePlot(NULL),
distancePlot(NULL),
velocityPlot(NULL),
mCurveDataMutex(QMutex::Recursive),
mUpdateFlag(0)
{
	initUi();
	initCurveDataVector();
	initTimer();
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
void PlotView::updateGantryDegreeDistance(const float y)
{
	mGantryDegree->append(y);
	setGantryUpdateFlag();

	if (!mTimer->isActive())
		mTimer->start();

}

void PlotView::updateGantryDegreeVelocity(const float y)
{
	static int last_y = (int)y;//截断小数点后的角度变动
	static int last_time = 0;
	int current_time = QTime::currentTime().msec();
	int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
	float v = (float)(y - last_y) / delt_time;

	last_time = current_time;
	last_y = (int)y;
	v *= 1000;//degree/s
	mGantryDegreeVelocity->append(v);
}

void PlotView::updateCollimatorDegreeDistance(const float y)
{
	mCollimatorDegree->append(y);
	setCollimatorUpdateFlag();

	if (!mTimer->isActive())
		mTimer->start();
}

void PlotView::updateCollimatorDegreeVelocity(const float y)
{
	static int last_y = (int)y;
	static int last_time = 0;
	int current_time = QTime::currentTime().msec();
	int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
	float v = (float)(y - last_y) / delt_time;

	last_time = current_time;
	last_y = (int)y;
	v *= 1000;//degree/s
	mCollimatorDegreeVelocity->append(v);
}

void PlotView::updateBedDegreeDistance(const float y)
{
	mBedDegree->append(y);
	setBedDegreeUpdateFlag();

	if (!mTimer->isActive())
		mTimer->start();
}

void PlotView::updateBedDegreeVelocity(const float y)
{
	static int last_y = (int)y;
	static int last_time = 0;
	int current_time = QTime::currentTime().msec();
	int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
	float v = (float)(y - last_y) / delt_time;

	last_time = current_time;
	last_y = (int)y;
	v *= 1000;//degree/s
	mBedDegreeVelocity->append(v);
}

void PlotView::updateBedDistance(const float x, const float y, const float z)
{
	mBedXDistance->append(x);
	mBedYDistance->append(y);
	mBedZDistance->append(z);
	setBedDistanceUpdateFlag();

	if (!mTimer->isActive())
		mTimer->start();
}

void PlotView::updateBedVelocity(const float x, const float y, const float z)
{
	static int last_x = (int)x;
	static int last_y = (int)y;
	static int last_z = (int)z;

	static int last_time = 0;
	int current_time = QTime::currentTime().msec();
	int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);

	float v_x = (x - last_x) / delt_time;
	float v_y = (y - last_y) / delt_time;
	float v_z = (z - last_z) / delt_time;

	last_time = current_time;
	last_x = (int)x;
	last_y = (int)y;
	last_z = (int)z;

	v_x *= 1000;//mm/s
	v_y *= 1000;
	v_z *= 1000;
	mBedXVelocity->append(v_x);
	mBedYVelocity->append(v_y);
	mBedZVelocity->append(v_z);
}

void PlotView::update()
{
	static double degree_time = 0;
	static double distance_time = 0;

	{
		if (getBedDistanceUpdatFlag()){
			distance_time += 0.05;
			mDistanceTimeData.append(distance_time);

			distancePlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXDistance);
			distancePlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYDistance);
			distancePlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZDistance);

			velocityPlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXVelocity);
			velocityPlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYVelocity);
			velocityPlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZVelocity);
		}else{
			if (!mBedXDistance->isEmpty() || !mBedYDistance->isEmpty() || !mBedZDistance->isEmpty()){
				distance_time += 0.05;
				mDistanceTimeData.append(distance_time);
			}
			if (!mBedXDistance->isEmpty()){
				double end = mBedXDistance->last();
				mBedXDistance->append(end);
				distancePlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXDistance);
			}
			if (!mBedYDistance->isEmpty()){
				double end = mBedYDistance->last();
				mBedYDistance->append(end);
				distancePlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYDistance);
			}
			if (!mBedZDistance->isEmpty()){
				double end = mBedZDistance->last();
				mBedZDistance->append(end);
				distancePlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZDistance);
			}
			if (!mBedXVelocity->isEmpty()){
				double end = mBedXVelocity->last();
				mBedXVelocity->append(end);
				velocityPlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXVelocity);
			}
			if (!mBedYVelocity->isEmpty()){
				double end = mBedYVelocity->last();
				mBedYVelocity->append(end);
				velocityPlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYVelocity);
			}
			if (!mBedZVelocity->isEmpty()){
				double end = mBedZVelocity->last();
				mBedZVelocity->append(end);
				velocityPlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZVelocity);
			}
		}
	}

	if (getGantryUpdateFlag() || getCollimatorUpdateFlag() || getBedDegreeUpdateFlag()){

		degree_time += 0.05;
		mDegreeTimeData.append(degree_time);
		if (getGantryUpdateFlag()){
			degreeDistancePlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegree);
			degreeVelocityPlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegreeVelocity);
		}else{
			if (!mGantryDegree->isEmpty()){
				double end = mGantryDegree->last();
				mGantryDegree->append(end);
				degreeDistancePlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegree);
			}
			if (!mGantryDegreeVelocity->isEmpty()){
				double end = mGantryDegreeVelocity->last();
				mGantryDegreeVelocity->append(end);
				degreeVelocityPlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegreeVelocity);
			}
		}
		if (getCollimatorUpdateFlag()){
			degreeDistancePlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegree);
			degreeVelocityPlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegreeVelocity);
		}else{
			if (!mCollimatorDegree->isEmpty()){
				double end = mCollimatorDegree->last();
				mCollimatorDegree->append(end);
				degreeDistancePlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegree);
			}
			if (!mCollimatorDegreeVelocity->isEmpty()){
				double end = mCollimatorDegreeVelocity->last();
				mCollimatorDegreeVelocity->append(end);
				degreeVelocityPlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegreeVelocity);
			}
		}
		if (getBedDegreeUpdateFlag()){
			degreeDistancePlot->updateSample(CURVE_BED, mDegreeTimeData, *mBedDegree);
			degreeVelocityPlot->updateSample(CURVE_BED, mDegreeTimeData, *mBedDegreeVelocity);
		}else{
			if (!mBedDegree->isEmpty()){
				double end = mBedDegree->last();
				mBedDegree->append(end);
				degreeDistancePlot->updateSample(CURVE_BED, mDegreeTimeData, *mBedDegree);
			}
			if (!mBedDegreeVelocity->isEmpty()){
				double end = mBedDegreeVelocity->last();
				mBedDegreeVelocity->append(end);
				degreeVelocityPlot->updateSample(CURVE_BED, mDegreeTimeData, *mBedDegreeVelocity);
			}
		}
	}
}

void PlotView::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event);
	stopTimer();
	clearCurveDataVector();
}

void PlotView::initUi()
{
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
	vLayout->setSpacing(45);

}

void PlotView::initTimer()
{
	//set timer T = 50ms
	mTimer = new QTimer(this);
	mTimer->setInterval(50);

	connect(mTimer, &QTimer::timeout, this, &PlotView::update);
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
	QMutexLocker locker(&mCurveDataMutex);

	mDegreeTimeData.clear();
	mDistanceTimeData.clear();
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
	mBedDegree->clear();
	mBedDegreeVelocity->clear();
}

void PlotView::stopTimer()
{
	mTimer->stop();
}
