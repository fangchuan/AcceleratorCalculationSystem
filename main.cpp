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
    ok = ok && QFile::exists("./Resources/data/HorizontalRegister.dat");

    return ok;
}

int main(int argc, char *argv[])
{

	QtSingleApplication::setOrganizationName(QStringLiteral("Focus"));
	QtSingleApplication::setOrganizationDomain(QStringLiteral("jsfocus.cn"));
	QtSingleApplication::setApplicationName(QStringLiteral("Accelerator Calibration System"));
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
		QMessageBox::critical(Q_NULLPTR, QCoreApplication::applicationName(), QObject::tr("缺少必要的文件，软件无法正常工作"));
    }

    if (app.isRunning())
    {
        return !app.sendMessage("activate");
    }

    QScopedPointer<QSplashScreen> splashScreen;

    QPixmap pixmap(":/Resources/image/SplashScreen.png");
    splashScreen.reset(new QSplashScreen(pixmap));
	splashMessage(splashScreen, QObject::tr("初始化..."));
    splashScreen->show();
	splashMessage(splashScreen, QObject::tr("初始化工作目录..."));

// 	initWorkEnvironment();

	splashMessage(splashScreen, QObject::tr("正在寻找硬件..."));
    if (OpsTrackingDevice::getInstance()->findTracker() == NDIPolaris)
    {
		splashMessage(splashScreen, QObject::tr("已经找到硬件，正在初始化..."));
        OpsTrackingDevice::getInstance()->openConnection();
	}else{
		splashMessage(splashScreen, QObject::tr("没有找到硬件!"));
        QMessageBox::warning(Q_NULLPTR, QCoreApplication::applicationName(), QObject::tr("未找到相机，请检查相机连接！"));
    }
	splashMessage(splashScreen, QObject::tr("正在创建用户界面..."));

    MainWindow w;
    app.setActivationWindow(&w);
	w.setWindowFlags(w.windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

	splashMessage(splashScreen, QObject::tr("程序已经启动."));
	splashScreen->finish(&w);
	w.showFullScreen();
    return app.exec();

}

