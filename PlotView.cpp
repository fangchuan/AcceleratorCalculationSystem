#include "plotview.h"
#include <qwt_plot_renderer.h>
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
	mPayloadBedXDistance = new QVector<double>();
	mBedYDistance = new QVector<double>();
	mPayloadBedYDistance = new QVector<double>();
	mBedZDistance = new QVector<double>();
	mPayloadBedZDistance = new QVector<double>();
	mBedXVelocity = new QVector<double>();
	mPayloadBedXVelocity = new QVector<double>();
	mBedYVelocity = new QVector<double>();
	mPayloadBedYVelocity = new QVector<double>();
	mBedZVelocity = new QVector<double>();
	mPayloadBedZVelocity = new QVector<double>();
	mGantryDegree = new QVector<double>();
	mGantryDegreeVelocity = new QVector<double>();
	mCollimatorDegree = new QVector<double>();
	mCollimatorDegreeVelocity = new QVector<double>();
	mCbctDegree = new QVector<double>();
	mCbctDegreeVelocity = new QVector<double>();
	mBedDegree = new QVector<double>();
	mBedDegreeVelocity = new QVector<double>();
	mPayloadBedDegree = new QVector<double>();
	mPayloadBedDegreeVelocity = new QVector<double>();
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
void PlotView::setCBCTUpdateFlag()
{
	mUpdateFlag = UPDATE_CBCT_DEGREE;
}
void PlotView::setEmptyBedDegreeUpdateFlag()
{
	mUpdateFlag = UPDATE_BED_EMPTY_DEGREE;
}
void PlotView::setEmptyBedDistanceUpdateFlag()
{
	mUpdateFlag = UPDATE_BED_EMPTY_DISTANCE;
}
void PlotView::setPayloadBedDegreeUpdateFlag()
{
	mUpdateFlag = UPDATE_BED_PAYLOAD_DEGREE;
}
void PlotView::setPayloadBedDistanceUpdateFlag()
{
	mUpdateFlag = UPDATE_BED_PAYLOAD_DISTANCE;
}
bool PlotView::getGantryUpdateFlag()
{
	return mUpdateFlag == UPDATE_GANTRY_DEGREE;
}
bool PlotView::getCollimatorUpdateFlag()
{
	return mUpdateFlag == UPDATE_COLLIMATOR_DEGREE;
}
bool PlotView::getCBCTUpdateFlag()
{
	return mUpdateFlag == UPDATE_CBCT_DEGREE;
}
int PlotView::getBedDegreeUpdateFlag()
{
	return mUpdateFlag;
}
int PlotView::getBedDistanceUpdatFlag()
{
	return mUpdateFlag;
}


void PlotView::exportDistancePlot()
{
	QwtPlotRenderer render;
	QString str = QString("bed_displacement_plot_");
	QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	str += current_date;
	render.exportTo(distancePlot, str);
}

void PlotView::exportDistanceVelPlot()
{
	QwtPlotRenderer render;
	QString str = QString("bed_displacement_velocity_plot_");
	QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	str += current_date;
	render.exportTo(velocityPlot, str);
}

void PlotView::exportDegreePlot()
{
	QwtPlotRenderer render;
	QString str = QString("degree_displacement_plot_");
	QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	str += current_date;
	render.exportTo(degreeDistancePlot, str);
}

void PlotView::exportDegreeVelPlot()
{
	QwtPlotRenderer render;
	QString str = QString("degree_velocity_plot_");
	QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	str += current_date;
	render.exportTo(degreeVelocityPlot, str);
}

