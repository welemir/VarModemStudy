#-------------------------------------------------
#
# Project created by QtCreator 2012-08-27T17:41:57
#
#-------------------------------------------------

QT       += core gui network
equals(QT_MAJOR_VERSION, 4) {
    include($$PWD/QtAddOnSerialPort/qt4support_serialport.prf)
} else {
    QT += widgets serialport
}

TARGET = VarModemStudy
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ckernel.cpp \
    BitBusPipes/cbitbuspipes.cpp \
    BitBusPipes/PipeMgr.cpp \
    BitBusPipes/USB_Communicator.cpp \
    programsettings.cpp \
    BitBusPipes/ctcp_client_communicator.cpp \
    BitBusPipes/CCommunicator.cpp \
    connectioncontrol_ui.cpp \
    connectioncontrol.cpp \
    dialogaboutprogram.cpp \
    dialogsettings.cpp \
    dialogdiagnostics.cpp

HEADERS  += mainwindow.h \
    ckernel.h \
    BitBusPipes/cbitbuspipes.h \
    BitBusPipes/PipeMgr.h \
    BitBusPipes/CommunicationStructs.h \
    BitBusPipes/USB_Communicator.h \
    programsettings.h \
    BitBusPipes/ctcp_client_communicator.h \
    BitBusPipes/CCommunicator.h \
    connectioncontrol_ui.h \
    connectioncontrol.h \
    dialogaboutprogram.h \
    dialogsettings.h \
    dialogdiagnostics.h

FORMS    += mainwindow.ui \
    connectioncontrol_ui.ui \
    dialogaboutprogram.ui \
    dialogsettings.ui \
    dialogdiagnostics.ui

RESOURCES += \
    VarControl.qrc
