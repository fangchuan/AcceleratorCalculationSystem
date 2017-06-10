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
#include <QTranslator>
#include <QDesktopWidget>


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
	QTranslator ts;
	ts.load("./Resources/language/lnac_zh.qm");

	QtSingleApplication::setOrganizationName(QStringLiteral("Focus"));
	QtSingleApplication::setOrganizationDomain(QStringLiteral("jsfocus.cn"));
	QtSingleApplication::setApplicationName(QStringLiteral("Accelerator Calibration System"));
	QtSingleApplication::setApplicationVersion(ACS_VERSION);
	QtSingleApplication::setDesktopSettingsAware(false);
	QtSingleApplication app(argc, argv);
	app.installTranslator(&ts);

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

// 	initWorkEnvironment();
	splashMessage(splashScreen, QObject::tr("initializing..."));

	splashMessage(splashScreen, QObject::tr("Creating user interface ..."));

    MainWindow w;
    app.setActivationWindow(&w);
	w.setWindowFlags(w.windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

	splashMessage(splashScreen, QObject::tr("The program has started"));
	splashScreen->finish(&w);

	//QSize size = QSize(QtSingleApplication::desktop()->width(), QtSingleApplication::desktop()->height());
	//w.setFixedSize(size);
	w.showFullScreen();

    return app.exec();

}

