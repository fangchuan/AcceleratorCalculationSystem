#include "renderview.h"
#include <QDebug>
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
#include <Compositor/OgreCompositorManager2.h>
#endif

/*
Note that we pass any supplied QWindow parent to the base QWindow class. This is necessary should we
need to use our class within a container.
*/
QtRenderView::RenderView::RenderView(QWindow *parent)
	: QWindow(parent)
	, mOgreRoot(NULL)
	, mOgreWindow(NULL)
	, mOgreCamera(NULL)
	, mCameraMan(NULL)
	, mOgreSoftCenterNode(NULL)
	, mOgreLaserCenterNode(NULL)
	, mOgreLightCenterNode(NULL)
	, mOgreMarkerNode(NULL)
	, m_update_pending(false)
	, m_animating(false)
{
	setAnimating(true);
	installEventFilter(this);
	mOgreBackground = Ogre::ColourValue(0.2f, 0.7f, 0.9f, 0.0f);

}

/*
Upon destruction of the QWindow object we destroy the Ogre3D scene.
*/
QtRenderView::RenderView::~RenderView()
{
	if (mCameraMan)
		delete mCameraMan;
	delete mOgreRoot;
}

/*
In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow) of Qt are supplied to this
class in any way we inform Qt that they will be unused.
*/
void QtRenderView::RenderView::render(QPainter *painter)
{
	Q_UNUSED(painter);
}

/*
Our initialization function. Called by our renderNow() function once when the window is first exposed.
*/
void QtRenderView::RenderView::initialize()
{
#if   _DEBUG
	Ogre::String  mResourcesCfg = "resources_d.cfg";
	Ogre::String  mPluginsCfg = "plugins_d.cfg";
#else
	Ogre::String mResourcesCfg = "resources.cfg";
	Ogre::String mPluginsCfg = "plugins.cfg";
#endif
#ifdef _MSC_VER
	mOgreRoot = new Ogre::Root(mPluginsCfg);
#else
	mOgreRoot = new Ogre::Root(mPluginsCfg);
#endif
	/*setup resource config file*/
	Ogre::ConfigFile ogreConfig;
	ogreConfig.load(mResourcesCfg);
	Ogre::ConfigFile::SectionIterator seci = ogreConfig.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}

	//setup the search order for used render system.
	Ogre::RenderSystem *rs = mOgreRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
	//Ogre::RenderSystem *rs = mOgreRoot->getRenderSystemByName("Direct3D11 Rendering Subsystem");
	QString dimensions = QString("%1x%2").arg(this->width()).arg(this->height());
	rs->setConfigOption("Full Screen", "No");
	rs->setConfigOption("Video Mode", dimensions.toStdString());
	rs->setConfigOption("VSync", "Yes");
	mOgreRoot->setRenderSystem(rs);
	mOgreRoot->initialise(false);

	Ogre::NameValuePairList parameters;
	//flag within the parameters set so that Ogre3D initializes an OpenGL context on it's own.
	if (rs->getName().find("GL") <= rs->getName().size())
		parameters["currentGLContext"] = Ogre::String("false");

	/*
	We need to supply the low level OS window handle to this QWindow so that Ogre3D knows where to draw
	the scene. Below is a cross-platform method on how to do this.
	If you set both options (externalWindowHandle and parentWindowHandle) this code will work with OpenGL
	and DirectX.
	*/
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
	parameters["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)(this->winId()));
	parameters["parentWindowHandle"] = Ogre::StringConverter::toString((size_t)(this->winId()));
#else
	parameters["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
	parameters["parentWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
#endif

#if defined(Q_OS_MAC)
	parameters["macAPI"] = "cocoa";
	parameters["macAPICocoaUseNSView"] = "true";
#endif

	mOgreWindow = mOgreRoot->createRenderWindow("QT Window",
												this->width(),
												this->height(),
												false,
												&parameters);
	mOgreWindow->setVisible(true);

	//create SceneManager ,Camera, viewport, and user's scene
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	const size_t numThreads = std::max<int>(1, Ogre::PlatformInformation::getNumLogicalCores());
	Ogre::InstancingThreadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
	if (numThreads > 1)threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
	mOgreSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC, numThreads, threadedCullingMethod);
#else
	mOgreSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC);
	mCorSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC);
#endif

	mOgreCamera = mOgreSceneMgr->createCamera(SCENE_CAMERA_NAME);
	mCorSceneMgr->createCamera(COORDINATE_CAMERA_NAME);
	mOgreCamera->setPosition(Ogre::Vector3(0.0f, 0.0f, 1.0f));
	mOgreCamera->lookAt(Ogre::Vector3(0.0f, 0.0f, -300.0f));
	mOgreCamera->setNearClipDistance(1.0f);
	mOgreCamera->setFarClipDistance(500.0f);
	mCameraMan = new OgreQtBites::SdkQtCameraMan(mOgreCamera);   // create a default camera controller


#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	createCompositor();
#else
	Ogre::Viewport* pViewPort = mOgreWindow->addViewport(mOgreCamera);
	pViewPort->setBackgroundColour(mOgreBackground);
#endif

	mOgreCamera->setAspectRatio(Ogre::Real(mOgreWindow->getWidth()) / Ogre::Real(mOgreWindow->getHeight()));
	mOgreCamera->setAutoAspectRatio(true);

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	createScene();
	//rehister frame listener
	mOgreRoot->addFrameListener(this);
}

