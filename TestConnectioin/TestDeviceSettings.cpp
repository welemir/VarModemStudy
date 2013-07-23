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

    TestHelper::getInstance()->prepare();
    QList<int> values = TestHelper::getInstance()->modulationTypeList();
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    uploadSettings(eModulationTypeSet, values, PACKET_TIMEOUT);

    // ожидаем результата выполнения операции
    QTest::qWait(PACKET_TIMEOUT);

    // анализ результатов
    checkOperationResult(eModulationTypeSet, values);
}

void TestDeviceSettings::TestModulationSpeedSet()
{
    qDebug() << "---- Trying to set new modulation Speed -----";

//
    //TestHelper::getInstance()->prepare();
    QList<int> values = TestHelper::getInstance()->connectionSpeedList();
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    uploadSettings(eModulationSpeedSet, values, PACKET_TIMEOUT);

    // ожидаем результата выполнения операции
    QTest::qWait(PACKET_TIMEOUT*values.count());

    // анализ результатов
    checkOperationResult(eModulationSpeedSet, values);
}

void TestDeviceSettings::TestTxPowerSet()
{
    qDebug() << "---- Trying to set new transmit Power -----";

    TestHelper::getInstance()->prepare();
    QList<int> values = TestHelper::getInstance()->txPowerList();
    // отправка настроек в модем. Между пакетами задержка в packetsDelay милисекунд
    uploadSettings(eTxPowerSet, values, PACKET_TIMEOUT);

    // ожидаем результата выполнения операции
    QTest::qWait(PACKET_TIMEOUT);

    // анализ результатов
    checkOperationResult(eTxPowerSet, values);

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
