# AcceleratorCalculationSystem
2017-5-23:  完成class RenderView 模型与坐标轴显示。采用两个Scene分别显示模型与坐标轴，可以做到坐标轴随模型场景中的相机旋转而旋转
			完成Ogre系统裁剪，只保留需要的部分，软件开启速度明显加快
			
2017-5-24： 完成水平面注册问题。
			机架旋转问题要考虑个合适的模拟方案
			
2017-5-25： 机床旋转测试正常，但是在从负角度转到证角度的过程中三维见面会出现错误。未解决

2017-5-26： fitCircle在0°附近会将center拟合出错()，导致acos返回nan，从而导致三维显示时出错.待解决。。。
			机床平移测试，正常
			机架旋转测试，正常
			拟合计算calculate()的条件不太合理，初始点(第一个点)附近100mm的点都不参与计算(范围缩小到10mm会出现计算出错的情况???)，待解决
			plotView显示部分做了点修改，可以一直更新上一次的数据点
			增加清空plotView中curveData的功能，需要加锁操作。。。还未加

2017-5-31： 添加 class logger，简单的记录操作日志

2017-6-3： 添加class LoggerView, 采用QTextEdit显示报告，可以保存、另存为、打印、打印预览、生成PDF
			
2017-6-4:  输出报告采用HTML 排版，所有输出内容在一页纸上

2017-6-5： 重构 class CentralModel和CentralWidget, Fit3DCircle里增加计算旋转误差函数
		   最后生成报告的流程: 所有校准操作结束---->点击"生成报告"按钮---->ControlWidget发送信号给CentralModel----> 
							   CentralModel计算完要报告数据结构后发送信号给CentralWidget---->CentralWidget显示
							   
2017-6-6： 重新设置ControlWidget 的styleSheet， 添加复位按钮和生成报告按钮
			去掉menuBar,重新设置TollBar的图标
			解决close()无法退出进程问题， 关闭时OpsTrackingDevice 的关闭连接问题
			
2017-6-8: 验证了垂足A,B的正确性，但是公垂线长度不对。原因：3D场景坐标系与加速器坐标系的长度不一致，不能靠简单的缩放0.01倍来加载到3D场景里显示
		  验证激光灯中心位置的检测，基本正确
		  画机架轴线和治疗床轴线时将其轨迹圆中心也画了出来。
		  绘制激光中心，淡蓝色小球表示
		  
2017-6-10： 修改机架和治疗床轴线、圆心绘制方式，公垂线距离问题还没解决
			重写BasePlot,详见PlotView.h
			添加lnac_zh.ts，但还没有把所有字符全部改为tr()
			
2017-6-17: 公垂线距离正常
		   解决PlotView更新速度曲线问题，抛弃函数内使用static变量方法
		   增加LightCenterHandler类， 用于检测准直器模拟光野的十字中心
		   所有功能基本实现
		   
2017-6-20： 笔记本上重新编译ITK，不会出现 mSerialCommunication析构时报错，奇怪，难道真的是我台式机上的ITK变异的有问题？？？
			修改渲染引擎为D3D9_RenderSystem。在其他电脑上测试时发现OpenGL渲染加载场景太慢(主要是win10 OpenGL版本太高，Ogre支持的不好)
			改为D3D9_RenderSystem后，凡是使用本软件的电脑要安装directx
			
2018-01-03: 迁移工程到win10，修改渲染引擎为D3D11,一直无法正常显示渲染界面.可能是因为本机DirectX版本12高于D3D11.

2018-01-30： 为应用添加图标；

2018-03-02： 添加class CBCTHandler;
			 修改部分signals，slots;
			 
2018-03-28：完成所有文字的翻译工作;
			软件打包，放在笔记本上会出现OPENGL初始化不成功的现象。难道自己编译的ogre都会出现OPENGL在其他机器上不支持的问题？
			
2018-04-17：给BedHandler、GantryHandler、CbctHandler、CollimitorHandler添加m_BaseAngle成员，用于记录初始位置时小球所在圆上的初始角度。
			这样就可以不考虑小球贴在仪器上的位置，但是要求仪器必须从零位开始运动，因为我们解算出的角度实际上是角度差。
			
2018-05-11：cbctpositionhandler中计算CBCT面板平面跟注册水平面之间夹角时没有转换为角度，已修改；
			centralWidget中将添加BCT注册完成的message弹窗;
			

2018-05-24： 将GantryHandle、BedHandle的 m_angleC2HContainer直接设置为每一次解算出的旋转轴与水平面的夹角，以前是加权平均的办法算旋转平面夹角，会出现首次拟合出错时影响之后结果的现象。

2018-08-11:  删除任何CBCT相关的cbctHandler和DisplayWidget、PlotWidget, 替换成EpidHandler, EpidHandler与CbctPositionHandler共用同一个DisplayWidget;

2018-08-12:  将检测光野中心替换为用定位球法检测激光等中心，显示界面上是“检测激光等中心定位球法”，代码里任然是LightCenterHandler,因为激光等中心==光野中心;
			  将GantryHandler、CollimatorHandler、BedHandler中旋转角度为负值的角度 +=360;
			  FitCircle中添加你和阈值：50mm
			 