void PlotView::updateGantryDegree(const float y)
{
	if (getGantryUpdateFlag()) {

		mGantryDegree->append(y);
		updateGantryDegreeVelocity();

		if (!mTimer->isActive())
			mTimer->start();
	}
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
	if (getCollimatorUpdateFlag()) {

		mCollimatorDegree->append(y);
		updateCollimatorDegreeVelocity();

		if (!mTimer->isActive())
			mTimer->start();
	}
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
void PlotView::updateCBCTDegree(const float y)
{
	if (getCBCTUpdateFlag()) {

		mCbctDegree->append(y);
		updateCBCTDegreeVelocity();

		if (!mTimer->isActive())
			mTimer->start();
	}
}
void PlotView::updateCBCTDegreeVelocity()
{
	int size = mCbctDegree->size();

	if (size > 1) {
		const int cur_val = (int)(mCbctDegree->at(size - 1));//截断小数点后的角度变动
		const int last_val = (int)(mCbctDegree->at(size - 2));
		static int last_time = 0;
		int current_time = QTime::currentTime().msec();
		int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
		float v = (float)(cur_val - last_val) / delt_time;

		last_time = current_time;
		v *= 1000;//degree/s
		mCbctDegreeVelocity->append(v);

	}
	else {
		return;
	}
}
double PlotView::getCBCTAvrDegreeVelocity()
{
	QMutexLocker lock(&mCurveDataMutex);//此时所有曲线都不能读写直到该函数退出
	int size = mCbctDegree->size();
	if (size > 0) {
		double sum = 0;
		for (int i = 0; i < size; i++) {
			sum += mCbctDegree->at(i);
		}
		return sum / size;
	}
	else {
		return 0;
	}
}
//治疗床运动曲线分为 空载旋转、空载移动、负载旋转、负载移动
void PlotView::updateBedDegree(const float y)
{
	int flag = getBedDegreeUpdateFlag();
	if (flag == UPDATE_BED_EMPTY_DEGREE) {
		mBedDegree->append(y);
	}else{
		if (flag == UPDATE_BED_PAYLOAD_DEGREE)
			mPayloadBedDegree->append(y);
		else
			return;
	}
	updateBedDegreeVelocity();

	if (!mTimer->isActive())
		mTimer->start();
}
void PlotView::updateBedDegreeVelocity()
{
	int updateFlag = getBedDegreeUpdateFlag();
	if (updateFlag == UPDATE_BED_EMPTY_DEGREE) {
		int size = mBedDegree->size();

		if (size > 1) {
			const int cur_val = (int)(mBedDegree->at(size - 1));//截断小数点后的角度变动
			const int last_val = (int)(mBedDegree->at(size - 2));
			static int last_time = 0;
			int current_time = QTime::currentTime().msec();
			int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
			float v = (float)(cur_val - last_val) / delt_time;

			last_time = current_time;
			v *= 1000;//degree/s
			mBedDegreeVelocity->append(v);

		}
		else {
			return;
		}
	}
	else {
		if (updateFlag == UPDATE_BED_PAYLOAD_DEGREE) {
			int size = mPayloadBedDegree->size();

			if (size > 1) {
				const int cur_val = (int)(mPayloadBedDegree->at(size - 1));//截断小数点后的角度变动
				const int last_val = (int)(mPayloadBedDegree->at(size - 2));
				static int last_time = 0;
				int current_time = QTime::currentTime().msec();
				int delt_time = (current_time - last_time) < 0 ? (current_time - last_time + 1000) : (current_time - last_time);
				float v = (float)(cur_val - last_val) / delt_time;

				last_time = current_time;
				v *= 1000;//degree/s
				mPayloadBedDegreeVelocity->append(v);

			}
			else {
				return;
			}
		}
		else {
			return;
		}
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
	int flag = getBedDistanceUpdatFlag();
	if (flag == UPDATE_BED_EMPTY_DISTANCE) {
		mBedXDistance->append(x);
		mBedYDistance->append(y);
		mBedZDistance->append(z);
	}
	else {
		if (flag == UPDATE_BED_PAYLOAD_DISTANCE)
		{
			mPayloadBedXDistance->append(x);
			mPayloadBedYDistance->append(y);
			mPayloadBedZDistance->append(z);
		}
		else
		{
			return;
		}
	}

	updateBedDistanceVelocity();

	if (!mTimer->isActive())
		mTimer->start();
}
void PlotView::updateBedDistanceVelocity()
{
	int flag = getBedDistanceUpdatFlag();
	if (flag == UPDATE_BED_EMPTY_DISTANCE) {
		int size_x = mBedXDistance->size();
		int size_y = mBedYDistance->size();
		int size_z = mBedZDistance->size();

		if (size_x > 1 && size_y > 1 && size_z > 1) {
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
		}
		else {
			return;
		}
	}
	else {
		if (flag == UPDATE_BED_PAYLOAD_DISTANCE)
		{
			int size_x = mPayloadBedXDistance->size();
			int size_y = mPayloadBedYDistance->size();
			int size_z = mPayloadBedZDistance->size();

			if (size_x > 1 && size_y > 1 && size_z > 1) {
				const int current_x = (int)(mPayloadBedXDistance->at(size_x - 1));
				const int current_y = (int)(mPayloadBedYDistance->at(size_y - 1));
				const int current_z = (int)(mPayloadBedZDistance->at(size_z - 1));
				const int last_x = (int)(mPayloadBedXDistance->at(size_x - 2));
				const int last_y = (int)(mPayloadBedYDistance->at(size_y - 2));
				const int last_z = (int)(mPayloadBedZDistance->at(size_z - 2));

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
				mPayloadBedXVelocity->append(v_x);
				mPayloadBedYVelocity->append(v_y);
				mPayloadBedZVelocity->append(v_z);
			}
			else {
				return;
			}
		}
		else
		{
			return;
		}
	}
}
void PlotView::update()
{
	static double degree_time = 0;
	static double distance_time = 0;

	{
		if (getBedDistanceUpdatFlag() == UPDATE_BED_EMPTY_DISTANCE){
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

			if (getBedDistanceUpdatFlag() == UPDATE_BED_PAYLOAD_DISTANCE){
				distance_time += 0.05;
				mDistanceTimeData.append(distance_time);

				distancePlot->updateSample(CURVE_X_PAYLOAD, mDistanceTimeData, *mPayloadBedXDistance);
				distancePlot->updateSample(CURVE_Y_PAYLOAD, mDistanceTimeData, *mPayloadBedYDistance);
				distancePlot->updateSample(CURVE_Z_PAYLOAD, mDistanceTimeData, *mPayloadBedZDistance);

				velocityPlot->updateSample(CURVE_X_PAYLOAD, mDistanceTimeData, *mPayloadBedXVelocity);
				velocityPlot->updateSample(CURVE_Y_PAYLOAD, mDistanceTimeData, *mPayloadBedYVelocity);
				velocityPlot->updateSample(CURVE_Z_PAYLOAD, mDistanceTimeData, *mPayloadBedZVelocity);
			}
			else {
				if (!mPayloadBedXDistance->isEmpty() || !mPayloadBedYDistance->isEmpty() || !mPayloadBedZDistance->isEmpty()) {
					distance_time += 0.05;
					mDistanceTimeData.append(distance_time);
				}
				if (!mPayloadBedXDistance->isEmpty()) {
					double end = mPayloadBedXDistance->last();
					mPayloadBedXDistance->append(end);
					distancePlot->updateSample(CURVE_X_PAYLOAD, mDistanceTimeData, *mPayloadBedXDistance);
				}
				if (!mPayloadBedYDistance->isEmpty()) {
					double end = mPayloadBedYDistance->last();
					mPayloadBedYDistance->append(end);
					distancePlot->updateSample(CURVE_Y_PAYLOAD, mDistanceTimeData, *mPayloadBedYDistance);
				}
				if (!mPayloadBedZDistance->isEmpty()) {
					double end = mPayloadBedZDistance->last();
					mPayloadBedZDistance->append(end);
					distancePlot->updateSample(CURVE_Z_PAYLOAD, mDistanceTimeData, *mPayloadBedZDistance);
				}
				if (!mPayloadBedXVelocity->isEmpty()) {
					double end = mPayloadBedXVelocity->last();
					mPayloadBedXVelocity->append(end);
					velocityPlot->updateSample(CURVE_X_PAYLOAD, mDistanceTimeData, *mPayloadBedXVelocity);
				}
				if (!mPayloadBedYVelocity->isEmpty()) {
					double end = mPayloadBedYVelocity->last();
					mPayloadBedYVelocity->append(end);
					velocityPlot->updateSample(CURVE_Y_PAYLOAD, mDistanceTimeData, *mPayloadBedYVelocity);
				}
				if (!mPayloadBedZVelocity->isEmpty()) {
					double end = mPayloadBedZVelocity->last();
					mPayloadBedZVelocity->append(end);
					velocityPlot->updateSample(CURVE_Z_PAYLOAD, mDistanceTimeData, *mPayloadBedZVelocity);
				}
			}
		}
	}

	if (getGantryUpdateFlag() || getCollimatorUpdateFlag() || getCBCTUpdateFlag() || getBedDegreeUpdateFlag()){

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
		if (getCBCTUpdateFlag()) {
			degreeDistancePlot->updateSample(CURVE_CBCT, mDegreeTimeData, *mCbctDegree);
			degreeVelocityPlot->updateSample(CURVE_CBCT, mDegreeTimeData, *mCbctDegreeVelocity);
		}
		else {
			if (!mCbctDegree->isEmpty()) {
				double end = mCbctDegree->last();
				mCbctDegree->append(end);
				degreeDistancePlot->updateSample(CURVE_CBCT, mDegreeTimeData, *mCbctDegree);
			}
			if (!mCbctDegreeVelocity->isEmpty()) {
				double end = mCbctDegreeVelocity->last();
				mCbctDegreeVelocity->append(end);
				degreeVelocityPlot->updateSample(CURVE_CBCT, mDegreeTimeData, *mCbctDegreeVelocity);
			}
		}
		if (getBedDegreeUpdateFlag() == UPDATE_BED_EMPTY_DEGREE){
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

		if (getBedDegreeUpdateFlag() == UPDATE_BED_PAYLOAD_DEGREE) {
			degreeDistancePlot->updateSample(CURVE_BED_PAYLOAD, mDegreeTimeData, *mPayloadBedDegree);
			degreeVelocityPlot->updateSample(CURVE_BED_PAYLOAD, mDegreeTimeData, *mPayloadBedDegreeVelocity);
		}
		else {
			if (!mBedDegree->isEmpty()) {
				double end = mBedDegree->last();
				mBedDegree->append(end);
				degreeDistancePlot->updateSample(CURVE_BED_PAYLOAD, mDegreeTimeData, *mPayloadBedDegree);
			}
			if (!mBedDegreeVelocity->isEmpty()) {
				double end = mBedDegreeVelocity->last();
				mBedDegreeVelocity->append(end);
				degreeVelocityPlot->updateSample(CURVE_BED_PAYLOAD, mDegreeTimeData, *mPayloadBedDegreeVelocity);
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
	mPayloadBedXDistance->clear();
	mBedYDistance->clear();
	mPayloadBedYDistance->clear();
	mBedZDistance->clear();
	mPayloadBedZDistance->clear();
	mBedXVelocity->clear();
	mPayloadBedXVelocity->clear();
	mBedYVelocity->clear();
	mPayloadBedYVelocity->clear();
	mBedZVelocity->clear();
	mPayloadBedZVelocity->clear();
	mGantryDegree->clear();
	mGantryDegreeVelocity->clear();
	mCollimatorDegree->clear();
	mCollimatorDegreeVelocity->clear();
	mCbctDegree->clear();
	mCbctDegreeVelocity->clear();
	mBedDegree->clear();
	mBedDegreeVelocity->clear();
	mPayloadBedDegree->clear();
	mPayloadBedDegreeVelocity->clear();

	distancePlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXDistance);
	distancePlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYDistance);
	distancePlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZDistance);
	distancePlot->updateSample(CURVE_X_PAYLOAD, mDistanceTimeData, *mPayloadBedXDistance);
	distancePlot->updateSample(CURVE_Y_PAYLOAD, mDistanceTimeData, *mPayloadBedYDistance);
	distancePlot->updateSample(CURVE_Z_PAYLOAD, mDistanceTimeData, *mPayloadBedZDistance);

	velocityPlot->updateSample(CURVE_X, mDistanceTimeData, *mBedXVelocity);
	velocityPlot->updateSample(CURVE_Y, mDistanceTimeData, *mBedYVelocity);
	velocityPlot->updateSample(CURVE_Z, mDistanceTimeData, *mBedZVelocity);
	velocityPlot->updateSample(CURVE_X_PAYLOAD, mDistanceTimeData, *mPayloadBedXVelocity);
	velocityPlot->updateSample(CURVE_Y_PAYLOAD, mDistanceTimeData, *mPayloadBedYVelocity);
	velocityPlot->updateSample(CURVE_Z_PAYLOAD, mDistanceTimeData, *mPayloadBedZVelocity);

	degreeDistancePlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegree);
	degreeDistancePlot->updateSample(CURVE_BED,    mDegreeTimeData, *mBedDegree);
	degreeDistancePlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegree);
	degreeDistancePlot->updateSample(CURVE_CBCT, mDegreeTimeData, *mCbctDegree);
	degreeDistancePlot->updateSample(CURVE_BED_PAYLOAD, mDegreeTimeData, *mPayloadBedDegree);


	degreeVelocityPlot->updateSample(CURVE_GANTRY, mDegreeTimeData, *mGantryDegreeVelocity);
	degreeVelocityPlot->updateSample(CURVE_COLLIMATOR, mDegreeTimeData, *mCollimatorDegreeVelocity);
	degreeVelocityPlot->updateSample(CURVE_BED, mDegreeTimeData, *mBedDegreeVelocity);
	degreeDistancePlot->updateSample(CURVE_CBCT, mDegreeTimeData, *mCbctDegreeVelocity);
	degreeDistancePlot->updateSample(CURVE_BED_PAYLOAD, mDegreeTimeData, *mPayloadBedDegreeVelocity);


}

void PlotView::stopTimer()
{
	mTimer->stop();
}
