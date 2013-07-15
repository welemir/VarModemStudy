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
    QTest::qWait(3000);
}

void TestDeviceExistance::testReceiverConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();

    bool bExpected = true;

    qDebug() << "isRxConnected :" << testHelper->isRxConnected();
    QCOMPARE(testHelper->isRxConnected(), bExpected);

    // Тест на подключение прошли, проинициализируем ресивер
    testHelper->attachReceiver();
}

void TestDeviceExistance::testTransmitterConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();

    bool bExpected = true;
    QCOMPARE(testHelper->isTxConnected(), bExpected);

    // Тест на подключение прошли, проинициализируем трансмиттер
    testHelper->attachTransmitter();
}
