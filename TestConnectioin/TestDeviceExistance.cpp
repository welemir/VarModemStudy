#include "TestDeviceExistance.h"


#include <QDebug>
#include <QTest>

#include "TestHelper.h"


#include "ctransceiver.h"

TestDeviceExistance::TestDeviceExistance()
    : QObject()
{
}

void TestDeviceExistance::initTestCase()
{
    //  init devices in global singleton
    TestHelper::getInstance()->initDevices();

    qDebug() << "Waiting until devices starts";
    QTest::qWait(10000);
}

void TestDeviceExistance::testReceiverConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();

    bool bExpected = true;

    qDebug() << "isRxConnected :" << testHelper->isRxConnected();
    if (!QTest::qCompare(testHelper->isRxConnected(), bExpected, "ReceiverConnected", "ReceiverConnected", NULL, 0))
    {
        qDebug() << "Timeout expired:";
        qDebug() << "Seems device is not connected";
        return;
    }

    testHelper->attachReceiver();
}

void TestDeviceExistance::testTransmitterConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();

    bool bExpected = true;
    if (!QTest::qCompare(testHelper->isTxConnected(), bExpected, "TrensmitterConnected", "TrensmitterConnected", NULL, 0))
    {
        qDebug() << "Timeout expired:";
        qDebug() << "Seems device is not connected";
        return;
    }

    testHelper->attachTransmitter();

}

void TestDeviceExistance::sendPacket()
{

}
