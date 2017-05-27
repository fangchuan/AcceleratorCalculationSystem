#include "mainwindow.h"
//#include <QApplication>
#include "ACSConfigure.h"
#include "ACSConstants.h"
#include "ACSUtils.h"
#include "OpsTrackingDevice.h"
#include "qtsingleapplication.h"
#include <QTextCodec>
#include <QSplashScreen>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>



void splashMessage(QScopedPointer<QSplashScreen>& splashScreen, const QString& message)
{
    if (splashScreen.isNull())
    {
        return;
    }
    splashScreen->showMessage(message, Qt::AlignBottom | Qt::AlignLeft);
    //splashScreen->repaint();
}

bool checkEnvironment()
{
    bool ok = true;
    ok = ok && QFile::exists(":/Resources/dat/HorizontalRegister.dat");

    return ok;
}

#ifdef  __cplusplus
extern "C" {
#endif

int main(int argc, char *argv[])
{

	QtSingleApplication::setOrganizationName("Focus");
	QtSingleApplication::setOrganizationDomain("jsfocus.cn");
	QtSingleApplication::setApplicationName("Accelerator Calibration System");
	QtSingleApplication::setApplicationVersion(ACS_VERSION);
	QtSingleApplication::setDesktopSettingsAware(false);
	QtSingleApplication app(argc, argv);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

#ifdef LICENSE
    if (!ACSUtils::CheckSoftwareLicense())
    {
        exit(-1);
        return false;
    }
#endif

    //Check Registry
#ifdef REGISTRY
    if (!ACSUtils::CheckSoftwareRegistry())
    {
        exit(-1);
        return false;
    }
#endif
    if (!checkEnvironment()) {
		QMessageBox::warning(Q_NULLPTR, QString::fromUtf8("警告"), QString::fromUtf8("缺少必要的文件，软件无法正常工作"));
    }

    if (app.isRunning())
    {
        return !app.sendMessage("activate");
    }

    QScopedPointer<QSplashScreen> splashScreen;

    QPixmap pixmap(":/Resources/image/SplashScreen.png");
    splashScreen.reset(new QSplashScreen(pixmap));
	splashMessage(splashScreen, QString::fromUtf8("初始化..."));
    splashScreen->show();
	splashMessage(splashScreen, QString::fromUtf8("初始化工作目录..."));

// 	initWorkEnvironment();

	splashMessage(splashScreen, QString::fromUtf8("正在寻找硬件..."));
    if (OpsTrackingDevice::getInstance()->findTracker() == NDIPolaris)
    {
        splashMessage(splashScreen, QString::fromUtf8("已经找到硬件，正在初始化..."));
        OpsTrackingDevice::getInstance()->openConnection();
	}
    else
    {
		splashMessage(splashScreen, QString::fromUtf8("没有找到硬件!"));
        QMessageBox::warning(Q_NULLPTR, QString::fromUtf8("警告"), QString::fromUtf8("未找到相机，请检查相机连接！"));
    }

	splashMessage(splashScreen, QString::fromUtf8("正在创建用户界面..."));
    MainWindow w;
    app.setActivationWindow(&w);
    w.showMaximized();
	splashMessage(splashScreen, QString::fromUtf8("程序已经启动."));
    splashScreen->finish(&w);
    return app.exec();
}
#ifdef __cplusplus
}
#endif
