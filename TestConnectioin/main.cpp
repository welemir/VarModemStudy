#include <QCoreApplication>
#include <QTest>

#include "TestDeviceExistance.h"

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    QTest::qExec(new TestDeviceExistance, argc, argv);
    return 0;
}
