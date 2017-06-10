#ifndef RenderView_H
#define RenderView_H


/*
Qt headers
*/
#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>
#include <QVector3D>
/*
Ogre3D header
*/
#include <Ogre.h>

/*
Changed SdkCameraMan implementation to work with QKeyEvent, QMouseEvent, QWheelEvent
*/
#include "SdkQtCameraMan.h"

namespace  QtRenderView{
	//
#define  LIGHT_NAME   "MainLight"
#define  SCENE_CAMERA_NAME  "MainCamera"
#define  COORDINATE_CAMERA_NAME  "CoordinateCamera"

	//SceneNode name
#define  ACCEL_BOX_NAME          "AccelBoxNode"
#define  ACCEL_CHASSIS_NAME      "AccelChassisNode"
#define  ACCEL_CONNECT_NAME      "AccelConnectNode"
#define  ACCEL_BEDBOARD_NAME     "BedBoardNode"
#define  ACCEL_BED_CONNECT1_NAME "BedConnect1Node"
#define  ACCEL_BED_CONNECT2_NAME "BedConnect2Node"
#define  ACCEL_BED_STRECH_NAME   "BedStrechNode"
#define  ACCEL_BEDBOTTOM_NAME    "BedBottomNode"
#define  SOFT_ISOCENTER_NAME     "SoftISOCenterNode"
#define  LASER_ISOCENTER_NAME     "LaserISOCenterNode"
#define  MARKER_POINT_NAME       "MarkerPointNode"
#define  X_AXIS_NODE_NAME		 "XAxisNode"
#define  Y_AXIS_NODE_NAME		 "YAxisNode"
#define  VERTICAL_LINE_NODE_NAME "VerticalLineNode"
	// ManualObject name
#define  YAXIS_LINE_NAME         "YAxis"
#define  XAXIS_LINE_NAME         "XAxis"
#define  VERTICAL_LINE_NAME      "VerticalLine"


#define  ACCEL_BOX_BIAS          Ogre::Vector3(-24.0f,0.0f,0.0f)
#define  ACCEL_CONNECT_BIAS      Ogre::Vector3(4.25f, 0.0f, 0.0f)
#define  ACCEL_CHASSIS_BIAS      Ogre::Vector3(0.5f, 0.0f, 0.0f)
#define  ACCEL_BED_BOTTOM_BIAS   Ogre::Vector3(5.25f, -10.0f, 0.0f)
#define  ACCEL_BED_STRECH_BIAS   Ogre::Vector3(1.5f, 0.85f, 0.0f)
#define  ACCEL_BED_CONNECT2_BIAS Ogre::Vector3(0.0f, 0.7f, 0.0f)
#define  ACCEL_BED_CONNECT1_BIAS Ogre::Vector3(0.0f, 0.3f, 0.0f)
#define  ACCEL_BED_BOARD_BIAS    Ogre::Vector3(-1.0f, 0.25f,0.0f)

	/*
	//3D SCENE COORIDNATE
	//
	//****************************
	//          /|\  Y
	//           |
	//           |
	//           |
	//           |
	//           |
	//           | O
	//           |_ _ _ _ _ _ _ _ _ _ _\ X
	//          /                      /
	//         /
	//        /
	//       /
	//      /
	//     /
	//  | /
	//  |/__ Z
	//
	*/
	enum LineStyle{
		LS_SOLIDLINE = 0,
		LS_DOTLINE = 1,
		LS_RAYLINE = 2,
		LS_SEGEMENTLINE = 3
	};
	/*
	With the headers included we now need to inherit from QWindow.
	*/
	class RenderView : public QWindow, public Ogre::FrameListener
	{
		Q_OBJECT

	public:
		explicit RenderView(QWindow *parent = NULL);
		~RenderView();

		//    We declare these methods virtual to allow for further inheritance.
		virtual void render(QPainter *painter);
		virtual void render();
		virtual void initialize();
		virtual void createScene();
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
		virtual void createCompositor();
#endif
		//复位场景
		void resetScene();
		//
		void setAnimating(bool animating);
		//确定等中心
		void setISOCenter(const QVector3D& center);
		//旋转机架，degree是绝对角度，不是相对上一次的角度差
		void rotateGantry(float degree);
		//旋转辐射头，degree是绝对角度，不是相对上一次的角度差
		void rotateCollimator(float degree);
		//旋转机床,degree是绝对角度，不是相对上一次的角度差
		void rotateBed(float degree);
		//延Z轴移动机床
		void translateBedAlongZ(float z_mm);
		//延X轴移动机床
		void translateBedAlongX(float x_mm);
		//延Y轴移动机床
		void translateBedAlongY(float y_mm);
		//三维场景里的X轴对应机架旋转轴线
		void drawGantryAxis(const QVector3D& start, const QVector3D& end, const QColor& color = QColor(Qt::black));
		//三维场景里的Y轴对应机床旋转轴线
		void drawBedAxis(const QVector3D& start, const QVector3D& end, const QColor& color = QColor(Qt::black));
		//画出等中心
		void drawSoftISOCenter(const double x, const double y, const double z);
		void drawLaserISOCenter(const double x, const double y, const double z);
		//画屏幕左下方的参考坐标系
		void drawCoordinate();
		//画出标定球位置
		void drawMarkerPoint(const double x, const double y, const double z);
		//画两个轴线的公垂线
		void drawVerticalLine(const double footA[3], const double footB[3]);
		public slots:

		virtual void renderLater();
		virtual void renderNow();
		// We use an event filter to be able to capture keyboard/mouse events. More on this later.
		virtual bool eventFilter(QObject *target, QEvent *event);

	signals:
		//Event for clicking on an entity.
		void entitySelected(Ogre::Entity* entity);
		void ISOCenterSelected();
	protected:

		Ogre::Root*         mOgreRoot;
		Ogre::RenderWindow* mOgreWindow;
		Ogre::SceneManager* mOgreSceneMgr;
		Ogre::SceneManager* mCorSceneMgr;
		Ogre::SceneNode*    mOgreNode;
		Ogre::Camera*       mOgreCamera;
		Ogre::ColourValue   mOgreBackground;
		OgreQtBites::SdkQtCameraMan* mCameraMan;
		Ogre::SceneNode*    mOgreSoftCenterNode;
		Ogre::SceneNode*    mOgreLaserCenterNode;
		Ogre::SceneNode*    mOgreMarkerNode;

		bool m_update_pending;
		bool m_animating;


		virtual void keyPressEvent(QKeyEvent * ev);
		virtual void keyReleaseEvent(QKeyEvent * ev);
		virtual void mouseMoveEvent(QMouseEvent* e);
		virtual void wheelEvent(QWheelEvent* e);
		virtual void mousePressEvent(QMouseEvent* e);
		virtual void mouseReleaseEvent(QMouseEvent* e);
		virtual void exposeEvent(QExposeEvent *event);
		virtual bool event(QEvent *event);

		//  FrameListener method
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	};
}
#endif // RenderView_H

