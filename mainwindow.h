#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CentralWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    //初始化window
	void initUi();
	//初始化File toolbar
	void initFileActions();
	//初始化 view toolbar
	void initViewActions();
	// 初始化 help toolbar
	void initHelpAction();
	//void setupTextActions();
    // 建立信号槽
	void buildConnections();
    // 断开相机
	void disconnectTracker();
	// 打开帮助文档....
	void showHelp();
	// 关闭系统
	void closeWindow();
	//检测相机是否连接
	void detectTracker();


private:

    Ui::MainWindow *ui;
    CentralWidget *m_CentralWidget;

	QAction*  mThreeDAction;
	QAction*  mPlotAction;
	QAction*  mExportAction;
	QAction*  mExportDegreePlotAction;
	QAction*  mExportDegreeVelPlotAction;
	QAction*  mExportDistancePlotAction;
	QAction*  mExportDistanceVelPlotAction;
	QAction*  mSaveAction;
	QAction*  mSaveAsAction;
	QAction*  mPrintAction;
	QAction*  mPreviewAction;
	QAction*  mQuit;
	QAction*  mUndoAction;
	QAction*  mRedoAction;
	QAction*  mClearAction;
	QAction*  mTextAction;
	QAction*  mHelpAction;

	bool  mTrackerDetected;
};

#endif // MAINWINDOW_H
