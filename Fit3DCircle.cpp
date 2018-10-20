#include "Fit3DCircle.h"

#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkLandmarkTransform.h"
#include "vtkPlane.h"
#include "vtkMath.h"
#include "vtkSmartPointer.h"

#include "opencv2/opencv.hpp"
#include "opencv/cv.h"


// Class for Circle
// A circle has 7 fields: 
//     a, b, r (of type reals), the circle parameters
//     s (of type reals), the estimate of sigma (standard deviation)
//     g (of type reals), the norm of the gradient of the objective function
//     i and j (of type int), the iteration counters (outer and inner, respectively)
class Circle2D
{
public:

	// The fields of a Circle
	double a, b, r, s, g, Gx, Gy;
	int i, j;

	// constructors
	Circle2D();
	Circle2D(double aa, double bb, double rr);

	// routines
	void print(void);

	// no destructor we didn't allocate memory by hand.
};


/************************************************************************
BODY OF THE MEMBER ROUTINES
************************************************************************/
// Default constructor

Circle2D::Circle2D()
{
	a = 0.; b = 0.; r = 1.; s = 0.; i = 0; j = 0;
}

// Constructor with assignment of the circle parameters only

Circle2D::Circle2D(double aa, double bb, double rr)
{
	a = aa; b = bb; r = rr;
}

// Printing routine

void Circle2D::print(void)
{
	cout << endl;
	cout << setprecision(10) << "center (" << a << "," << b << ")  radius "
		<< r << "  sigma " << s << "  gradient " << g << "  iter " << i << "  inner " << j << endl;
}

// Class for Data
// A data has 5 fields: 
//       n (of type int), the number of data points 
//       X and Y (arrays of type reals), arrays of x- and y-coordinates
//       meanX and meanY (of type reals), coordinates of the centroid (x and y sample means)

class Data
{
public:

	int n;
	double *X;		//space is allocated in the constructors
	double *Y;		//space is allocated in the constructors
	double meanX, meanY;

	// constructors
	Data();
	Data(int N);
	Data(int N, double X[], double Y[]);

	// routines
	void means(void);
	void center(void);
	void scale(void);
	void print(void);

	// destructors
	~Data();
};


/************************************************************************
BODY OF THE MEMBER ROUTINES
************************************************************************/
// Default constructor
Data::Data()
{
	n = 0;
	X = new double[n];
	Y = new double[n];
	for (int i = 0; i<n; i++)
	{
		X[i] = 0.;
		Y[i] = 0.;
	}
}

// Constructor with assignment of the field N
Data::Data(int N)
{
	n = N;
	X = new double[n];
	Y = new double[n];

	for (int i = 0; i<n; i++)
	{
		X[i] = 0.;
		Y[i] = 0.;
	}
}

// Constructor with assignment of each field
Data::Data(int N, double dataX[], double dataY[])
{
	n = N;
	X = new double[n];
	Y = new double[n];

	for (int i = 0; i<n; i++)
	{
		X[i] = dataX[i];
		Y[i] = dataY[i];
	}
}

// Routine that computes the x- and y- sample means (the coordinates of the centeroid)

void Data::means(void)
{
	meanX = 0.; meanY = 0.;

	for (int i = 0; i<n; i++)
	{
		meanX += X[i];
		meanY += Y[i];
	}
	meanX /= n;
	meanY /= n;
}

// Routine that centers the data set (shifts the coordinates to the centeroid)

void Data::center(void)
{
	double sX = 0., sY = 0.;
	int i;

	for (i = 0; i<n; i++)
	{
		sX += X[i];
		sY += Y[i];
	}
	sX /= n;
	sY /= n;

	for (i = 0; i<n; i++)
	{
		X[i] -= sX;
		Y[i] -= sY;
	}
	meanX = 0.;
	meanY = 0.;
}

// Routine that scales the coordinates (makes them of order one)

