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
	QAction*  mClearAction;
	QAction*  mHelpAction;

};

#endif // MAINWINDOW_H
