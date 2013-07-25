#include "TestBase.h"

#include <QSignalSpy>
#include <QDebug>
#include <QTest>

#include "TestHelper.h"

QFile *pOutFile = NULL;

void customMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type)
    {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s\n", msg);
            break;
        case QtWarningMsg:
            txt = QString("%1").arg(msg);
            fprintf(stderr, "Warning: %s\n", msg);
            break;
        case QtCriticalMsg:
            txt = QString("%1").arg(msg);
            //fprintf(stdout, "Critical: %s\n", msg);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s\n", msg);
            abort();
    }

    if (NULL == pOutFile)
    {
        pOutFile = new QFile("TestLog.txt");
        pOutFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }

    if (!txt.isEmpty())
    {
        QTextStream ts(pOutFile);
        ts << txt << endl;
        pOutFile->flush();
    }
}


TestBase::TestBase(QObject *parent) :
    QObject(parent)
{
}

void TestBase::initTestCase()
{
    qInstallMsgHandler(customMessageHandler);
}

void TestBase::checkSignal(CTransceiver *pDevice, const int command, const int &expectedValue, const int valueNo)
{
    // преребор всех однотипных сигналов, отловленных одним шпионом (например, несколько ответов на установку типа модуляции)

    QSignalSpy *spy = TestHelper::getInstance()->spyForCommand(pDevice, command);

    QVERIFY(valueNo <= spy->count());

    if (-1 != valueNo) // если команда одна, то и значение ее параметра должно быть в одном пришедшем сигнале
    {
//        if (1 < spy->count())
//            log() << "Warning:" << spy->count() << "signals came. Only one signal should came";


        if (valueNo <= spy->count())
        {
            int argValue = spy->at(valueNo).at(0).toInt();

            if (expectedValue != argValue)
                log() << "There is no expected value" << expectedValue << "but received" << argValue;

            QVERIFY(expectedValue == argValue);
        }
    }
    else
    {
        if (valueNo+1 > spy->count())
            log() << "Error: expected signal #" << valueNo << "with value" << expectedValue << "didn't arrived.";

        bool bMatch = false;
        QList<int> valuesCame;
        for (int i = 0; i < spy->count(); i++) // если команд несколько, то значения параметров должны приходить в соответствующих сигналах
        {
            QList<QVariant> args = spy->at(i);

            // Перебор аргументов сигнала.
            for(int argNo = 0; argNo < args.count(); argNo++)
            {
                int argValue = args.at(argNo).toInt();
                valuesCame.append(argValue);
                if (expectedValue == argValue)
                {
                    bMatch = true;
                    break;
                }
            }
        }
        if (false == bMatch)
            log() << "Test Fail:" << "There is no expected value" << expectedValue << "but received" << valuesCame;
        QVERIFY(bMatch);
    }
}

void TestBase::checkOperationResult(const QList<int> &commands, const QList<int> &values)
{
    log() << "Sent << " << values;

    log() << "verification receiver:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->receiver(), commands.at(i), values.at(i));

    log() << "verification transmitter:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->transmitter(),  commands.at(i), values.at(i));
}

void TestBase::checkOperationResult(const int command, const QList<int> &values)
{
    log() << "Sent << " << values;

    log() << "verification receiver:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->receiver(), command, values.at(i), i);

    log() << "verification transmitter:";
    for (int i = 0; i < values.count(); i++)
        checkSignal(TestHelper::getInstance()->transmitter(), command, values.at(i), i);

}

// Отправка устройству одной и той же команды с разными параметрами
void TestBase::uploadSettings(const int command, const QList<int> &values, const int packetsDelay)
{
    TestHelper::getInstance()->prepare();
    TestHelper *testHelper = TestHelper::getInstance();
    for (int i = 0; i < values.count(); i++)
    {
        testHelper->askTransceiver(testHelper->receiver(), static_cast<TCommand_RadioModem>(command), values.at(i));
        testHelper->askTransceiver(testHelper->transmitter(), static_cast<TCommand_RadioModem>(command), values.at(i));
    }
}

// отправка устройству набора команд с соответствующими им параметрами (по одной команде на 1 параметр)
void TestBase::uploadSettings(const QList<int> &commands, const QList<int> &values, const int packetsDelay)
{
    TestHelper::getInstance()->prepare();
    TestHelper *testHelper = TestHelper::getInstance();
    testHelper->askTransceiver(testHelper->receiver(), commands, values, packetsDelay);
    testHelper->askTransceiver(testHelper->transmitter(),commands, values, packetsDelay);
}
