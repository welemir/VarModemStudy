#include "TestBase.h"

#include <QSignalSpy>
#include <QDebug>
#include <QTest>

#include "TestHelper.h"

TestBase::TestBase(QObject *parent) :
    QObject(parent)
{
}

void TestBase::checkSignal(const QSignalSpy &spy,const int &value)
{
    qDebug() << spy.objectName();
    QVERIFY(spy.count() >= 1);
    // преребор всех однотипных сигналов, отловленных одним шпионом (например, несколько ответов на установку типа модуляции)
    for (int i = 0; i < spy.count(); i++)
    {
        QList<QVariant> args = spy.at(i);
        // Перебор аргументов сигнала. Пока только их вывод на экран.
        for(int i = 0; i < args.count(); i++)
        {
            int arg = args.at(i).toInt();
            qDebug() << i+1 <<" " << arg << "expected " << value;
            //QVERIFY(value == arg);
        }
    }
}

void TestBase::checkOperationResult(const DevicesSpyDescriptor &spyDesc, const QList<int> &values)
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
}

DevicesSpyDescriptor TestBase::uploadSettings(const QList<TCommand_RadioModem> &commands, const QList<int> &values, const int &packetsDelay)
{
    DevicesSpyDescriptor retSpyDesc;
    TestHelper *testHelper = TestHelper::getInstance();

    retSpyDesc.RxSpyList = testHelper->askTransceiver(testHelper->receiver(), commands, values, packetsDelay);
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
                QList<TCommand_RadioModem> commands;
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
            }
        }
    }
}
