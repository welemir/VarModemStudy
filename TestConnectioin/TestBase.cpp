#include "TestBase.h"

#include <QSignalSpy>
#include <QDebug>
#include <QTest>

#include "TestHelper.h"

TestBase::TestBase(QObject *parent) :
    QObject(parent)
{
}

bool TestBase::checkSignal(const QSignalSpy &spy,const int &value)
{
    qDebug() << spy.objectName();
    if (0 == spy.count())
        return false;

//    QVERIFY(spy.count() >= 1);
    // преребор всех однотипных сигналов, отловленных одним шпионом (например, несколько ответов на установку типа модуляции)
    for (int i = 0; i < spy.count(); i++) // один шпион способен отловить сигнал несколько раз.
    {
        QList<QVariant> args = spy.at(i);
        // Перебор аргументов сигнала. Пока только их вывод на экран.
        for(int j = 0; j < args.count(); j++)
        {
            int arg = args.at(j).toInt();
            qDebug() << i+1 << j+1  << arg << "expected " << value;
            //QVERIFY(value == arg);
        }
    }
    return true;
}

bool TestBase::checkOperationResult(const DevicesSpyDescriptor &spyDesc, const QList<int> &values)
{
    qDebug() << "Sent << " << values;
    qDebug() << "verification receiver:";
    // перебор всех зарегистрированных шпионов (по одному на каждый тип сигнала)
    for (int i = 0; i < spyDesc.RxSpyList.count(); i++)
    {
        checkSignal(*spyDesc.RxSpyList.at(i), values.at(i));
    }

    qDebug() << "verification transmitter:";
    for (int i = 0; i < spyDesc.TxSpyList.count(); i++)
    {
        checkSignal(*spyDesc.TxSpyList.at(i), values.at(i));
    }

    return false;
}

void TestBase::freeSpyes(DevicesSpyDescriptor &spyDesc)
{
    for (int i = 0; i < spyDesc.RxSpyList.count(); i++)
    {
        delete spyDesc.RxSpyList.at(i);
    }

    qDebug() << "verification transmitter:";
    for (int i = 0; i < spyDesc.TxSpyList.count(); i++)
    {
        delete spyDesc.TxSpyList.at(i);
    }

}
// Отправка устройству одной и той же команды с разными параметрами
DevicesSpyDescriptor TestBase::uploadSettings(const int command, const QList<int> &values, const int packetsDelay)
{
    DevicesSpyDescriptor retSpyDesc;

    TestHelper *testHelper = TestHelper::getInstance();
    QList<QSignalSpy*> spyList;
    for (int i = 0; i < values.count(); i++)
    {
        qDebug() << "ask" << command << values.at(i);

        testHelper->receiver()->setSendPeriod(packetsDelay);
        retSpyDesc.RxSpyList.append(testHelper->askTransceiver(testHelper->receiver(), static_cast<TCommand_RadioModem>(command), values.at(i)));
        testHelper->transmitter()->setSendPeriod(packetsDelay);
        retSpyDesc.TxSpyList.append(testHelper->askTransceiver(testHelper->transmitter(), static_cast<TCommand_RadioModem>(command), values.at(i)));
    }
    return retSpyDesc;
}

// отправка устройству набора команд с соответствующими им параметрами (по одной команде на 1 параметр)
DevicesSpyDescriptor TestBase::uploadSettings(const QList<int> &commands, const QList<int> &values, const int packetsDelay)
{
    DevicesSpyDescriptor retSpyDesc;
    TestHelper *testHelper = TestHelper::getInstance();

    qDebug() << "ask Rx";
    retSpyDesc.RxSpyList = testHelper->askTransceiver(testHelper->receiver(), commands, values, packetsDelay);
    qDebug() << "ask Tx";
    retSpyDesc.TxSpyList = testHelper->askTransceiver(testHelper->transmitter(),commands, values, packetsDelay);

    return retSpyDesc;
}

void TestBase::uploadAllSettingsCombinations(int packetsDelay, int waitDelay)
{
    TestHelper *testHelper = TestHelper::getInstance();
    foreach(int modulation, testHelper->modulationTypeList())
    {
        foreach(int speed, testHelper->connectionSpeedList())
        {
            foreach(int power, testHelper->txPowerList())
            {
                QList<int> commands;
                commands.append(eModulationTypeSet);
                commands.append(eModulationSpeedSet);
                commands.append(eTxPowerSet);

                QList<int> values;
                values.append(modulation);
                values.append(speed);
                values.append(power);

                // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
                DevicesSpyDescriptor spyDesc = uploadSettings(commands, values, packetsDelay);

                // ожидаем результата выполнения операции
                QTest::qWait(waitDelay);

                // анализ результатов
                checkOperationResult(spyDesc, values);

                freeSpyes(spyDesc);
            }
        }
    }
}
