#include <QCoreApplication>
#include <QTest>
#include <QDebug>

#include "TestDeviceExistance.h"
#include "TestDeviceSettings.h"
#include "TestCommunicationStability.h"
#include "TestPacketSend.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(QTest::qExec(new TestDeviceExistance, argc, argv))
        log() << "TestDeviceConnection FAILED! Cannot create CTransceiver objects";
    else
    {
        if (QTest::qExec(new TestPacketSend, argc, argv))
            log() << "TestPacketSend FAILED!";
        if (QTest::qExec(new TestDeviceSettings, argc, argv))
            log() << "TestDeviceSettings FAILED!";

        if (QTest::qExec(new TestCommunicationStability, argc, argv))
            log() << "TestCommunicationStability FAILED!";
    }

    delete TestHelper::getInstance();
    return 0;
}
