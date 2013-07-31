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
        log() << "TestDeviceConnection FAILED! Cannot create CTransceiver objects";
    else
    {
        if (QTest::qExec(new TestDeviceSettings, argc, argv))
            log() << "TestDeviceSettings FAILED!";

        if (QTest::qExec(new TestCommunicationStability, argc, argv))
            log() << "TestCommunicationStability FAILED!";
    }

    qDebug() << "Test finished";
    delete TestHelper::getInstance();
    return 0;
}
