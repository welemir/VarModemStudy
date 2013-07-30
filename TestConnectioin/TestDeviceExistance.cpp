#include "TestDeviceExistance.h"


#include <QDebug>
#include <QTest>

#include "TestHelper.h"


#include "ctransceiver.h"

TestDeviceExistance::TestDeviceExistance(QObject *parent)
    : TestBase(parent)
{
}

void TestDeviceExistance::init()
{
    //  init devices in global singleton
    qDebug() << "------init TestDeviceExistance------";
    TestHelper::getInstance()->initDevices();

    qDebug() << "Waiting until devices starts";
    QTest::qWait(3000);
}

void TestDeviceExistance::receiverConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();
    log() << "isRxConnected :" << testHelper->isRxConnected();
    QVERIFY(testHelper->isRxConnected());
}

void TestDeviceExistance::transmitterConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();
    log() << "isTxConnected :" << testHelper->isTxConnected();
    QVERIFY(testHelper->isTxConnected());
}
