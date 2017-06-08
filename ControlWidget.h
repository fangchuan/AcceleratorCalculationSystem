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
	void reset();
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
	void resetRequest();
	void reportRequest();
private:
	void initUi();
	void buildConnections();
	void resetHorizontalRegister();
	void resetGantry();
	void resetCollimator();
	void resetBed();
	void resetISOCenter();
	void setButtonStyle();
private slots:
	void handleButtonClick();
	void handleButtonGroupClick(int id);

private:
	QPushButton* m_HorizontalRegisterButton;
	QPushButton* m_GantryButton;
	QPushButton* m_CollimatorButton;
	QPushButton* m_BedButton;
	QButtonGroup* m_BedGroup;
	QPushButton* m_LaserButton;
	QPushButton* m_ResetButton;
	QPushButton* m_ReportButton;
};
