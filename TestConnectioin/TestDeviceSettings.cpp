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

void TestDeviceSettings::uploadAllSettingsCombinations(int packetsDelay, int waitDelay)
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

                // анализ результатов
                QVERIFY(checkOperationResult(commands, values));
            }
        }
    }
}

void TestDeviceSettings::TestModulationTypeSet()
{
    log() << "---- Trying to set new modulation Type -----";

    QList<int> values = TestHelper::getInstance()->modulationTypeList();
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    uploadSettings(eModulationTypeSet, values, PACKET_TIMEOUT);

    // анализ результатов
    QVERIFY(checkOperationResult(eModulationTypeSet, values));
}

void TestDeviceSettings::TestModulationSpeedSet()
{
    log() << "---- Trying to set new modulation Speed -----";

    QList<int> values = TestHelper::getInstance()->connectionSpeedList();
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    uploadSettings(eModulationSpeedSet, values, PACKET_TIMEOUT);

    // анализ результатов
    QVERIFY(checkOperationResult(eModulationSpeedSet, values));
}

void TestDeviceSettings::TestTxPowerSet()
{
    log() << "---- Trying to set new transmit Power -----";

    QList<int> values = TestHelper::getInstance()->txPowerList();
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    uploadSettings(eTxPowerSet, values, PACKET_TIMEOUT);

    // анализ результатов
    QVERIFY(checkOperationResult(eTxPowerSet, values));

}

void TestDeviceSettings::TestSingleSettings()
{
    log() << "---- Trying to upload all settings separately -----";
    uploadAllSettingsCombinations(PACKET_TIMEOUT, PACKET_TIMEOUT);
}

void TestDeviceSettings::TestContinuouslySettings()
{
     log() << "---- Trying to upload all settings continously -----";
     uploadAllSettingsCombinations(0, PACKET_TIMEOUT);
}
