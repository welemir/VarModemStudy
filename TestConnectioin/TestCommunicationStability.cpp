#include "TestCommunicationStability.h"

#include <QTest>

#include "TestHelper.h"

TestCommunicationStability::TestCommunicationStability(QObject *parent) :
    TestBase(parent)
{
}

void TestCommunicationStability::fillList(QList<int> &list, const int &value, const int &count)
{
    for (int i = 0; i < count; i++)
    {
        list.append(static_cast<TCommand_RadioModem>(value));
    }
}

void TestCommunicationStability::TestMaximumContinuouslySettingsCount()
{

    TestHelper *testHelper = TestHelper::getInstance();
    /*for (int commandsCounter = 0; commandsCounter < testHelper->modulationTypeList().count(); commandsCounter++)
    {*/
        for (int count = 3; count < 10; count ++)
        {
            QList<int> commands;
            QList<int> values;

            fillList(commands, eModulationTypeSet, count);
            fillList(values, testHelper->modulationTypeList().at(1), count);

            // отправка настроек в модем. Между пакетами нет задержки
            uploadSettings(commands, values);

            // анализ результатов
            checkOperationResult(eModulationTypeSet, values);
        }
    //}
}
