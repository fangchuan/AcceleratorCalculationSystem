#include "mainwindow.h"
#include "acsconfigure.h"
#include "acsconstants.h"
#include "acsutils.h"
#include "opstrackingdevice.h"
#include "qtsingleapplication.h"

#include <QTextCodec>
#include <QSplashScreen>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QTranslator>
#include <QDesktopWidget>
#include "QScopedPointer"
#include <QMutex>
#include <QWaitCondition>
#include "QTime"

//#define REGISTRY
//#define LICENSE

//
void splashMessage(QScopedPointer<QSplashScreen>& splashScreen, const QString& message)
{
    if (splashScreen.isNull())
    {
        return;
    }
    splashScreen->showMessage(message, Qt::AlignBottom | Qt::AlignLeft);
}

//检测水平仪文件
bool checkEnvironment()
{
    bool ok = true;
    ok = ok && QFile::exists("./Resources/data/HorizontalRegister.dat");

    return ok;
}

void msecASleep(quint32 msec)
{
	//QMutex lock;
	//QWaitCondition sleep;
	//lock.lock();
	//sleep.wait(&lock, msec);
	//lock.unlock();
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

}

int main(int argc, char *argv[])
{
#ifdef LICENSE
	if (!ACSUtils::CheckSoftwareLicense())
	{
		exit(-1);
	}
#endif

	//Check Registry
//#ifdef REGISTRY
//	if (!ACSUtils::CheckSoftwareRegistry())
//	{
//		exit(-1);
//	}
//#endif

	//添加翻译文件
	QTranslator ts;
	ts.load("./Resources/language/lnac_zh.qm");
	//singleApplication
	QtSingleApplication::setOrganizationName(QStringLiteral("Focus"));
	QtSingleApplication::setOrganizationDomain(QStringLiteral("jsfocus.cn"));
	QtSingleApplication::setApplicationName(QObject::tr("ACS"));
	QtSingleApplication::setApplicationVersion(ACS_VERSION);
	QtSingleApplication::setDesktopSettingsAware(false);
	QtSingleApplication app(argc, argv);
	app.installTranslator(&ts);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

	if (!checkEnvironment()) {
		QMessageBox::critical(Q_NULLPTR, QCoreApplication::applicationName(), QObject::tr("Lack the necessary files, the software would not work properly!"));
	}

	if (app.isRunning())
	{
		return !app.sendMessage("activate");
	}

	QScopedPointer<QSplashScreen> splashScreen;

	QPixmap pixmap(":/Resources/image/splash_screen.png");
	splashScreen.reset(new QSplashScreen(pixmap));
	splashMessage(splashScreen, QObject::tr("Start initialization..."));
	splashScreen->show();
	splashMessage(splashScreen, QObject::tr("Initialize working directory ..."));

	splashMessage(splashScreen, QObject::tr("initializing..."));

	splashMessage(splashScreen, QObject::tr("Creating user interface ..."));

    MainWindow w;
    app.setActivationWindow(&w);
	w.setWindowFlags(w.windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

	splashMessage(splashScreen, QObject::tr("The program has started"));
	splashScreen->finish(&w);

	w.showFullScreen();

    return app.exec();

}

