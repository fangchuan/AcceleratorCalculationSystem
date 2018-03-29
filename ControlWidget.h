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
	void doSwitchToCbct();
	void doSwitchToCbctPosition();
	void doSwitchToBed();
	void doSwitchToLaserISO();
	void doSwitchToLightCenter();
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
	void switchToCbct();
	void recordingCbct();
	void switchToCbctPosition();
	void recordingCbctPosition();
	void switchToLaserISO();
	void recordingLaserISO();
	void switchToLightCenter();
	void recordingLightCenter();
	void resetRequest();
	void reportRequest();
private:
	void initUi();
	void buildConnections();
	void resetHorizontalRegister();
	void resetGantry();
	void resetCollimator();
	void resetBed();
	void resetCbct();
	void resetCbctPosition();
	void resetLaserISO();
	void resetLightCenter();
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
	QPushButton* m_CbctButton;
	QPushButton* m_CbctPositionButton;
	QPushButton* m_LaserButton;
	QPushButton* m_LightButton;
	QPushButton* m_ResetButton;
	QPushButton* m_ReportButton;
};