void Data::scale(void)
{
	double sXX = 0., sYY = 0., scaling;
	int i;

	for (i = 0; i<n; i++)
	{
		sXX += X[i] * X[i];
		sYY += Y[i] * Y[i];
	}
	scaling = sqrt((sXX + sYY) / n / 2);

	for (i = 0; i<n; i++)
	{
		X[i] /= scaling;
		Y[i] /= scaling;
	}
}

// Printing routine

void Data::print(void)
{
	cout << endl << "The data set has " << n << " points with coordinates :" << endl;

	for (int i = 0; i<n - 1; i++)
		cout << setprecision(7) << "(" << X[i] << "," << Y[i] << "), ";

	cout << "(" << X[n - 1] << "," << Y[n - 1] << ")\n";
}

// Destructor
Data::~Data()
{
	delete[] X;
	delete[] Y;
}

template<typename T>
inline T SQR(T t)
{
	return t*t;
};

//****************** Sigma ************************************
//
//   estimate of Sigma = square root of RSS divided by N
//   gives the root-mean-square error of the geometric circle fit
double Sigma(Data& data, Circle2D& circle)
{
	double sum = 0., dx, dy;

	for (int i = 0; i<data.n; i++)
	{
		dx = data.X[i] - circle.a;
		dy = data.Y[i] - circle.b;
		sum += SQR(sqrt(dx*dx + dy*dy) - circle.r);
	}
	return sqrt(sum / data.n);
}

Circle2D CircleFitByHyper(Data& data)
/*
Circle fit to a given set of data points (in 2D)

This is an algebraic fit based on the journal article

A. Al-Sharadqah and N. Chernov, "Error analysis for circle fitting algorithms",
Electronic Journal of Statistics, Vol. 3, pages 886-911, (2009)

It is an algebraic circle fit with "hyperaccuracy" (with zero essential bias).
The term "hyperaccuracy" first appeared in papers by Kenichi Kanatani around 2006

Input:  data     - the class of data (contains the given points):

data.n   - the number of data points
data.X[] - the array of X-coordinates
data.Y[] - the array of Y-coordinates

Output:
circle - parameters of the fitting circle:

circle.a - the X-coordinate of the center of the fitting circle
circle.b - the Y-coordinate of the center of the fitting circle
circle.r - the radius of the fitting circle
circle.s - the root mean square error (the estimate of sigma)
circle.j - the total number of iterations

This method combines the Pratt and Taubin fits to eliminate the essential bias.

It works well whether data points are sampled along an entire circle or
along a small arc.

Its statistical accuracy is theoretically higher than that of the Pratt fit
and Taubin fit, but practically they all return almost identical circles
(unlike the Kasa fit that may be grossly inaccurate).

It provides a very good initial guess for a subsequent geometric fit.

Nikolai Chernov  (September 2012)

*/
{
	int i, iter, IterMAX = 99;

	double Xi, Yi, Zi;
	double Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Cov_xy, Var_z;
	double A0, A1, A2, A22;
	double Dy, xnew, x, ynew, y;
	double DET, Xcenter, Ycenter;

	Circle2D circle;

	data.means();   // Compute x- and y- sample means (via a function in the class "data") 

					//     computing moments 

	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (i = 0; i < data.n; i++)
	{
		Xi = data.X[i] - data.meanX;   //  centered x-coordinates
		Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
		Zi = Xi*Xi + Yi*Yi;

		Mxy += Xi*Yi;
		Mxx += Xi*Xi;
		Myy += Yi*Yi;
		Mxz += Xi*Zi;
		Myz += Yi*Zi;
		Mzz += Zi*Zi;
	}
	Mxx /= data.n;
	Myy /= data.n;
	Mxy /= data.n;
	Mxz /= data.n;
	Myz /= data.n;
	Mzz /= data.n;

	//    computing the coefficients of the characteristic polynomial

	Mz = Mxx + Myy;
	Cov_xy = Mxx*Myy - Mxy*Mxy;
	Var_z = Mzz - Mz*Mz;

	A2 = 4 * Cov_xy - 3 * Mz*Mz - Mzz;
	A1 = Var_z*Mz + 4 * Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
	A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
	A22 = A2 + A2;

	//    finding the root of the characteristic polynomial
	//    using Newton's method starting at x=0  
	//     (it is guaranteed to converge to the right root)

	for (x = 0., y = A0, iter = 0; iter < IterMAX; iter++)  // usually, 4-6 iterations are enough
	{
		Dy = A1 + x*(A22 + 16.*x*x);
		xnew = x - y / Dy;
		if ((xnew == x) || (!isfinite(xnew))) break;
		ynew = A0 + xnew*(A1 + xnew*(A2 + 4 * xnew*xnew));
		if (abs(ynew) >= abs(y))  break;
		x = xnew;  y = ynew;
	}

	//    computing paramters of the fitting circle

	DET = x*x - x*Mz + Cov_xy;
	Xcenter = (Mxz*(Myy - x) - Myz*Mxy) / DET / 2;
	Ycenter = (Myz*(Mxx - x) - Mxz*Mxy) / DET / 2;

	//       assembling the output

	circle.a = Xcenter + data.meanX;
	circle.b = Ycenter + data.meanY;
	circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz - x - x);
	circle.s = Sigma(data, circle);
	circle.i = 0;
	circle.j = iter;  //  return the number of iterations, too

	return circle;
}

