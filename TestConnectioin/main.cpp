#include <QCoreApplication>
#include <QTest>
#include <QDebug>

#include "TestDeviceExistance.h"
#include "TestDeviceSettings.h"
#include "TestCommunicationStability.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(QTest::qExec(new TestDeviceExistance, argc, argv))
        qCritical() << "TestDeviceConnection FAILED! Cannot create CTransceiver objects";
    //else

//    QTest::qExec(new TestCommunicationStability, argc, argv);
    if (QTest::qExec(new TestDeviceSettings, argc, argv))
        qCritical() << "TestDeviceSettings FAILED!";


    return 0;
}
