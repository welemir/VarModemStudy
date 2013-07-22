#include "TestHelper.h"

#include <QDebug>
#include <QThread>

#include <connectioncontrol.h>

TestHelper *TestHelper::m_pThis = NULL;

TestHelper *TestHelper::getInstance()
{
    if (NULL == m_pThis)
        m_pThis = new TestHelper;

    return m_pThis;
}

TestHelper::TestHelper(QObject *parent)
    : QObject(parent)
    , m_bTransmitterConnected(false)
    , m_bReceiverConnected(false)
    , m_iReceiverAnswer(0)
    , m_iTransmitterAnswer(0)
{
    qRegisterMetaType<TCommand_RadioModem>("TCommand_RadioModem");
    qRegisterMetaType<CTransceiver::T_ModulationType>("CTransceiver::T_ModulationType");

    m_ModulationTypeList.append(CTransceiver::eOOK);
    m_ModulationTypeList.append(CTransceiver::eFSK);

    m_ConnectionSpeedList.append(1200);
    m_ConnectionSpeedList.append(2400);
    m_ConnectionSpeedList.append(4800);
    m_ConnectionSpeedList.append(9600);
    m_ConnectionSpeedList.append(19200);
    m_ConnectionSpeedList.append(32000);

    m_TxPowerList.append(-8);
    m_TxPowerList.append(-2);
    m_TxPowerList.append(4);
    m_TxPowerList.append(10);
}


void TestHelper::slotTransmitterConnected()
{
    m_Transmitter = new CTransceiver(this);
    attachTransmitter(m_Transmitter);
    m_bTransmitterConnected = true;
}

void TestHelper::slotReceiverConnected()
{
    m_Receiver = new CTransceiver(this);
    attachReceiver(m_Receiver);
    m_bReceiverConnected = true;
}

void TestHelper::initDevices()
{
    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance();
    connect(pConnectionControl, SIGNAL(signalTransmitterConnected()), this, SLOT(slotTransmitterConnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverrConnected()), this, SLOT(slotReceiverConnected()));
}

void TestHelper::attachReceiver(CTransceiver *pReceiver)
{
    Q_ASSERT(pReceiver);

    if (!pReceiver) return;

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetRxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    CPipe    *pipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);
    connect(pReceiver, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            pReceiver, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(pipeRadioRaw, SIGNAL(ReadData(QByteArray,unsigned short)),
            pReceiver, SLOT(slotParceRadioData(QByteArray,unsigned short)) );
}

void TestHelper::attachTransmitter(CTransceiver *pTransmitter)
{
    Q_ASSERT(pTransmitter);

    if (!pTransmitter) return;

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetTxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    CPipe    *pipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);

    connect(pTransmitter, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            pTransmitter, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(pTransmitter, SIGNAL(signalNewRawPacket(QByteArray,unsigned short)),
            pipeRadioRaw, SLOT(WriteData(QByteArray,unsigned short)));

}

QSignalSpy* TestHelper::askTransceiver(CTransceiver *pDevice, const TCommand_RadioModem cmd, int value)
{
    QSignalSpy *spy = NULL;
    switch (cmd)
    {
        case eModulationTypeSet:
        {
            spy = new QSignalSpy(pDevice, SIGNAL(signalNewModulationType(CTransceiver::T_ModulationType)));
            spy->setObjectName("ModulationTypeSet");
            pDevice->slotSetModulationType(static_cast<CTransceiver::T_ModulationType>(value));
        }break;

        case eModulationSpeedSet:
        {
            spy = new QSignalSpy(pDevice, SIGNAL(signalNewConnectionSpeed(int)));
            spy->setObjectName("ModulationSpeedSet");
            pDevice->slotSetConnectionSpeed(static_cast<int>(value));
        }break;

        case eTxPowerSet:
        {
            spy = new QSignalSpy(pDevice, SIGNAL(signalNewOutputPower(int)));
            spy->setObjectName("TxPowerSet");
            pDevice->slotSetOutputPower(static_cast<int>(value));
        } break;

    default:
        break;
    }

    return spy;
}

#include <QTest>
QList<QSignalSpy*> TestHelper::askTransceiver(CTransceiver *pDevice, const QList<int> &commands, const QList<int> &values, const int &sendDelay)
{
    Q_ASSERT(commands.count());
    Q_ASSERT(values.count());
    Q_ASSERT(commands.count() == values.count());

    pDevice->setSendPeriod(sendDelay);

    QList<QSignalSpy*> spyList;
    for (int i = 0; i < commands.count(); i++)
    {
        qDebug() << "askTransceiver" << commands.at(i);
        QSignalSpy *retSpy = askTransceiver(pDevice, static_cast<TCommand_RadioModem>(commands.at(i)), values.at(i));
        if (retSpy)
            spyList.append(retSpy);
//        if (0 < sendDelay)
            //QTest::qSleep(sendDelay);
    }
    return spyList;
}
