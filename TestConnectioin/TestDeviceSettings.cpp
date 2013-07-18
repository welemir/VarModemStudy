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

void TestDeviceSettings::TestSingleSettings()
{
    qDebug() << "---- Trying to upload all settings separately -----";
    uploadAllSettingsCombinations(PACKET_TIMEOUT, PACKET_TIMEOUT);
}

void TestDeviceSettings::TestContinuouslySettings()
{
     qDebug() << "---- Trying to upload all settings continousely -----";
     uploadAllSettingsCombinations(0, PACKET_TIMEOUT);
}
