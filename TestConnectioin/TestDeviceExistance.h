#ifndef TESTDEVICEEXISTANCE_H
#define TESTDEVICEEXISTANCE_H
#include "TestBase.h"

#include <QObject>

#include "TestHelper.h"

class CTransceiver;
class TestDeviceExistance : public TestBase
{
    Q_OBJECT

public:
    TestDeviceExistance(QObject *parent = 0);

// тест подключения устройств
private slots:
    void init();
    void receiverConnected();
    void transmitterConnected();
};


#endif // TESTDEVICEEXISTANCE_H
