
#ifndef TESTBASE_H
#define TESTBASE_H

#include <QObject>
#include <QSignalSpy>

#include "TestHelper.h"
#include "CommandCode_RadioModem.h"


class TestBase : public QObject
{
    Q_OBJECT
public:
    explicit TestBase(QObject *parent = 0);
    void checkSignal(CTransceiver *pDevice, const int command, const int &value, const int valueNo = -1);

    // проверка результатов операции при условии, что отправлялись разные команды с соответствующими значениями
    void checkOperationResult(const QList<int> &commands, const QList<int> &values);

    // проверка результатов операции при условии, что отправлялась одна команда с разными значениями
    void checkOperationResult(const int command, const QList<int> &values);
    void uploadSettings(const int command, const QList<int> &values, const int packetsDelay = 0);
    void uploadSettings(const QList<int> &commands, const QList<int> &values, const int packetsDelay = 0);
    void uploadAllSettingsCombinations(int packetsDelay, int waitDelay);
};

#endif // TESTDEVICESTATUS_H
