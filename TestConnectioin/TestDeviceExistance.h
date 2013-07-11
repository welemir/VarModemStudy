#ifndef TESTDEVICEEXISTANCE_H
#define TESTDEVICEEXISTANCE_H

#include <QObject>


#include "TestHelper.h"
class CTransceiver;
class TestDeviceExistance : public QObject
{
    Q_OBJECT

public:
    TestDeviceExistance();

// тест подключения устройств
private slots:
    void initTestCase();
    void testReceiverConnected();
    void testTransmitterConnected();
    void sendPacket();
};


#endif // TESTDEVICEEXISTANCE_H
