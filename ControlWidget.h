#pragma once

#include <QWidget>

class QPushButton;
class QButtonGroup;

class ControlWidget : public QWidget
{
	Q_OBJECT

public:
	ControlWidget(QWidget *parent = Q_NULLPTR);
	~ControlWidget();

	void doSwitchToHorizontalRegister();
	void doSwitchToGantry();
	void doSwitchToCollimator();
	void doSwitchToBed();
	void doSwitchToISOCenter();

signals:
	void switchToHorizontalRegister();
	void recordingHorizontalRegister();
	void switchToGantry();
	void recordingGantry();
	void switchToCollimator();
	void recordingCollimator();
	void switchToBed(int mode);
	void recordingBed(int mode);
	void switchToISOCenter();
	void recordingISOCenter();

private:
	void initUi();
	void buildConnections();
	void resetHorizontalRegister();
	void resetGantry();
	void resetCollimator();
	void resetBed();
	void resetISOCenter();

private slots:
	void handleButtonClick();
	void handleButtonGroupClick(int id);

private:
	QPushButton *m_HorizontalRegisterButton;
	QPushButton *m_GantryButton;
	QPushButton *m_CollimatorButton;
	QPushButton *m_BedButton;
	QButtonGroup *m_BedGroup;
	QPushButton *m_LaserButton;
};
