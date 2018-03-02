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
			
2018-01-03: 迁移工程到win10，修改渲染引擎为D3D11,一直无法正常显示渲染界面.

2018-01-30： 为应用添加图标；

2018-03-02： 添加class CBCTHandler;
			 修改部分signals，slots;
			