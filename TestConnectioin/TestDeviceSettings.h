#ifndef TESTDEVICESTATUS_H
#define TESTDEVICESTATUS_H

#include "TestBase.h"

#include <QObject>

class TestDeviceSettings : public TestBase
{
    Q_OBJECT
public:
    explicit TestDeviceSettings(QObject *parent = 0);

private slots:
    void TestModulationTypeSet();
    void TestModulationSpeedSet();
    void TestTxPowerSet();
    void TestSingleSettings();
    void TestContinuouslySettings();
};

#endif // TESTDEVICESTATUS_H