Fit3DCircle::Fit3DCircle(QObject* parent): QObject(parent),
	m_Radius(-1),
	m_Valid(0)
{
	m_Normal[0] = m_Normal[1] = m_Normal[2] = 0;
	m_Center[0] = m_Center[1] = m_Center[2] = 0;
}

Fit3DCircle::~Fit3DCircle()
{

}

//Ax+by+cz=D
void cvFitPlane(const CvMat* points, double* plane) 
{
	// Estimate geometric centroid.  
	int nrows = points->rows;
	int ncols = points->cols;
	int type = points->type;
	CvMat* centroid = cvCreateMat(1, ncols, type);
	cvSet(centroid, cvScalar(0));
	for (int c = 0; c < ncols; c++) 
	{
		for (int r = 0; r < nrows; r++) 
		{
			centroid->data.fl[c] += points->data.fl[ncols*r + c];
		}
		centroid->data.fl[c] /= nrows;
	}
	// Subtract geometric centroid from each point.  
	CvMat* points2 = cvCreateMat(nrows, ncols, type);
	for (int r = 0; r < nrows; r++) {
		for (int c = 0; c < ncols; c++) {
			points2->data.fl[ncols*r + c] = points->data.fl[ncols*r + c] - centroid->data.fl[c];
		}
	}
	// Evaluate SVD of covariance matrix.  
	CvMat* A = cvCreateMat(ncols, ncols, type);
	CvMat* W = cvCreateMat(ncols, ncols, type);
	CvMat* V = cvCreateMat(ncols, ncols, type);
	cvGEMM(points2, points, 1, NULL, 0, A, CV_GEMM_A_T);
	cvSVD(A, W, NULL, V, CV_SVD_V_T);
	// Assign plane coefficients by singular vector corresponding to smallest singular value.  
	plane[ncols] = 0;
	for (int c = 0; c < ncols; c++) {
		plane[c] = V->data.fl[ncols*(ncols - 1) + c];
		plane[ncols] += plane[c] * centroid->data.fl[c];
	}
	// Release allocated resources.  
	cvReleaseMat(&centroid);
	cvReleaseMat(&points2);
	cvReleaseMat(&A);
	cvReleaseMat(&W);
	cvReleaseMat(&V);
}

void Fit3DCircle::addPoint(const MarkerPointType &point)
{
	m_Positions.push_back(point);
	if (m_Positions.size() <= 3)
	{
		m_Valid = 0;
		return;
	}

	if (!isValid())
	{
		if(fabs(point[0] - m_Positions.at(0)[0]) >= 40)
			calculate();
	}
	else
	{
		if (!isPointOnCircle(point))
			calculate();
	}
}

