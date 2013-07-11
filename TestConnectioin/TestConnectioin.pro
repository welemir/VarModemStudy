#-------------------------------------------------
#
# Project created by QtCreator 2013-07-04T10:36:37
#
#-------------------------------------------------

CONFIG += release
QT += testlib
QT += core network
equals(QT_MAJOR_VERSION, 4) {
    qtAddLibrary(QtSerialPort)
} else {
    QT += widgets serialport
}
QT       -= gui

TARGET = TestConnectioin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ..

SOURCES +=  \
    main.cpp \
    ../BitBusPipes/cbitbuspipes.cpp \
    ../BitBusPipes/PipeMgr.cpp \
    ../BitBusPipes/USB_Communicator.cpp \
    ../programsettings.cpp \
    ../BitBusPipes/ctcp_client_communicator.cpp \
    ../BitBusPipes/CCommunicator.cpp \
    ../connectioncontrol.cpp \
    TestDeviceExistance.cpp \
    ../ctransceiver.cpp \
    TestHelper.cpp \
    TestDeviceStatus.cpp

HEADERS += \
    ../BitBusPipes/cbitbuspipes.h \
    ../BitBusPipes/PipeMgr.h \
    ../BitBusPipes/CommunicationStructs.h \
    ../BitBusPipes/USB_Communicator.h \
    ../programsettings.h \
    ../BitBusPipes/ctcp_client_communicator.h \
    ../BitBusPipes/CCommunicator.h \
    ../connectioncontrol.h \
    ../CommandCode_RadioModem.h \
    ../CommandCode.h \
    ../CommandCode_RadioModem.h \
    TestDeviceExistance.h \
    ../ctransceiver.h \
    TestHelper.h \
    TestDeviceStatus.h
