#include "TestBase.h"

#include <QSignalSpy>
#include <QDebug>
#include <QTest>

#include "TestHelper.h"

TestBase::TestBase(QObject *parent) :
    QObject(parent)
{
}

void TestBase::checkSignal(CTransceiver *pDevice, const int command, const int &value, const int valueNo)
{
    // преребор всех однотипных сигналов, отловленных одним шпионом (например, несколько ответов на установку типа модуляции)

    QSignalSpy *spy = TestHelper::getInstance()->spyForCommand(pDevice, command);
    qDebug() << "Spy" << spy << "expected value" << value;

    //QVERIFY(valueNo <= spy->count());

    //QCOMPARE(spy->at(valueNo).at(0).toInt(), value);
    if (-1 != valueNo)
        qDebug() << spy->at(valueNo).at(0).toInt();
    else
    for (int i = 0; i < spy->count(); i++) // один шпион способен отловить сигнал несколько раз.
    {
        QList<QVariant> args = spy->at(i);
        // Перебор аргументов сигнала. Пока только их вывод на экран.
        for(int argNo = 0; argNo < args.count(); argNo++)
        {
            int argValue = static_cast<int>(args.at(argNo).toInt());
            qDebug() << "arg" << argNo+1  << "=" << argValue;
            //QVERIFY(value == argValue);
        }
    }

}

void TestBase::checkOperationResult(const QList<int> &commands, const QList<int> &values)
{
    qDebug() << "Sent << " << values;

    qDebug() << "verification receiver:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->receiver(), commands.at(i), values.at(i));

    qDebug() << "verification transmitter:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->transmitter(),  commands.at(i), values.at(i));
}

void TestBase::checkOperationResult(const int command, const QList<int> &values)
{
    qDebug() << "Sent << " << values;

    qDebug() << "verification receiver:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->receiver(), command, values.at(i), i);

    qDebug() << "verification transmitter:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->transmitter(), command, values.at(i), i);

}

// Отправка устройству одной и той же команды с разными параметрами
void TestBase::uploadSettings(const int command, const QList<int> &values, const int packetsDelay)
{
    TestHelper::getInstance()->prepare();
    TestHelper *testHelper = TestHelper::getInstance();
    for (int i = 0; i < values.count(); i++)
    {
        testHelper->askTransceiver(testHelper->receiver(), static_cast<TCommand_RadioModem>(command), values.at(i));
        testHelper->askTransceiver(testHelper->transmitter(), static_cast<TCommand_RadioModem>(command), values.at(i));
    }
}

// отправка устройству набора команд с соответствующими им параметрами (по одной команде на 1 параметр)
void TestBase::uploadSettings(const QList<int> &commands, const QList<int> &values, const int packetsDelay)
{
    TestHelper::getInstance()->prepare();
    TestHelper *testHelper = TestHelper::getInstance();
    testHelper->askTransceiver(testHelper->receiver(), commands, values, packetsDelay);
    testHelper->askTransceiver(testHelper->transmitter(),commands, values, packetsDelay);
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
                uploadSettings(commands, values, packetsDelay);

                // ожидаем результата выполнения операции
                QTest::qWait(waitDelay);

                // анализ результатов
                checkOperationResult(commands, values);
            }
        }
    }
}