void Fit3DCircle::calculate()
{
	int size = m_Positions.size();
	CvMat *points_mat = cvCreateMat(size, 3, CV_32FC1);
	for (int i = 0; i < size; ++i)
	{
		const MarkerPointType& point = m_Positions.at(i);
		points_mat->data.fl[i * 3 + 0] = point[0];
		points_mat->data.fl[i * 3 + 1] = point[1];
		points_mat->data.fl[i * 3 + 2] = point[2];
	}
	double plane[4] = { 0, 0, 0, 0 };
	cvFitPlane(points_mat, plane);
	double A = plane[0], B = plane[1], C = plane[2], D = plane[3];
	cvReleaseMat(&points_mat);
	m_Normal[0] = A; m_Normal[1] = B; m_Normal[2] = C;
	//Ax+By+Cz=D
	double P1[3];
	double origin[3] = { 0, 0, 0 };
	if (fabs(C) > 1e-5) { // C != 0
		P1[0] = P1[1] = 0;
		P1[2] = D / C;
	}
	else { // C == 0
		if (fabs(B) > 1e-5) { // B != 0
			P1[0] = P1[2] = 0;
			P1[1] = D / B;
		}
		else { // B == 0
			P1[1] = P1[2] = 0;
			P1[0] = D / A;
		}
	}
	double projPoint[3];
	vtkPlane::ProjectPoint(origin, P1, plane, projPoint);
	vtkSmartPointer<vtkLandmarkTransform> transform = vtkSmartPointer<vtkLandmarkTransform>::New();
	vtkSmartPointer<vtkPoints> source = vtkSmartPointer<vtkPoints>::New();
	source->SetNumberOfPoints(3);
	vtkSmartPointer<vtkPoints> target = vtkSmartPointer<vtkPoints>::New();
	target->SetNumberOfPoints(3);
	transform->SetSourceLandmarks(source);
	transform->SetTargetLandmarks(target);
	transform->SetModeToRigidBody();
	double xPoint[3] = { 1, 0, 0 };
	double yPoint[3] = { 0, 1, 0 };
	double zPoint[3] = { 0, 0, 1 };
	source->SetPoint(0, xPoint);
	source->SetPoint(2, origin);
	double projxPoint[3];
	vtkPlane::ProjectPoint(xPoint, P1, plane, projxPoint);
	double xDistance = sqrt(vtkMath::Distance2BetweenPoints(projxPoint, projPoint));
	double t;
	double intersectPoint[3];
	if (vtkPlane::IntersectWithLine(origin, xPoint, plane, P1, t, intersectPoint) == 0) {
		// not intersect
		target->SetPoint(0, xDistance, 0, sqrt(vtkMath::Distance2BetweenPoints(xPoint, projxPoint)));
	}
	else {
		target->SetPoint(0, xDistance, 0, -sqrt(vtkMath::Distance2BetweenPoints(xPoint, projxPoint)));
	}
	double OO[3] = { origin[0] - projPoint[0], origin[1] - projPoint[1], origin[1] - projPoint[1] };
	double OX[3] = { 0, 1, 0 };
	double result[3];
	vtkMath::Cross(OO, OX, result);
	if (fabs(result[0]) < 1e-5 && fabs(result[1]) < 1e-5 && fabs(result[2]) < 1e-5) {
		// OO与OX共线
		if (fabs(D) < 1e-5 && !(fabs(B) < 1e-5 && fabs(C) < 1e-5)) {
			// 平面过原点
			source->SetPoint(1, A, B, C);
			target->SetPoint(1, 0, 0, sqrt(A*A + B*B + C*C));
		}
		else {
			// 平面垂直 X 轴
			source->SetPoint(1, 0, 1, 0);
			target->SetPoint(1, 0, 1, sqrt(vtkMath::Distance2BetweenPoints(origin, projPoint)));
		}
	}
	else {
		source->SetPoint(1, projPoint);
		target->SetPoint(1, origin);
	}
	target->SetPoint(2, 0, 0, sqrt(vtkMath::Distance2BetweenPoints(origin, projPoint)));
	source->Modified();
	target->Modified();
	transform->Update();
	vtkMatrix4x4 *matrix = transform->GetMatrix();
	double derminant = matrix->Determinant();
	if (fabs(derminant - 1) < 1e-3)
	{
		double *XiData = new double[size];
		double *YiData = new double[size];
		for (int i = 0; i < size; ++i)
		{
			double in[4], out[4];
			in[0] = m_Positions.at(i)[0];
			in[1] = m_Positions.at(i)[1];
			in[2] = m_Positions.at(i)[2];
			in[3] = 1.0;
			matrix->MultiplyPoint(in, out);
			XiData[i] = out[0];
			YiData[i] = out[1];
		}

		Data data(size, XiData, YiData);
		Circle2D circleFitter = CircleFitByHyper(data);
		delete[] XiData;
		delete[] YiData;
		m_Radius = circleFitter.r;
		matrix->Invert();
		double out[4];
		double center[4] = { circleFitter.a, circleFitter.b, 0, 1 };
		matrix->MultiplyPoint(center, out);
		m_Center[0] = out[0];
		m_Center[1] = out[1];
		m_Center[2] = out[2];

		m_Valid = 1;
	}
	else
	{
		m_Valid = 0;
	}
}
bool Fit3DCircle::getCircle(double center[3], double normal[3], double& radius)
{
	center[0] = m_Center[0];
	center[1] = m_Center[1];
	center[2] = m_Center[2];
	normal[0] = m_Normal[0];
	normal[1] = m_Normal[1];
	normal[2] = m_Normal[2];
	radius = m_Radius;

	return true;
}


