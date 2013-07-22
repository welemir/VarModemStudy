
#ifndef TESTBASE_H
#define TESTBASE_H

#include <QObject>
#include <QSignalSpy>

#include "CommandCode_RadioModem.h"

struct DevicesSpyDescriptor
{
    QList<QSignalSpy *> RxSpyList;
    QList<QSignalSpy *> TxSpyList;
};


class TestBase : public QObject
{
    Q_OBJECT
public:
    explicit TestBase(QObject *parent = 0);
    bool checkSignal(const QSignalSpy &spy, const int &value);
    bool checkOperationResult(const DevicesSpyDescriptor &spyDesc, const QList<int> &values);
    void freeSpyes(DevicesSpyDescriptor &spyDesc);
    DevicesSpyDescriptor uploadSettings(const int command, const QList<int> &values, const int packetsDelay = 0);
    DevicesSpyDescriptor uploadSettings(const QList<int> &commands, const QList<int> &values, const int packetsDelay = 0);
    void uploadAllSettingsCombinations(int packetsDelay, int waitDelay);
};

#endif // TESTDEVICESTATUS_H
