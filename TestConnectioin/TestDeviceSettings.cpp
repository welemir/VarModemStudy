#include "TestDeviceSettings.h"

#include <QDebug>
#include <QTest>

#include "TestHelper.h"
#include "CommandCode_RadioModem.h"
#include "ctransceiver.h"

TestDeviceSettings::TestDeviceSettings(QObject *parent) :
    TestBase(parent)
{
}

void TestDeviceSettings::TestModulationTypeSet()
{
    qDebug() << "---- Trying to set new modulation Type -----";

    QList<int> values;
    values.append(TestHelper::getInstance()->modulationTypeList().at(0));
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    DevicesSpyDescriptor spyDesc = uploadSettings(eModulationTypeSet, TestHelper::getInstance()->modulationTypeList(), PACKET_TIMEOUT);

    // ожидаем результата выполнения операции
    QTest::qWait(PACKET_TIMEOUT);

    // анализ результатов
    checkOperationResult(spyDesc, values);

    freeSpyes(spyDesc);
}

void TestDeviceSettings::TestModulationSpeedSet()
{
    qDebug() << "---- Trying to set new modulation Speed -----";

    QList<int> values;
    values.append(TestHelper::getInstance()->connectionSpeedList().at(0));
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    DevicesSpyDescriptor spyDesc = uploadSettings(eModulationSpeedSet, values, PACKET_TIMEOUT);

    // ожидаем результата выполнения операции
    QTest::qWait(PACKET_TIMEOUT);

    // анализ результатов
    checkOperationResult(spyDesc, values);

    freeSpyes(spyDesc);
}

void TestDeviceSettings::TestTxPowerSet()
{
    qDebug() << "---- Trying to set new transmit Power -----";

    QList<int> values;
    values.append(TestHelper::getInstance()->txPowerList().at(0));
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    DevicesSpyDescriptor spyDesc = uploadSettings(eTxPowerSet, values, PACKET_TIMEOUT);

    // ожидаем результата выполнения операции
    QTest::qWait(PACKET_TIMEOUT);

    // анализ результатов
    checkOperationResult(spyDesc, values);

    freeSpyes(spyDesc);
}

void TestDeviceSettings::TestSingleSettings()
{
    qDebug() << "---- Trying to upload all settings separately -----";
    uploadAllSettingsCombinations(PACKET_TIMEOUT, PACKET_TIMEOUT);
}

void TestDeviceSettings::TestContinuouslySettings()
{
     qDebug() << "---- Trying to upload all settings continously -----";
     uploadAllSettingsCombinations(0, PACKET_TIMEOUT);
}
