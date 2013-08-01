#include "TestCommunicationStability.h"

#include <QTest>

#include "TestHelper.h"
#include <QDebug>

TestCommunicationStability::TestCommunicationStability(QObject *parent) :
    TestBase(parent)
{
}

void TestCommunicationStability::fillList(QList<int> &list, const int &value, const int &count)
{
    for (int i = 0; i < count; i++)
    {
        list.append(static_cast<TCommand_RadioModem>(value));
    }
}

void TestCommunicationStability::TestMaximumContinuouslySettingsCount()
{
    qWarning() << "---- Trying to send settings by groups ----";
    TestHelper *testHelper = TestHelper::getInstance();
    /*for (int commandsCounter = 0; commandsCounter < testHelper->modulationTypeList().count(); commandsCounter++)
    {*/
        for (int count = 3; count < 10; count ++)
        {
            QList<int> commands;
            QList<int> values;

            fillList(commands, eModulationTypeSet, count);
            fillList(values, testHelper->modulationTypeList().at(1), count);

            // отправка настроек в модем. Между пакетами нет задержки
            uploadSettings(commands, values);

            // анализ результатов
            checkOperationResult(eModulationTypeSet, values);
        }
    //}
}

void TestCommunicationStability::prepareDevicesForRawDataTransfer()
{
    qWarning() << "preparing devices for RAW data communication stability tests";
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

void TestCommunicationStability::testReceivePackets()
{
    QList<QString> logMessages;
    logMessages.append(QString("w---- Receiving packets test ----"));

    qDebug() << "---- Receiving packets test ----";
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
    int iRawPacketsToCatch = 150;
    testHelper->startRxMeasure(&RxIntervalsList);

    QElapsedTimer timeoutTimer;

    float expectedTime = 128.*8*iRawPacketsToCatch/connectionSpeed;
    timeoutTimer.start();

    logMessages.append(QString("lWaiting for %1 second(s)").arg(expectedTime));
    bool bExtendedWaitingTime = false;
    do
    {
        if (timeoutTimer.elapsed()/1000 >= expectedTime)
        {
            if (!bExtendedWaitingTime)
            {
                logMessages.append(QString("wFAILED: not enought data during expected time."));
                // процент количества полученных пакетов
                float receivedPercent = RxIntervalsList.count()/iRawPacketsToCatch;
                bExtendedWaitingTime = true;

                logMessages.append(QString("ltrying to wait %1 second(s) more").arg(expectedTime*bExtendedWaitingTime));
                expectedTime += expectedTime*(1+receivedPercent);
                continue;
            }

            logMessages.append(QString("lExtended time out. There is still not enought data."));

            foreach(QString s, logMessages)
            {
                if (s.startsWith("l"))
                    log() << s.remove(0,1);
                if (s.startsWith("w"))
                    qWarning() << s.remove(0,1);
            }
            qWarning() << RxIntervalsList.count() << "packets received (" << RxIntervalsList.count()/iRawPacketsToCatch << "% )";

            break;
        }

        QTest::qWait(1);
    }while (RxIntervalsList.count() < iRawPacketsToCatch);

    QVERIFY(RxIntervalsList.count() >= iRawPacketsToCatch);

    testHelper->stopRxMeasure();
    Rx->slotRxStop();

    int timeTotal = 0;
    foreach(int t, RxIntervalsList)
    {
        timeTotal+=t;
    }

    float measuredDataRate = testHelper->rxRawDataSize()/(timeTotal/1000.)*8;
    float allowedInacuracy = 0.05;
    bool dataRateIsFine = measuredDataRate*(1+allowedInacuracy) >= effectiveConnectionSpeed && effectiveConnectionSpeed >= measuredDataRate*(1-allowedInacuracy);

    if (!dataRateIsFine)
    {
        foreach(QString s, logMessages)
        {
            if (s.startsWith("l"))
                log() << s.remove(0,1);
            if (s.startsWith("w"))
                qWarning() << s.remove(0,1);
        }
        qWarning() << "measured data rate is:" << measuredDataRate << "but" << connectionSpeed << "expected";
    }

    QVERIFY(dataRateIsFine);
}


void TestCommunicationStability::testTransmitPackets()
{
    qWarning() << "---- Transmitting packets test ----";
    TestHelper *testHelper = TestHelper::getInstance();

    int iPacketDataLength = 50;
    QByteArray newPacket;
    for( int i = 0; i < iPacketDataLength; i++)
        newPacket.append(qrand());

    CTransceiver *Tx = testHelper->transmitter();

    Tx->slotSetDataPacketLength(iPacketDataLength);
    Tx->appendCrc(&newPacket);

    int iPacketsToSend = 150;
    for (int i = 0; i < iPacketsToSend; i++)
    {
        Tx->slotAppendRawPacket(newPacket);
    }

    QList<int> TxStatusList;
    //connect(Tx, SIGNAL(signalNewRawPacket(QByteArray, unsigned short)), testHelper, SLOT(slotTxRawDataCame()));

    int serviceDataSize, connectionSpeed;
    Tx->getTranscieverStatistics(iPacketDataLength, serviceDataSize, connectionSpeed);
    QElapsedTimer timeoutTimer;
    float expectedTime = 128.*8*iPacketsToSend/connectionSpeed;

    testHelper->startTxMeasure(&TxStatusList);
    timeoutTimer.start();
    do
    {
        if (timeoutTimer.elapsed()/1000 >= expectedTime)
        {
            qWarning() << "FAILED!" << TxStatusList.count() << "packets sent (" << TxStatusList.count()/Tx->packetsToSend()*100. << "% )";
            break;
        }
        QTest::qWait(1);
    }while (TxStatusList.count() < Tx->packetsToSend());

    int PacketsSent = -1;
    foreach (int i, TxStatusList)
    {
        PacketsSent+=i;
    }
    PacketsSent /=2 ;
    Tx->slotTxStop();
    QVERIFY(PacketsSent == iPacketsToSend && 0 == Tx->packetsToSend());

    log() << "ok";
}
