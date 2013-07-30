#ifndef TESTCOMMUNICATIONSTABILITY_H
#define TESTCOMMUNICATIONSTABILITY_H

#include "TestBase.h"

class TestCommunicationStability : public TestBase
{
    Q_OBJECT
public:
    explicit TestCommunicationStability(QObject *parent = 0);

private:
    void fillList(QList<int> &list, const int &value, const int &count);

private slots:
    void TestMaximumContinuouslySettingsCount();
};

#endif // TESTCOMMUNICATIONSTABILITY_H
