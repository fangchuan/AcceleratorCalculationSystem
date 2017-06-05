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