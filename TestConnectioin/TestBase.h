
#ifndef TESTBASE_H
#define TESTBASE_H

#include <QObject>

class TestBase : public QObject
{
    Q_OBJECT
public:
    explicit TestBase(QObject *parent = 0);
    void uploadAllSettingsCombinations(int packetsDelay, int waitDelay);
};

#endif // TESTDEVICESTATUS_H
