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
    void initUi();
	void setupFileActions();
	void setupEditActions();
	void setupViewActions();
	void setupHelpAction();
	//void setupTextActions();
    void buildConnections();
    void exitMainWindow();
    void disconnectTracker();
	void showHelp();
private:
    Ui::MainWindow *ui;
    CentralWidget *m_CentralWidget;
	QAction*  mThreeDAction;
	QAction*  mPlotAction;
	QAction*  mExportAction;
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

};

#endif // MAINWINDOW_H
