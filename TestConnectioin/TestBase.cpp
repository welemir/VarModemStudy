#include "TestBase.h"

#include <QSignalSpy>
#include <QDebug>
#include <QTest>

#include "TestHelper.h"

TestBase::TestBase(QObject *parent) :
    QObject(parent)
{
}

void TestBase::uploadAllSettingsCombinations(int packetsDelay, int waitDelay)
{
    TestHelper *testHelper = TestHelper::getInstance();

    CTransceiver *pRecv = testHelper->receiver();
    CTransceiver *pTransm = testHelper->transmitter();

    foreach(int modulation, testHelper->modulationTypeList())
    {
        foreach(int speed, testHelper->connectionSpeedList())
        {
            foreach(int power, testHelper->txPowerList())
            {
                QList<TCommand_RadioModem> commands;
                commands.append(eModulationTypeSet);
                commands.append(eModulationSpeedSet);
                commands.append(eTxPowerSet);

                QList<int> values;
                values.append(modulation);
                values.append(speed);
                values.append(power);

//                qDebug() << "Setting up Receiver:";
                QList<QSignalSpy *> recvSpyes = testHelper->askTransceiver(pRecv, commands, values, packetsDelay);
//                qDebug() << "Setting up Transmitter";
                QList<QSignalSpy *> transmSpyes = testHelper->askTransceiver(pTransm,commands, values, packetsDelay);

                QTest::qWait(waitDelay);

                qDebug() << "Sent << " << values;

//                qDebug() << "Receiver verification results:";
                for (int i = 0; i < recvSpyes.count(); i++)
                {
                    QSignalSpy *spy = recvSpyes.at(i);
//                    qDebug() << "signals catched: " << spy->count();
                    QVERIFY(spy->count() >= 1);
                    QList<QVariant> args = spy->takeFirst();

  //                  qDebug() << "Arguments in signal: " << args.count();
                    int res = args.at(0).toInt();
                    qDebug() << res;
                //    QVERIFY(values.at(i) == res);
                }
//                qDebug() << "Transmitter verification results:";
                foreach(QSignalSpy *spy, transmSpyes)
                {
    //                qDebug() << "signals catched: " << spy->count();
                    QVERIFY(spy->count() >= 1);
                    QList<QVariant> args = spy->takeFirst();
  //                  qDebug() << "Arguments in signal: " << args.count();
                    int res = args.at(0).toInt();
                    qDebug() << res;
                   // QVERIFY(values.contains(res));
                }
            }
        }
    }
}