bool Fit3DCircle::isValid()
{
	return m_Valid == 1;
}

//若点距圆心距离跟半径的差1.0，则认为点在圆上
bool Fit3DCircle::isPointOnCircle(const MarkerPointType &point)
{
	double p[3];
	p[0] = point[0]; p[1] = point[1]; p[2] = point[2];
	double dis = sqrt(vtkMath::Distance2BetweenPoints(p, m_Center));
	return fabs(dis - m_Radius) < 1.0;
}

void Fit3DCircle:: clearPoints()
{
	m_Valid = 0;
	m_Positions.clear();
}

bool Fit3DCircle::calRotateError(double& variance, double& mean)
{
	//若平面法向量为0，则返回
	if (m_Normal[0] == 0 && m_Normal[1] == 0 && m_Normal[2] == 0)
		return false;
	//构建轨迹圆所在的平面
	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	plane->SetNormal(m_Normal);
	plane->SetOrigin(m_Center);

	double sum = 0;
	std::vector<double> m_error;

	for (std::vector<MarkerPointType>::const_iterator itp = m_Positions.begin(); itp != m_Positions.end(); itp++) 
	{
		double d[3] = { (*itp)[0], (*itp)[1], (*itp)[2] };
		double distance = plane->DistanceToPlane(d);
		sum += distance;
		m_error.push_back(distance);
	}
	//如果此时点集的大小跟误差集的大小不一致，或点集为0则返回
	if (m_Positions.size() != m_error.size())
		return false;
	if (m_error.size() == 0)
		return false;

	m_ErrorMean = sum / m_error.size();
	sum = 0;
	for (std::vector<double>::const_iterator ite = m_error.begin(); ite != m_error.end(); ite++) {
		double e = *ite - m_ErrorMean;
		sum += e * e;
	}
	m_ErrorVariance = sum / m_error.size();

	variance = m_ErrorVariance;
	mean = m_ErrorMean;

	return true;
}