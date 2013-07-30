
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

private slots:
    void initTestCase();

protected:
    bool checkSignal(CTransceiver *pDevice, const int command, const int &expectedValue, const int valueNo = -1);

    // �������� ����������� �������� ��� �������, ��� ������������ ������ ������� � ���������������� ����������
    bool checkOperationResult(const QList<int> &commands, const QList<int> &values);

    // �������� ����������� �������� ��� �������, ��� ������������ ���� ������� � ������� ����������
    bool checkOperationResult(const int command, const QList<int> &values);
    void uploadSettings(const int command, const QList<int> &values, const int packetsDelay = 0);
    void uploadSettings(const QList<int> &commands, const QList<int> &values, const int packetsDelay = 0);
};

#endif // TESTDEVICESTATUS_H
