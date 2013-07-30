#include "TestPacketSend.h"

#include <QTest>
#include <QDebug>

#include "TestHelper.h"


TestPacketSend::TestPacketSend(QObject *parent) :
    TestBase(parent)
{
}

void TestPacketSend::prepareDevices()
{
    TestHelper *testHelper = TestHelper::getInstance();


    QList<int> commands;
    QList<int> values;

    commands.append(eModulationTypeSet);
    values.append(testHelper->modulationTypeList().at(0));

    commands.append(eModulationSpeedSet);
    values.append(testHelper->connectionSpeedList().at(3));

    commands.append(eTxPowerSet);
    values.append(testHelper->txPowerList().at(0));

    uploadSettings(commands, values);

    checkOperationResult(commands, values);

//    QSignalSpy retryTxSpy(testHelper->transmitter(), SIGNAL(signalCommandSendRetry()));
//    QSignalSpy retryRxSpy(testHelper->receiver(), SIGNAL(signalCommandSendRetry()));
}

void TestPacketSend::testReceivePackets()
{
    TestHelper *testHelper = TestHelper::getInstance();

    int iPacketDataLength = 50;
    CTransceiver *Rx = testHelper->receiver();

    Rx->slotSetDataPacketLength(iPacketDataLength);

    QList<int> RxIntervalsList;

    int serviceDataSize, connectionSpeed;
    Rx->getTranscieverStatistics(iPacketDataLength, serviceDataSize, connectionSpeed);
    float payloadPercent = 100.*iPacketDataLength /(iPacketDataLength + serviceDataSize);
    float effectiveConnectionSpeed = payloadPercent*connectionSpeed/100;

    // ожидаем прихода пакетов сплошных данных
    int iRawPacketsToCatch = 1000;

    testHelper->startRxMeasure(&RxIntervalsList);

    do
    {
        QTest::qWait(1);
    }while (RxIntervalsList.count() < iRawPacketsToCatch);

    testHelper->stopRxMeasure();
    Rx->slotRxStop();

    int timeTotal = 0;
    foreach(int t, RxIntervalsList)
    {
        timeTotal+=t;
    }


//    qDebug() << RxIntervalsList;
//    qDebug() << "Rx Packets Came" << RxIntervalsList.count();

    float measuredDataRate = testHelper->rxRawDataSize()/(timeTotal/1000.)*8;

    qDebug() << "total data came:" << testHelper->rxRawDataSize();
    qDebug() << "measure time: " << timeTotal/1000. <<  "seconds";
    qDebug() << "connection speed:" << connectionSpeed;
    qDebug() << "effective:" << measuredDataRate;


    float allowedInacuracy = 0.05;
    bool dataRateIsFine = measuredDataRate*(1+allowedInacuracy) >= effectiveConnectionSpeed && effectiveConnectionSpeed >= measuredDataRate*(1-allowedInacuracy);

    if (!dataRateIsFine)
        qWarning() << "measured data rate is:" << measuredDataRate << "but" << connectionSpeed << "expected";

    QVERIFY(dataRateIsFine);
}

void TestPacketSend::sendPackets()
{
    TestHelper *testHelper = TestHelper::getInstance();

    int iPacketDataLength = 50;
    QByteArray newPacket;
    for( int i = 0; i < iPacketDataLength; i++)
        newPacket.append(qrand());

    CTransceiver *Tx = testHelper->transmitter();
    CTransceiver *Rx = testHelper->receiver();

    Rx->slotSetDataPacketLength(iPacketDataLength);
    Tx->slotSetDataPacketLength(iPacketDataLength);
    Tx->appendCrc(&newPacket);



    int iPacketsToSend = 100;
    for (int i = 0; i < iPacketsToSend; i++)
    {
        Tx->slotAppendRawPacket(newPacket);
    }

    QList<int> RxIntervalsList;
    QList<int> TxIntervalsList;
    //connect(Tx, SIGNAL(signalNewRawPacket(QByteArray, unsigned short)), testHelper, SLOT(slotTxRawDataCame()));

    testHelper->startRxMeasure(&RxIntervalsList);
    testHelper->startTxMeasure(&TxIntervalsList);
/*
    int payloadDataSize, serviceDataSize, connectionSpeed;
    Tx->getTranscieverStatistics(payloadDataSize, serviceDataSize, connectionSpeed);
    float payloadPercent = (100. * payloadDataSize )/(payloadDataSize + serviceDataSize);
    float effectiveConnectionSpeed = payloadPercent*connectionSpeed/100;
*/
    do
    {
//        if (iPacketsToSend >= RxIntervalsList.count())
//            break;
        //else
            QTest::qWait(100);
    }while (TxIntervalsList.count() < iPacketsToSend);

    Rx->slotRxStop();

    qDebug() << RxIntervalsList << TxIntervalsList;
    qWarning() << "Rx Packets Came" << RxIntervalsList.count();
    //QCOMPARE(iPacketsToSend, RxIntervalsList.count());

    Tx->slotTxStop();
}

