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

void PlotView::resetPlot()
{
	mUpdateFlag = 0;
	stopTimer();
	clearCurveDataVector();

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
void PlotView::updateGantryDegree(const float y)
{
	setGantryUpdateFlag();

	mGantryDegree->append(y);
	updateGantryDegreeVelocity();

	if (!mTimer->isActive())
		mTimer->start();
}
void PlotView::updateGantryDegreeVelocity()
{
	int size = mGantryDegree->size();

	if (size > 1){
		const int cur_val = (int)(mGantryDegree->at(size - 1));//截断小数点后的角度变动
		const int last_val = (int)(mGantryDegree->at(size - 2));
		static int last_time = 0;
		int current_time = QTime::currentTime().msec();
		int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
		float v = (float)(cur_val - last_val) / delt_time;

		last_time = current_time;
		v *= 1000;//degree/s
		mGantryDegreeVelocity->append(v);

	}else{
		return;
	}
}
double PlotView::getGantryAvrDegreeVelocity()
{
	QMutexLocker lock(&mCurveDataMutex);//此时所有曲线都不能读写直到该函数退出
	int size = mGantryDegree->size();
	if (size > 0){
		double sum = 0;
		for (int i = 0; i < size; i++){
			sum += mGantryDegree->at(i);
		}
		return sum / size;
	}else{
		return 0;
	}
}
void PlotView::updateCollimatorDegree(const float y)
{
	setCollimatorUpdateFlag();

	mCollimatorDegree->append(y);
	updateCollimatorDegreeVelocity();

	if (!mTimer->isActive())
		mTimer->start();
}
void PlotView::updateCollimatorDegreeVelocity()
{
	int size = mCollimatorDegree->size();

	if (size > 1){
		const int cur_val = (int)(mCollimatorDegree->at(size - 1));//截断小数点后的角度变动
		const int last_val = (int)(mCollimatorDegree->at(size - 2));
		static int last_time = 0;
		int current_time = QTime::currentTime().msec();
		int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
		float v = (float)(cur_val - last_val) / delt_time;

		last_time = current_time;
		v *= 1000;//degree/s
		mCollimatorDegreeVelocity->append(v);

	}
	else{
		return;
	}
}
double PlotView::getCollimatorAvrDegreeVelocity()
{
	QMutexLocker lock(&mCurveDataMutex);//此时所有曲线都不能读写直到该函数退出
	int size = mCollimatorDegree->size();
	if (size > 0){
		double sum = 0;
		for (int i = 0; i < size; i++){
			sum += mCollimatorDegree->at(i);
		}
		return sum / size;
	}else{
		return 0;
	}
}
void PlotView::updateBedDegree(const float y)
{
	setBedDegreeUpdateFlag();

	mBedDegree->append(y);
	updateBedDegreeVelocity();

	if (!mTimer->isActive())
		mTimer->start();
}
void PlotView::updateBedDegreeVelocity()
{
	int size = mBedDegree->size();

	if (size > 1){
		const int cur_val = (int)(mBedDegree->at(size - 1));//截断小数点后的角度变动
		const int last_val = (int)(mBedDegree->at(size - 2));
		static int last_time = 0;
		int current_time = QTime::currentTime().msec();
		int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
		float v = (float)(cur_val - last_val) / delt_time;

		last_time = current_time;
		v *= 1000;//degree/s
		mBedDegreeVelocity->append(v);

	}else{
		return;
	}
}
double PlotView::getBedAvrDegreeVelocity()
{
	QMutexLocker lock(&mCurveDataMutex);//此时所有曲线都不能读写直到该函数退出
	int size = mBedDegree->size();
	if (size > 0){
		double sum = 0;
		for (int i = 0; i < size; i++){
			sum += mBedDegree->at(i);
		}
		return sum / size;
	}else{
		return 0;
	}
}
void PlotView::updateBedDistance(const float x, const float y, const float z)
{
	setBedDistanceUpdateFlag();

	mBedXDistance->append(x);
	mBedYDistance->append(y);
	mBedZDistance->append(z);
	updateBedDistanceVelocity();

	if (!mTimer->isActive())
		mTimer->start();
}
void PlotView::updateBedDistanceVelocity()
{
	int size_x = mBedXDistance->size();
	int size_y = mBedYDistance->size();
	int size_z = mBedZDistance->size();

	if (size_x > 1 && size_y > 1 && size_z > 1){
		const int current_x = (int)(mBedXDistance->at(size_x - 1));
		const int current_y = (int)(mBedYDistance->at(size_y - 1));
		const int current_z = (int)(mBedZDistance->at(size_z - 1));
		const int last_x = (int)(mBedXDistance->at(size_x - 2));
		const int last_y = (int)(mBedYDistance->at(size_y - 2));
		const int last_z = (int)(mBedZDistance->at(size_z - 2));

		static int last_time = 0;
		int current_time = QTime::currentTime().msec();
		int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);

		float v_x = (float)(current_x - last_x) / delt_time;
		float v_y = (float)(current_y - last_y) / delt_time;
		float v_z = (float)(current_z - last_z) / delt_time;

		last_time = current_time;

		v_x *= 1000;//mm/s
		v_y *= 1000;
		v_z *= 1000;
		mBedXVelocity->append(v_x);
		mBedYVelocity->append(v_y);
		mBedZVelocity->append(v_z);
	}else{
		return;
	}
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

	distancePlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXDistance);
	distancePlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYDistance);
	distancePlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZDistance);

	velocityPlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXVelocity);
	velocityPlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYVelocity);
	velocityPlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZVelocity);

	degreeDistancePlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegree);
	degreeDistancePlot->updateSample(CURVE_BED,    mDegreeTimeData, *mBedDegree);
	degreeDistancePlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegree);

	degreeVelocityPlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegreeVelocity);
	degreeVelocityPlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegreeVelocity);
	degreeVelocityPlot->updateSample(CURVE_BED, mDegreeTimeData, *mBedDegreeVelocity);
}

void PlotView::stopTimer()
{
	mTimer->stop();
}
