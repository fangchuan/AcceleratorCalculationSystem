#-------------------------------------------------
#
# Project created by QtCreator 2017-05-15T14:41:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network concurrent

TARGET = LNAC
TEMPLATE = app

HEADERS  += mainwindow.h\
            ACSConfigure.h \
            ACSConstants.h \
            ACSUtils.h \
            Circle.h \
            HorizontalRegister.h \
            qtlockedfile.h \
            TranslationDisplayWidget.h \
            RotationDisplayWidget.h \
            AbstractMonitorHandler.h \
            OpsTrackingDevice.h \
            CentralModel.h \
            GantryHandler.h \
            Fit3DCircle.h \
            BedHandler.h \
            CollimatorHandler.h \
            HorizontalRegisterHandler.h \
            HorizontalRegisterDisplayWidget.h \
            ISOCenterDisplayWidget.h \
            ISOCenterHandler.h \
            qtsinglecoreapplication.h \
            qtsingleapplication.h \
            qtlocalpeer.h \
            ControlWidget.h \
            DisplayWidget.h \
            CentralWidget.h \
            trackingdevice.h \
            trackingtool.h \
            trackingtypes.h \
            vpscommon.h \
            vpsigthardwareexception.h \
            vpsigttimestamp.h \
            vpsinternaltrackingtool.h \
            vpsndipassivetool.h \
            vpsndiprotocol.h \
            vpsnditrackingdevice.h \
            vpsrealtimeclock.h \
            vpsserialcommunication.h \
            vpsvector.h \
            vpswindowsrealtimeclock.h

SOURCES += main.cpp\
        mainwindow.cpp\
        AbstractMonitorHandler.cpp \
        ACSUtils.cpp \
        BedHandler.cpp \
        CentralModel.cpp \
        CentralWidget.cpp \
        Circle.cpp \
        CollimatorHandler.cpp \
        ControlWidget.cpp \
        DisplayWidget.cpp \
        Fit3DCircle.cpp \
        GantryHandler.cpp \
        HorizontalRegister.cpp \
        HorizontalRegisterDisplayWidget.cpp \
        HorizontalRegisterHandler.cpp \
        ISOCenterDisplayWidget.cpp \
        ISOCenterHandler.cpp \
        OpsTrackingDevice.cpp \
        qtlocalpeer.cpp \
        qtsingleapplication.cpp \
        qtsinglecoreapplication.cpp \
        RotationDisplayWidget.cpp \
        TranslationDisplayWidget.cpp\
        trackingdevice.cpp\
        trackingtool.cpp\
        vpsigttimestamp.cpp \
        vpsinternaltrackingtool.cpp \
        vpsndipassivetool.cpp \
        vpsndiprotocol.cpp \
        vpsnditrackingdevice.cpp \
        vpsrealtimeclock.cpp \
        vpsserialcommunication.cpp \
        vpsvector.cpp \
        vpswindowsrealtimeclock.cpp


CONFIG(debug, debug|release){

    INCLUDEPATH += F:\VTKandITK\VTK\vtk\debug\include\vtk-7.0
    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\arpack
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\blas
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\compilers
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\core\testlib
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\datapac
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\eispack
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\gdcmcharls
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\gdcmjpeg
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\internal
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\itkfdstream
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\itkhdf5
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\itkjpeg
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\itkkwiml
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\itkpng
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\itksys
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\itkzlib
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\lapack\complex16
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\lapack\double
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\lapack\single
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\lapack\util
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\laso
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\linalg
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\linpack
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\mathews
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\minpack
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\napack
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\opt
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\socketxx
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\sparse
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\temperton
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\toms
##    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\vcl_sys
#    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\debug\include\ITK-4.11\vnl

    LIBS += -LF:\VTKandITK\VTK\vtk\debug\lib  -lvtkCommonTransforms-7.0 -lvtkCommonCore-7.0 -lvtkCommonMath-7.0 -lvtksys-7.0
    LIBS += -LF:\VTKandITK\ITK\itk-4.11\debug\lib  -litksys-4.11 -lITKCommon-4.11  -litkvnl-4.11  -litkvcl-4.11

}else{

    INCLUDEPATH += F:\VTKandITK\VTK\vtk\release\include\vtk-7.0
    INCLUDEPATH += F:\VTKandITK\ITK\itk-4.11\release\include\ITK-4.11

    LIBS += -L"F:\VTKandITK\VTK\vtk\release\lib"  -lvtkCommonTransforms-7.0 -lvtkCommonCore-7.0 -lvtkCommonMath-7.0 -lvtksys-7.0
    LIBS += -L"F:\VTKandITK\ITK\itk-4.11\release\lib"  -lITKCommon-4.11  -litksys-4.11  -litkvnl-4.11  -litkvcl-4.11

}

FORMS    += mainwindow.ui

RESOURCES += ACS.qrc \
    acs.qrc


