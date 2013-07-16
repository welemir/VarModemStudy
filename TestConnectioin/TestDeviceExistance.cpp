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
    TestHelper::getInstance()->initDevices();

    qDebug() << "Waiting until devices starts";
    QTest::qWait(3000);
}
void TestDeviceExistance::receiverConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();

    bool bExpected = true;

    qDebug() << "isRxConnected :" << testHelper->isRxConnected();
    QCOMPARE(testHelper->isRxConnected(), bExpected);

    // ���� �� ����������� ������, ����������������� �������
    m_Receiver = new CTransceiver(testHelper);
    testHelper->attachReceiver(m_Receiver);
}

void TestDeviceExistance::transmitterConnected()
{
    TestHelper *testHelper = TestHelper::getInstance();

    bool bExpected = true;
    QCOMPARE(testHelper->isTxConnected(), bExpected);

    // ���� �� ����������� ������, ����������������� �����������
    m_Transmitter = new CTransceiver(testHelper);
    testHelper->attachTransmitter(m_Transmitter);
}
