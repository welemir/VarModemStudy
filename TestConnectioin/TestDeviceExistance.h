#ifndef TESTDEVICEEXISTANCE_H
#define TESTDEVICEEXISTANCE_H

#include <QObject>

#include "TestHelper.h"

class CTransceiver;
class TestDeviceExistance : public QObject
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
