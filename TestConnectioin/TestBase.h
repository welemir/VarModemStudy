
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
    void checkSignal(const QSignalSpy &spy, const int &value);
    void checkOperationResult(const DevicesSpyDescriptor &spyDesc, const QList<int> &values);
    DevicesSpyDescriptor uploadSettings(const QList<TCommand_RadioModem> &commands, const QList<int> &values, const int &packetsDelay);
    void uploadAllSettingsCombinations(int packetsDelay, int waitDelay);
};

#endif // TESTDEVICESTATUS_H