void QtRenderView::RenderView::createScene()
{

	mOgreSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	
	//set accelerator material
	Ogre::MaterialPtr accelMaterial = Ogre::MaterialManager::getSingleton().create("AccelMaterial",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	accelMaterial->getTechnique(0)->getPass(0)->setAmbient(0.2f, 0.2f, 0.2f);
	accelMaterial->getTechnique(0)->getPass(0)->setDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
	accelMaterial->getTechnique(0)->getPass(0)->setSpecular(0.9f, 0.9f, 0.9f, 1.0f);
	//accelMaterial->getTechnique(0)->getPass(0)->setDepthBias(10.0f, 1.0f);
	accelMaterial->setManuallyLoaded(true);

	//load LNAC entiity: Box + Connect + Head + bedBoard + bedConnect1 + bedConnect2 + bedStrech + bedBottom
	Ogre::Entity* ogreEntity1 = mOgreSceneMgr->createEntity("acceleratorBox.mesh");
	Ogre::SceneNode* ogreNode1 = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(ACCEL_BOX_NAME, ACCEL_BOX_BIAS);
	ogreNode1->attachObject(ogreEntity1);
	ogreNode1->setScale(Ogre::Vector3(4.0f, 4.0f, 4.0f));
	ogreEntity1->setMaterialName("AccelMaterial");

	Ogre::Entity* ogreEntity2 = mOgreSceneMgr->createEntity("acceleratorConnect.mesh");
	Ogre::SceneNode* ogreNode2 = ogreNode1->createChildSceneNode(ACCEL_CONNECT_NAME, ACCEL_CONNECT_BIAS);
	ogreNode2->attachObject(ogreEntity2);
	ogreEntity2->setMaterialName("AccelMaterial");

	Ogre::Entity* ogreEntity3 = mOgreSceneMgr->createEntity("acceleratorHead.mesh");
	Ogre::SceneNode* ogreNode3 = ogreNode2->createChildSceneNode(ACCEL_CHASSIS_NAME, ACCEL_CHASSIS_BIAS);
	ogreNode3->attachObject(ogreEntity3);
	ogreEntity3->setMaterialName("AccelMaterial");

	//set bed material
	Ogre::MaterialPtr bedMaterial = Ogre::MaterialManager::getSingleton().create("BedMaterial",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	bedMaterial->getTechnique(0)->getPass(0)->setAmbient(0.1f, 0.1f, 0.1f);
	bedMaterial->getTechnique(0)->getPass(0)->setDiffuse(0.5f, 0.5f, 0.5f, 1.0f);
	bedMaterial->getTechnique(0)->getPass(0)->setSpecular(0.9f, 0.9f, 0.9f, 1.0f);
	bedMaterial->setManuallyLoaded(true);
	//accelMaterial->setSelfIllumination(0.2f, 0.2f, 0.1f);

	Ogre::Entity* ogreEntity4 = mOgreSceneMgr->createEntity("acceleratorbedBottom.mesh");
	Ogre::SceneNode* ogreNode4 = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(ACCEL_BEDBOTTOM_NAME, ACCEL_BED_BOTTOM_BIAS);
	ogreEntity4->setMaterialName("BedMaterial");
	ogreNode4->attachObject(ogreEntity4);
	ogreNode4->setScale(Ogre::Vector3(4.0f, 4.0f, 4.0f)); // Radius, in theory.


	Ogre::Entity* ogreEntity5 = mOgreSceneMgr->createEntity("acceleratorbedStrech.mesh");
	Ogre::SceneNode* ogreNode5 = ogreNode4->createChildSceneNode(ACCEL_BED_STRECH_NAME, ACCEL_BED_STRECH_BIAS);
	ogreEntity5->setMaterialName("AccelMaterial");
	ogreNode5->attachObject(ogreEntity5);

	Ogre::Entity* ogreEntity6 = mOgreSceneMgr->createEntity("acceleratorbedConnect2.mesh");
	Ogre::SceneNode* ogreNode6 = ogreNode5->createChildSceneNode(ACCEL_BED_CONNECT2_NAME, ACCEL_BED_CONNECT2_BIAS);
	ogreEntity6->setMaterialName("Example/bedconnect");
	ogreNode6->attachObject(ogreEntity6);
	ogreNode6->setInheritScale(false);
	ogreNode6->setScale(Ogre::Vector3(4.0f, 4.0f, 4.0f));

	Ogre::Entity* ogreEntity7 = mOgreSceneMgr->createEntity("acceleratorbedConnect1.mesh");
	Ogre::SceneNode* ogreNode7 = ogreNode6->createChildSceneNode(ACCEL_BED_CONNECT1_NAME, ACCEL_BED_CONNECT1_BIAS);
	ogreEntity7->setMaterialName("Example/bedconnect");
	ogreNode7->attachObject(ogreEntity7);

	Ogre::Entity* ogreEntity8 = mOgreSceneMgr->createEntity("acceleratorbedBoard.mesh");
	Ogre::SceneNode* ogreNode8 = ogreNode7->createChildSceneNode(ACCEL_BEDBOARD_NAME, ACCEL_BED_BOARD_BIAS);
	ogreEntity8->setMaterialName("Example/bedboard");
	ogreNode8->attachObject(ogreEntity8);
	
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	Ogre::SceneNode* pLightNode = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Light* light = mOgreSceneMgr->createLight();
	pLightNode->attachObject(light);
	pLightNode->setPosition(20.0f, 80.0f, 50.0f);
#else
	Ogre::Light* light = mOgreSceneMgr->createLight(LIGHT_NAME);
	light->setPosition(20.0f, 80.0f, 50.0f);
#endif

	drawCoordinate();
}

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
void QtRenderView::RenderView::createCompositor()
{
	/*
	Example compositor
	Derive this class for your own purpose and overwite this function to have a working Ogre
	widget with your own compositor.
	*/
	Ogre::CompositorManager2* compMan = mOgreRoot->getCompositorManager2();
	const Ogre::String workspaceName = "default scene workspace";
	const Ogre::IdString workspaceNameHash = workspaceName;
	compMan->createBasicWorkspaceDef(workspaceName, mOgreBackground);
	compMan->addWorkspace(mOgreSceneMgr, mOgreWindow, mOgreCamera, workspaceNameHash, true);
}
#endif

void QtRenderView::RenderView::render()
{
	/*
	How we tied in the render function for OGre3D with QWindow's render function. This is what gets call
	repeatedly. Note that we don't call this function directly; rather we use the renderNow() function
	to call this method as we don't want to render the Ogre3D scene unless everything is set up first.
	That is what renderNow() does.
	*/
	Ogre::WindowEventUtilities::messagePump();
	mOgreRoot->renderOneFrame();

	Ogre::Light* light = mOgreSceneMgr->getLight(LIGHT_NAME);
	Ogre::Vector3 p = mOgreCamera->getPosition();
	if (NULL != light)
		light->setPosition(p);

	Ogre::Quaternion q = mOgreCamera->getOrientation().Inverse();
	Ogre::SceneNode* coordinateNode = mCorSceneMgr->getSceneNode("Coordinate");
	if (NULL != coordinateNode)
		coordinateNode->setOrientation(q);

}

void QtRenderView::RenderView::renderLater()
{
	/*
	This function forces QWindow to keep rendering. Omitting this causes the renderNow() function to
	only get called when the window is resized, moved, etc. as opposed to all of the time; which is
	generally what we need.
	*/
	if (!m_update_pending)
	{
		m_update_pending = true;
		QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
	}
}

bool QtRenderView::RenderView::event(QEvent *event)
{
	switch (event->type())
	{
	case QEvent::UpdateRequest:
		m_update_pending = false;
		renderNow();
		return true;

	default:
		return QWindow::event(event);
	}
}

/*
Called after the QWindow is reopened or when the QWindow is first opened.
*/
void QtRenderView::RenderView::exposeEvent(QExposeEvent *event)
{
	Q_UNUSED(event);

	if (isExposed())
		renderNow();
}

/*
The renderNow() function calls the initialize() function when needed and if the QWindow is already
initialized and prepped calls the render() method.
*/
void QtRenderView::RenderView::renderNow()
{
	if (!isExposed())
		return;

	if (NULL == mOgreRoot)
	{
		initialize();
	}

	render();

	if (m_animating)
		renderLater();
}

/*
Our event filter; handles the resizing of the QWindow. When the size of the QWindow changes note the
call to the Ogre3D window and camera. This keeps the Ogre3D scene looking correct.
*/
bool QtRenderView::RenderView::eventFilter(QObject *target, QEvent *event)
{
	if (target == this)
	{
		if (event->type() == QEvent::Resize)
		{
			if (isExposed() && mOgreWindow != NULL)
			{
				mOgreWindow->resize(this->width(), this->height());
			}
		}
	}

	return false;
}

/*
How we handle keyboard and mouse events.
*/
void QtRenderView::RenderView::keyPressEvent(QKeyEvent * ev)
{
	if (mCameraMan)
		mCameraMan->injectKeyDown(*ev);
}

void QtRenderView::RenderView::keyReleaseEvent(QKeyEvent * ev)
{
	if (mCameraMan)
		mCameraMan->injectKeyUp(*ev);
}

void QtRenderView::RenderView::mouseMoveEvent(QMouseEvent* e)
{
	static int lastX = e->x();
	static int lastY = e->y();
	int relX = e->x() - lastX;
	int relY = e->y() - lastY;
	lastX = e->x();
	lastY = e->y();

	if (mCameraMan && (e->buttons() & Qt::LeftButton))
		mCameraMan->injectMouseMove(relX, relY);
}

void QtRenderView::RenderView::wheelEvent(QWheelEvent *e)
{
	if (mCameraMan)
		mCameraMan->injectWheelMove(*e);
}

void QtRenderView::RenderView::mousePressEvent(QMouseEvent* e)
{
	if (mCameraMan)
		mCameraMan->injectMouseDown(*e);
}

void QtRenderView::RenderView::mouseReleaseEvent(QMouseEvent* e)
{
	if (mCameraMan)
		mCameraMan->injectMouseUp(*e);

	QPoint pos = e->pos();
	Ogre::Ray mouseRay = mOgreCamera->getCameraToViewportRay(
		(Ogre::Real)pos.x() / mOgreWindow->getWidth(),
		(Ogre::Real)pos.y() / mOgreWindow->getHeight());
	Ogre::RaySceneQuery* pSceneQuery = mOgreSceneMgr->createRayQuery(mouseRay);
	pSceneQuery->setSortByDistance(true);
	Ogre::RaySceneQueryResult vResult = pSceneQuery->execute();
	for (size_t ui = 0; ui < vResult.size(); ui++)
	{
		if (vResult[ui].movable)
		{
			if (vResult[ui].movable->getMovableType().compare("Entity") == 0)
			{
				emit entitySelected((Ogre::Entity*)vResult[ui].movable);
			}
		}
	}
	mOgreSceneMgr->destroyQuery(pSceneQuery);
}

/*
Function to keep track of when we should and shouldn't redraw the window; we wouldn't want to do
rendering when the QWindow is minimized. This takes care of those scenarios.
*/
void QtRenderView::RenderView::setAnimating(bool animating)
{
	m_animating = animating;

	if (animating)
		renderLater();
}

bool QtRenderView::RenderView::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mCameraMan->frameRenderingQueued(evt);
	return true;
}

void QtRenderView::RenderView::resetScene()
{

	//Ogre::SceneNode* ogreNode = mOgreSceneMgr->getSceneNode(ACCEL_CONNECT_NAME);
	//ogreNode->resetOrientation();

	//ogreNode = mOgreSceneMgr->getSceneNode(ACCEL_BEDBOTTOM_NAME);
	//ogreNode->resetOrientation();
	//ogreNode->setPosition(ACCEL_BED_BOTTOM_BIAS);
	rotateGantry(0);
	rotateBed(0);

	Ogre::SceneNode* ogreNode = mOgreSceneMgr->getSceneNode(ACCEL_BED_STRECH_NAME);
	ogreNode->setScale(1.0f, 1.0f, 1.0f);
	ogreNode->setPosition(ACCEL_BED_STRECH_BIAS);

	ogreNode = mOgreSceneMgr->getSceneNode(ACCEL_BED_CONNECT2_NAME);
	ogreNode->setPosition(ACCEL_BED_CONNECT2_BIAS);

	ogreNode = mOgreSceneMgr->getSceneNode(ACCEL_BED_CONNECT1_NAME);
	ogreNode->setPosition(ACCEL_BED_CONNECT1_BIAS);

	ogreNode = mOgreSceneMgr->getSceneNode(ACCEL_BEDBOARD_NAME);
	ogreNode->setPosition(ACCEL_BED_BOARD_BIAS);

	if (mOgreSceneMgr->hasSceneNode(LASER_ISOCENTER_NAME)){
		ogreNode = mOgreSceneMgr->getSceneNode(LASER_ISOCENTER_NAME);
		ogreNode->setVisible(false);
	}
	
	if (mOgreSceneMgr->hasSceneNode(LIGHT_CENTER_NAME)){
		ogreNode = mOgreSceneMgr->getSceneNode(LIGHT_CENTER_NAME);
		ogreNode->setVisible(false);
	}

	if (mOgreSceneMgr->hasSceneNode(SOFT_ISOCENTER_NAME)){
		ogreNode = mOgreSceneMgr->getSceneNode(SOFT_ISOCENTER_NAME);
		ogreNode->setVisible(false);
	}

	if(mOgreSceneMgr->hasSceneNode(MARKER_POINT_NAME)){
		ogreNode = mOgreSceneMgr->getSceneNode(MARKER_POINT_NAME);
		ogreNode->setVisible(false);
	}

	if(mOgreSceneMgr->hasSceneNode(Y_AXIS_NODE_NAME)){
		ogreNode = mOgreSceneMgr->getSceneNode(Y_AXIS_NODE_NAME);
		ogreNode->setVisible(false);
	}

	if(mOgreSceneMgr->hasSceneNode(X_AXIS_NODE_NAME)){
		ogreNode = mOgreSceneMgr->getSceneNode(X_AXIS_NODE_NAME);
		ogreNode->setVisible(false);
	}


}
void QtRenderView::RenderView::setISOCenter(const QVector3D &center)
{

}
/*
*  旋转机架
*/
void QtRenderView::RenderView::rotateGantry(float degree)
{
	static float last_degree = 0;
	if (!isnan(degree)){
		float delt_degree = degree - last_degree;

		mOgreNode = mOgreSceneMgr->getSceneNode(ACCEL_CONNECT_NAME);
		if (NULL != mOgreNode && 0 != delt_degree){
			mOgreNode->pitch(Ogre::Degree(delt_degree));
		}
		last_degree = degree;
		mOgreNode = NULL;
	}
}
/*
*  旋转辐射头
*/
void QtRenderView::RenderView::rotateCollimator(float degree)
{

}
/*
*  旋转cbct
*/
void QtRenderView::RenderView::rotateCbct(float degree)
{
}
/*
* 旋转机床
*/
void QtRenderView::RenderView::rotateBed(float degree)
{
	float x_mm;
	float z_mm;
	static float last_degree = 0;
	if (!isnan(degree)){
		float delt_degree = degree - last_degree;

		mOgreNode = mOgreSceneMgr->getSceneNode(ACCEL_BEDBOTTOM_NAME);
		if (NULL != mOgreNode && 0 != delt_degree){

			x_mm = ACCEL_BED_BOTTOM_BIAS.x * Ogre::Math::Cos(Ogre::Math::DegreesToRadians(degree));
			z_mm = -ACCEL_BED_BOTTOM_BIAS.x * Ogre::Math::Sin(Ogre::Math::DegreesToRadians(degree));

			if (x_mm >= ACCEL_BED_BOTTOM_BIAS.x)	x_mm = ACCEL_BED_BOTTOM_BIAS.x;
			if (x_mm <= -ACCEL_BED_BOTTOM_BIAS.x)   x_mm = -ACCEL_BED_BOTTOM_BIAS.x;
			if (z_mm >= ACCEL_BED_BOTTOM_BIAS.x)    z_mm = ACCEL_BED_BOTTOM_BIAS.x;
			if (z_mm <= -ACCEL_BED_BOTTOM_BIAS.x)	z_mm = -ACCEL_BED_BOTTOM_BIAS.x;

			mOgreNode->yaw(Ogre::Degree(delt_degree));

			mOgreNode->setPosition(x_mm, ACCEL_BED_BOTTOM_BIAS.y, z_mm);

		}
		last_degree = degree;
		mOgreNode = NULL;
	}
}
/*
*   沿着Z方向移动机床 : z_mm为绝对距离
*/
void QtRenderView::RenderView::translateBedAlongZ(float z_mm)
{
	static float last_z = 0.0f;
	float z = z_mm - last_z;

	mOgreNode = mOgreSceneMgr->getSceneNode(ACCEL_BEDBOARD_NAME);
	if (NULL != mOgreNode && 0 != z){
		//translate resolution:  m
		mOgreNode->translate(z * 0.01, 0, 0, Ogre::Node::TS_PARENT);
		last_z = z_mm;
	}

	mOgreNode = NULL;
}
/*
*   沿着X方向移动机床 :x_mm为绝对距离
*/
void QtRenderView::RenderView::translateBedAlongX(float x_mm)
{
	static float last_x = 0.0;
	float x = x_mm - last_x;

	mOgreNode = mOgreSceneMgr->getSceneNode(ACCEL_BED_CONNECT1_NAME);
	if (NULL != mOgreNode && 0 != x){
		//translate resolution:  m
		mOgreNode->translate(0, 0, -x * 0.01, Ogre::Node::TS_PARENT);//0.01 propotation
		last_x = x_mm;
	}

	mOgreNode = NULL;

}
/*
*   沿着Y方向移动机床 :y_mm为绝对距离
*/
void QtRenderView::RenderView::translateBedAlongY(float y_mm)
{
	//本不应该将y轴的放大系数设为static,但是不设为static的话放大/缩小效果太不明显
	static float y_scale = 1.0f;
	static float last_y = 0.0f;

	float y_trans = 0.75f;
	float y_m = (y_mm - last_y)*0.01;//0.01 propotation

	mOgreNode = mOgreSceneMgr->getSceneNode(ACCEL_BED_STRECH_NAME);
	if (NULL != mOgreNode && 0 != y_m){
		y_scale += y_m;
		mOgreNode->setScale(Ogre::Vector3(1.0f, y_scale, 1.0f));
		y_trans *= y_m;
		mOgreNode->translate(0, y_trans, 0, Ogre::Node::TS_PARENT);

		last_y = y_mm;
	}

	mOgreNode = NULL;
}

void QtRenderView::RenderView::drawGantryAxis(const QVector3D &start, const QVector3D &end, const QColor &color)
{
	float x_s = start.z();
	float y_s = start.y();
	float z_s = -start.x();
	float x_e = end.z();
	float y_e = end.y();
	float z_e = -end.x();

	float x_center = (x_s + x_e)*0.5;
	float y_center = (y_s + y_e)*0.5;
	float z_center = (z_s + z_e)*0.5;

	float r = (float)color.redF();
	float g = (float)color.greenF();
	float b = (float)color.blueF();

	if (!mOgreSceneMgr->hasManualObject(XAXIS_LINE_NAME)){
		Ogre::ManualObject* xAxis = mOgreSceneMgr->createManualObject(XAXIS_LINE_NAME);
		xAxis->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
		xAxis->position(x_s, y_s, z_s);  // start position
		xAxis->colour(r, g, b);
		xAxis->position(x_e, y_e, z_e);  // draw first line
		xAxis->end();
		//centerGantry = Ogre::Vector3(0, y_s, z_s);
		mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(X_AXIS_NODE_NAME)->attachObject(xAxis);

		Ogre::Entity* ogreEntity = mOgreSceneMgr->createEntity("GantryCenter", Ogre::SceneManager::PT_SPHERE);
		Ogre::SceneNode* ogreNode = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode("GantryCenterNode", Ogre::Vector3(x_center, y_center, z_center));
		ogreNode->attachObject(ogreEntity);
		ogreEntity->setMaterialName("Examples/green");
		ogreNode->setScale(Ogre::Vector3(0.002f, 0.002f, 0.002f)); // Radius, in theory.
	}
	else{
		Ogre::ManualObject* xAxis = mOgreSceneMgr->getManualObject(XAXIS_LINE_NAME);
		Ogre::SceneNode* node = mOgreSceneMgr->getSceneNode(X_AXIS_NODE_NAME);
		node->setVisible(true);
		xAxis->beginUpdate(0);
		xAxis->position(x_s, y_s, z_s);  // start position
		xAxis->colour(r, g, b);
		xAxis->position(x_e, y_e, z_e);  // draw first line
		xAxis->end();

		node = mOgreSceneMgr->getSceneNode("GantryCenterNode");
		node->setPosition(x_center, y_center, z_center);
		//centerGantry = Ogre::Vector3(0, y_s, z_s);
	}
	//若两个轴线都绘制完成，则开始绘制公垂线
	//m_gantry_calibrated = true;
	//if (m_gantry_calibrated && m_bed_calibrated){
	//	drawVerticalLine();
	//	emit ISOCenterSelected();
	//}
}

void QtRenderView::RenderView::drawBedAxis(const QVector3D &start, const QVector3D &end, const QColor &color)
{
	float x_s = -start.z();
	float y_s = start.y();
	float z_s = start.x();
	float x_e = -end.z();
	float y_e = end.y();
	float z_e = end.x();

	float x_center = (x_s + x_e)*0.5;
	float y_center = (y_s + y_e)*0.5;
	float z_center = (z_s + z_e)*0.5;

	float r = (float)color.redF();
	float g = (float)color.greenF();
	float b = (float)color.blueF();

	if (!mOgreSceneMgr->hasManualObject(YAXIS_LINE_NAME)){
		Ogre::ManualObject* yAxis = mOgreSceneMgr->createManualObject(YAXIS_LINE_NAME);
		yAxis->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
		yAxis->position(x_s, y_s, z_s);// start position
		yAxis->colour(r, g, b);
		yAxis->position(x_e, y_e, z_e);// draw first line
		yAxis->end();
		//centerBed = Ogre::Vector3(x_s, 0, z_s);
		mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(Y_AXIS_NODE_NAME)->attachObject(yAxis);

		Ogre::Entity* ogreEntity = mOgreSceneMgr->createEntity("BedCenter", Ogre::SceneManager::PT_SPHERE);
		Ogre::SceneNode* ogreNode = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode("BedCenterNode", Ogre::Vector3(x_center, y_center, z_center));
		ogreNode->attachObject(ogreEntity);
		ogreEntity->setMaterialName("Examples/blue");
		ogreNode->setScale(Ogre::Vector3(0.002f, 0.002f, 0.002f)); // Radius, in theory.
	}
	else{
		Ogre::ManualObject* yAxis = mOgreSceneMgr->getManualObject(YAXIS_LINE_NAME);
		Ogre::SceneNode*  node = mOgreSceneMgr->getSceneNode(Y_AXIS_NODE_NAME);
		node->setVisible(true);
		yAxis->beginUpdate(0);
		yAxis->position(x_s, y_s, z_s);// start position
		yAxis->colour(r, g, b);
		yAxis->position(x_e, y_e, z_e);// draw first line
		yAxis->end();

		node =  mOgreSceneMgr->getSceneNode("BedCenterNode");
		node->setPosition(x_center, y_center, z_center);
		//centerBed = Ogre::Vector3(x_s, 0, z_s);
	}
	//若两个轴线都绘制完成，则开始绘制公垂线
	//m_bed_calibrated = true;
	//if (m_gantry_calibrated && m_bed_calibrated){
	//	drawVerticalLine();
	//	emit ISOCenterSelected();
	//}
}
//画软件结算的等中心小球
void QtRenderView::RenderView::drawSoftISOCenter(const double x, const double y, const double z)
{
	//set ISO Center:(0,0,0)
	if (NULL == mOgreSoftCenterNode){
		Ogre::Entity* ogreEntity = mOgreSceneMgr->createEntity("SoftISOCenter", Ogre::SceneManager::PT_SPHERE);
		mOgreSoftCenterNode = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(SOFT_ISOCENTER_NAME, Ogre::Vector3(x, y, z));
		mOgreSoftCenterNode->attachObject(ogreEntity);
		ogreEntity->setMaterialName("Examples/red");
		mOgreSoftCenterNode->setScale(Ogre::Vector3(0.001f, 0.001f, 0.001f)); // Radius, in theory.
	}else{
		mOgreSoftCenterNode->setPosition(x, y, z);
	}
}
//画激光等中心小球
void QtRenderView::RenderView::drawLaserISOCenter(const double x, const double y, const double z)
{

	if (NULL == mOgreLaserCenterNode){
		Ogre::Entity* ogreEntity = mOgreSceneMgr->createEntity("LaserISOCenter", Ogre::SceneManager::PT_SPHERE);
		mOgreLaserCenterNode = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(LASER_ISOCENTER_NAME, Ogre::Vector3(x, y, z));
		mOgreLaserCenterNode->attachObject(ogreEntity);
		ogreEntity->setMaterialName("Examples/lightblue");
		mOgreLaserCenterNode->setScale(Ogre::Vector3(0.001f, 0.001f, 0.001f)); // Radius, in theory.
	}
	else{
		mOgreLaserCenterNode->setPosition(x, y, z);
	}
}
//
void QtRenderView::RenderView::drawLightCenter(const double x, const double y, const double z)
{
	if (NULL == mOgreLightCenterNode){
		Ogre::Entity* ogreEntity = mOgreSceneMgr->createEntity("Light Center", Ogre::SceneManager::PT_SPHERE);
		mOgreLightCenterNode = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(LIGHT_CENTER_NAME, Ogre::Vector3(x, y, z));
		mOgreLightCenterNode->attachObject(ogreEntity);
		ogreEntity->setMaterialName("Examples/lightblue");
		mOgreLightCenterNode->setScale(Ogre::Vector3(0.001f, 0.001f, 0.001f)); // Radius, in theory.
	}
	else{
		mOgreLightCenterNode->setPosition(x, y, z);
	}
}
//实时更新小球运动位置
void QtRenderView::RenderView::drawMarkerPoint(const double x, const double y, const double z)
{
	if (NULL == mOgreMarkerNode){
		Ogre::Entity* ogreEntityCenter = mOgreSceneMgr->createEntity("Marker Point", Ogre::SceneManager::PT_SPHERE);
		mOgreMarkerNode = mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(MARKER_POINT_NAME, Ogre::Vector3(x, y, z));
		mOgreMarkerNode->attachObject(ogreEntityCenter);
		ogreEntityCenter->setMaterialName("Examples/gray");
		mOgreMarkerNode->setScale(Ogre::Vector3(0.004f, 0.004f, 0.004f)); // Radius, in theory.
	}else{
		mOgreMarkerNode->setVisible(true);
		mOgreMarkerNode->setPosition(x, y, z);
	}
}
//画屏幕左下方的参考坐标系
void QtRenderView::RenderView::drawCoordinate()
{
	Ogre::Viewport *vp = 0;
	Ogre::Camera* cam = mCorSceneMgr->getCamera(COORDINATE_CAMERA_NAME);
	cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	mCorSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	vp = mOgreWindow->addViewport(cam, 1, 0.0f, 0.8f, 0.2f, 0.2f);
	vp->setBackgroundColour(mOgreBackground);
	//    cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	cam->setOrthoWindow(9,9);
	cam->setPosition(0, 0, 7);
	cam->lookAt(0, 0, -300);
	cam->setNearClipDistance(0.1f);
	cam->setFarClipDistance(200000);


	Ogre::SceneNode* ogreNodeCoordinate = mCorSceneMgr->getRootSceneNode()->createChildSceneNode("Coordinate", Ogre::Vector3(-0.8f, -1.0f, 0.0f));

	Ogre::Entity* ogreEntity9 = mOgreSceneMgr->createEntity("coordinateBox002.mesh");
	Ogre::SceneNode* ogreNode9 = ogreNodeCoordinate->createChildSceneNode("X002", Ogre::Vector3(0, 0, 0));
	ogreEntity9->setMaterialName("Examples/blue");
	ogreNode9->attachObject(ogreEntity9);
	ogreNode9->setScale(Ogre::Vector3(5.0f, 5.0f, 10.0f));
	ogreNode9->yaw(Ogre::Degree(180));

	Ogre::Entity* ogreEntity10 = mOgreSceneMgr->createEntity("coordinateBox001.mesh");
	Ogre::SceneNode* ogreNode10 = ogreNode9->createChildSceneNode("X001", Ogre::Vector3(0.0f, 0.0f, 0.2f));
	ogreEntity10->setMaterialName("Examples/blue");
	ogreNode10->attachObject(ogreEntity10);

	Ogre::Entity* ogreEntity11 = mOgreSceneMgr->createEntity("coordinateBox002.mesh");
	Ogre::SceneNode* ogreNode11 = ogreNodeCoordinate->createChildSceneNode("Z002", Ogre::Vector3(0, 0, 0));
	ogreEntity11->setMaterialName("Examples/green");
	ogreNode11->attachObject(ogreEntity11);
	ogreNode11->setScale(Ogre::Vector3(5.0f, 5.0f, 10.0f));
	ogreNode11->yaw(Ogre::Degree(90));

	Ogre::Entity* ogreEntity12 = mOgreSceneMgr->createEntity("coordinateBox001.mesh");
	Ogre::SceneNode* ogreNode12 = ogreNode11->createChildSceneNode("Z001", Ogre::Vector3(0.0f, 0.0f, 0.2f));
	ogreEntity12->setMaterialName("Examples/green");
	ogreNode12->attachObject(ogreEntity12);

	Ogre::Entity* ogreEntity13 = mOgreSceneMgr->createEntity("coordinateBox002.mesh");
	Ogre::SceneNode* ogreNode13 = ogreNodeCoordinate->createChildSceneNode("Y002", Ogre::Vector3(0.0f, 0.0f, 0.0f));
	ogreEntity13->setMaterialName("Examples/red");
	ogreNode13->attachObject(ogreEntity13);
	ogreNode13->setScale(Ogre::Vector3(5.0f, 5.0f, 10.0f));
	ogreNode13->pitch(Ogre::Degree(-90));

	Ogre::Entity* ogreEntity14 = mOgreSceneMgr->createEntity("coordinateBox001.mesh");
	Ogre::SceneNode* ogreNode14 = ogreNode13->createChildSceneNode("Y001", Ogre::Vector3(0.0f, 0.0f, 0.2f));
	ogreEntity14->setMaterialName("Examples/red");
	ogreNode14->attachObject(ogreEntity14);

	Ogre::Entity* ogreEntity15 = mOgreSceneMgr->createEntity("coordinatetextZ.mesh");
	Ogre::SceneNode* ogreNode15 = ogreNode11->createChildSceneNode("Z", Ogre::Vector3(0.0f, 0.0f, 0.28f));
	ogreEntity15->setMaterialName("Examples/green");
	ogreNode15->attachObject(ogreEntity15);

	Ogre::Entity* ogreEntity16 = mOgreSceneMgr->createEntity("coordinatetextY.mesh");
	Ogre::SceneNode* ogreNode16 = ogreNode13->createChildSceneNode("Y", Ogre::Vector3(0.0f, 0.0f, 0.28f));
	ogreEntity16->setMaterialName("Examples/red");
	ogreNode16->attachObject(ogreEntity16);
	ogreNode16->pitch(Ogre::Degree(90));

	Ogre::Entity* ogreEntity17 = mOgreSceneMgr->createEntity("coordinatetextX.mesh");
	Ogre::SceneNode* ogreNode17 = ogreNode9->createChildSceneNode("X", Ogre::Vector3(0.0f, 0.0f, 0.28f));
	ogreEntity17->setMaterialName("Examples/blue");
	ogreNode17->attachObject(ogreEntity17);
	/*
	//Ogre::ManualObject* manCoordinate = mCorSceneMgr->createManualObject("CoordinateManualObject");
	//if (NULL == manCoordinate)
	//	return;
	////begin manual draw coordinate axis
	//manCoordinate->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	//{
	//	//x
	//	manCoordinate->position(0, 0, 0);
	//	manCoordinate->colour(Ogre::ColourValue::Blue);
	//	manCoordinate->position(5, 0, 0);
	//	manCoordinate->colour(Ogre::ColourValue::Red);
	//	manCoordinate->position(5, 0, 0);
	//	manCoordinate->position(3.5, 0.8, 0);
	//	manCoordinate->position(5, 0, 0);
	//	manCoordinate->position(3.5, -0.8, 0);
	//	//y
	//	manCoordinate->position(0, 0, 0);
	//	manCoordinate->colour(Ogre::ColourValue::Red);
	//	manCoordinate->position(0, 5, 0);
	//	manCoordinate->colour(Ogre::ColourValue::Green);
	//	manCoordinate->position(0, 5, 0);
	//	manCoordinate->position(0.8, 3.5, 0);
	//	manCoordinate->position(0, 5, 0);
	//	manCoordinate->position(-0.8, 3.5, 0);
	//	//z
	//	manCoordinate->position(0, 0, 0);
	//	manCoordinate->colour(Ogre::ColourValue::Blue);
	//	manCoordinate->position(0, 0, 5);
	//	manCoordinate->colour(Ogre::ColourValue::Red);
	//	manCoordinate->position(0, 0, 5);
	//	manCoordinate->colour(Ogre::ColourValue::Blue);
	//	manCoordinate->position(0, 0.8, 3.5);
	//	manCoordinate->position(0, 0, 5);
	//	manCoordinate->position(0, -0.8, 3.5);
	//}
	//manCoordinate->end();
	//Ogre::SceneNode* pNode = mCorSceneMgr->getRootSceneNode()->createChildSceneNode("CoordinateNode");
	//if (NULL == pNode)
	//	return;
	//pNode->attachObject(manCoordinate);

	//{
	//	Ogre::ManualObject* manXLetter = mCorSceneMgr->createManualObject("LetterXManualObject");
	//	//begin draw axis letter:'X'
	//	manXLetter->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	//	manXLetter->position(5.2, 0, 0);
	//	manXLetter->colour(Ogre::ColourValue::Red);
	//	manXLetter->position(6, -1, 1);
	//	manXLetter->end();

	//	manXLetter->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	//	manXLetter->position(6, 0, 0);
	//	manXLetter->colour(Ogre::ColourValue::Red);
	//	manXLetter->position(5.2, -1, 1);
	//	manXLetter->end();
	//	pNode->attachObject(manXLetter);

	//	Ogre::ManualObject* manYLetter = mCorSceneMgr->createManualObject("LetterYManualObject");
	//	//begin draw axis letter:'Y'
	//	manYLetter->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	//	manYLetter->position(0.8, 5.5, 0);
	//	manYLetter->colour(Ogre::ColourValue::Green);
	//	manYLetter->position(1.2, 5.0, 0);
	//	manYLetter->position(1.2, 5.0, 0);
	//	manYLetter->position(1.2, 4.0, 0);
	//	manYLetter->position(1.2, 5.0, 0);
	//	manYLetter->position(1.6, 5.5, 0);
	//	manYLetter->end();
	//	pNode->attachObject(manYLetter);

	//	Ogre::ManualObject* manZLetter = mCorSceneMgr->createManualObject("LetterZManualObject");
	//	//begin draw axis letter:'Z'
	//	manZLetter->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
	//	manZLetter->position(0, 0.5, 6.0);
	//	manZLetter->colour(Ogre::ColourValue::Blue);
	//	manZLetter->position(0, 0.5, 5.0);
	//	manZLetter->position(0, 0.5, 5.0);
	//	manZLetter->position(0, -0.5, 6.0);
	//	manZLetter->position(0, -0.5, 6.0);
	//	manZLetter->position(0, -0.5, 5.0);
	//	manZLetter->end();
	//	pNode->attachObject(manZLetter);
	//}
	*/

}
//画两个轴线的公垂线
void QtRenderView::RenderView::drawVerticalLine(const double footA[3], const double footB[3])
{
	double x_s = footA[2] * 0.01;
	double y_s = footA[1] * 0.01;
	double z_s = -footA[0] * 0.01;
	double x_e = footB[2] * 0.01;
	double y_e = footB[1] * 0.01;
	double z_e = -footB[0] * 0.01;

	if (!mOgreSceneMgr->hasManualObject(VERTICAL_LINE_NAME)){
		Ogre::ManualObject* line = mOgreSceneMgr->createManualObject(VERTICAL_LINE_NAME);
		line->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
		line->position(x_s, y_s, z_s);// start position
		line->colour(1.0, 0, 0); //red color
		line->position(x_e, y_e, z_e);// draw first line
		line->end();
		mOgreSceneMgr->getRootSceneNode()->createChildSceneNode(VERTICAL_LINE_NODE_NAME)->attachObject(line);
	}
	else{
		Ogre::ManualObject* line = mOgreSceneMgr->getManualObject(VERTICAL_LINE_NAME);
		//Ogre::SceneNode*  node = mOgreSceneMgr->getSceneNode(VERTICAL_LINE_NODE_NAME);
		line->beginUpdate(0);
		line->position(x_s, y_s, z_s);// start position
		line->colour(1.0, 0, 0); //red color
		line->position(x_e, y_e, z_e);// draw first line
		line->end();
	}
}
