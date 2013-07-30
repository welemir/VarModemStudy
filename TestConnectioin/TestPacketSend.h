#ifndef TESTPACKETSEND_H
#define TESTPACKETSEND_H

#include "TestBase.h"

class TestPacketSend : public TestBase
{
    Q_OBJECT
public:
    explicit TestPacketSend(QObject *parent = 0);
    
private slots:
    void prepareDevices();
    void testReceivePackets();
public:
    void sendPackets();
};

#endif // TESTPACKETSEND_H
