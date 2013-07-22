#-------------------------------------------------
#
# Project created by QtCreator 2013-07-22T11:07:47
#
#-------------------------------------------------

QT       += core testlib network
QT       -= gui

equals(QT_MAJOR_VERSION, 4) {
    qtAddLibrary(QtSerialPort)
} else {
    QT += widgets serialport
}

TARGET = tst_testpacketsanalysetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_testpacketsanalysetest.cpp \
    BitBusPipes/cbitbuspipes.cpp \
    BitBusPipes/PipeMgr.cpp \
    BitBusPipes/USB_Communicator.cpp \
    programsettings.cpp \
    BitBusPipes/ctcp_client_communicator.cpp \
    BitBusPipes/CCommunicator.cpp \
    connectioncontrol.cpp \
    ctransceiver.cpp \
    ckernel.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ckernel.h \
    BitBusPipes/cbitbuspipes.h \
    BitBusPipes/PipeMgr.h \
    BitBusPipes/CommunicationStructs.h \
    BitBusPipes/USB_Communicator.h \
    programsettings.h \
    BitBusPipes/ctcp_client_communicator.h \
    BitBusPipes/CCommunicator.h \
    connectioncontrol.h \
    ctransceiver.h \
    CommandCode.h \
    CommandCode_RadioModem.h \
