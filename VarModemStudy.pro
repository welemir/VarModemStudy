#-------------------------------------------------
#
# Project created by QtCreator 2012-08-27T17:41:57
#
#-------------------------------------------------

QT       += core gui network
equals(QT_MAJOR_VERSION, 4) {
    qtAddLibrary(QtSerialPort)
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
    connectioncontrol.cpp \
    dialogaboutprogram.cpp \
    dialogsettings.cpp \
    dialogdiagnostics.cpp \
    ctransceiver.cpp \
    cstatuswidget.cpp \
    dialogstatistics.cpp \
    formbitserrorsinbytes.cpp \
    modelanalizedata.cpp

HEADERS  += mainwindow.h \
    ckernel.h \
    BitBusPipes/cbitbuspipes.h \
    BitBusPipes/PipeMgr.h \
    BitBusPipes/CommunicationStructs.h \
    BitBusPipes/USB_Communicator.h \
    programsettings.h \
    BitBusPipes/ctcp_client_communicator.h \
    BitBusPipes/CCommunicator.h \
    connectioncontrol.h \
    dialogaboutprogram.h \
    dialogsettings.h \
    dialogdiagnostics.h \
    ctransceiver.h \
    CommandCode_RadioModem.h \
    CommandCode.h \
    CommandCode.h \
    CommandCode_RadioModem.h \
    cstatuswidget.h \
    dialogstatistics.h \
    formbitserrorsinbytes.h \
    modelanalizedata.h

FORMS    += mainwindow.ui \
    dialogaboutprogram.ui \
    dialogsettings.ui \
    dialogdiagnostics.ui \
    cstatuswidget.ui \
    dialogstatistics.ui \
    formbitserrorsinbytes.ui

RESOURCES += \
    VarControl.qrc

VERSION = 01.01.01

VERSTR = '\\"$${VERSION}\\"'  # кавычки
DEFINES += VER=\"$${VERSTR}\" # макрос
