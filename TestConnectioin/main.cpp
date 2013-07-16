#include <QCoreApplication>
#include <QTest>
#include <QDebug>

#include "TestDeviceExistance.h"
#include "TestDeviceSettings.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(QTest::qExec(new TestDeviceExistance, argc, argv))
        qCritical() << "TestDeviceConnection FAILED! Cannot create CTransceiver objects";
    else
    if (QTest::qExec(new TestDeviceSettings, argc, argv))
        qCritical() << "TestDeviceSettings FAILED!";
    else

    return 0;
}
