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
    //��ʼ��window
	void initUi();
	//��ʼ��File toolbar
	void initFileActions();
	//��ʼ�� view toolbar
	void initViewActions();
	// ��ʼ�� help toolbar
	void initHelpAction();
	//void setupTextActions();
    // �����źŲ�
	void buildConnections();
    // �Ͽ����
	void disconnectTracker();
	// �򿪰����ĵ�....
	void showHelp();
	// �ر�ϵͳ
	void closeWindow();
	//�������Ƿ�����
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
