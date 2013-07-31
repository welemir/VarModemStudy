#ifndef TESTDEVICESTATUS_H
#define TESTDEVICESTATUS_H

#include "TestBase.h"

#include <QObject>

class TestDeviceSettings : public TestBase
{
    Q_OBJECT
public:
    explicit TestDeviceSettings(QObject *parent = 0);
    void uploadAllSettingsCombinations(int packetsDelay, int waitDelay);

private slots:
    void TestModulationTypeSet();
    void TestModulationSpeedSet();
    void TestTxPowerSet();
public:
    void TestSingleSettings();
    void TestContinuouslySettings();
};

#endif // TESTDEVICESTATUS_H