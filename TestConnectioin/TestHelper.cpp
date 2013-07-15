#include "TestHelper.h"

#include <QDebug>

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
}


void TestHelper::slotTransmitterConnected()
{
    m_bTransmitterConnected = true;
}

void TestHelper::slotReceiverConnected()
{
    m_bReceiverConnected = true;
}

void TestHelper::initDevices()
{
    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance();
    connect(pConnectionControl, SIGNAL(signalTransmitterConnected()), this, SLOT(slotTransmitterConnected()));
 //   connect(pConnectionControl, SIGNAL(signalTransmitterDisconnected()), m_pConControlTest, SLOT(slotTransmitterDisconnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverrConnected()), this, SLOT(slotReceiverConnected()));
 //   connect(pConnectionControl, SIGNAL(signalReceiverDisconnected()), m_pConControlTest, SLOT(slotReceiverDisconnected()));

}

void TestHelper::attachReceiver()
{
    m_Receiver    = new CTransceiver(this);

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetRxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    CPipe    *pipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);
    connect(m_Receiver, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            m_Receiver, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(pipeRadioRaw, SIGNAL(ReadData(QByteArray,unsigned short)),
            m_Receiver, SLOT(slotParceRadioData(QByteArray,unsigned short)) );
    connect(m_Receiver, SIGNAL(signalParceRawDataStart()),
            this, SLOT(slotParceRawDataStart()) );
}

void TestHelper::attachTransmitter()
{
    m_Transmitter = new CTransceiver(this);

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetTxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    CPipe    *pipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);

    connect(m_Transmitter, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            m_Transmitter, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(m_Transmitter, SIGNAL(signalNewRawPacket(QByteArray,unsigned short)),
            pipeRadioRaw, SLOT(WriteData(QByteArray,unsigned short)));



}

void TestHelper::askTransceiver(CTransceiver *device, const TCommand_RadioModem cmd, int value, TInfoType_RadioModem &waitFor)
{
    waitFor = static_cast<TInfoType_RadioModem>(eAnswerMark + cmd);
    switch (cmd)
    {
        case eModulationTypeSet:
        {
            device->slotSetModulationType(static_cast<CTransceiver::T_ModulationType>(value));

        }break;

        case eModulationSpeedSet:
        {
            device->slotSetConnectionSpeed(static_cast<int>(value));
        }break;

    default:
        waitFor = static_cast<TInfoType_RadioModem>(0);
        break;
    }
}

void TestHelper::askTransmitter(const TCommand_RadioModem cmd, int value)
{
    switch (cmd)
    {
        case eModulationTypeSet:
        {
            connect(m_Transmitter, SIGNAL(signalNewModulationType(CTransceiver::T_ModulationType)), this, SLOT(transmitterModulationTypeChanged(CTransceiver::T_ModulationType)));
            m_Transmitter->slotSetModulationType(static_cast<CTransceiver::T_ModulationType>(value));
            m_awaitingTransmitterAnswer = eAnsModulationType;
        }break;

        case eModulationSpeedSet:
        {
            connect(m_Transmitter, SIGNAL(signalNewConnectionSpeed(int)), this, SLOT(transmitterModulationSpeedChanged(int)));
            m_Transmitter->slotSetConnectionSpeed(static_cast<int>(value));
            m_awaitingTransmitterAnswer = eAnsModulationSpeed;
        }break;

    default:
        m_awaitingTransmitterAnswer = static_cast<TInfoType_RadioModem>(0);
        break;
    }
}

void TestHelper::askReceiver(const TCommand_RadioModem cmd, int value)
{
    switch (cmd)
    {
        case eModulationTypeSet:
        {
            connect(m_Receiver, SIGNAL(signalNewModulationType(CTransceiver::T_ModulationType)), this, SLOT(receiverModulationTypeChanged(CTransceiver::T_ModulationType)));
            m_Receiver->slotSetModulationType(static_cast<CTransceiver::T_ModulationType>(value));
            m_awaitingReceiverAnswer = eAnsModulationType;
        }break;

        case eModulationSpeedSet:
        {
            connect(m_Receiver, SIGNAL(signalNewConnectionSpeed(int)), this, SLOT(receiverModulationSpeedChanged(int)));
            m_Receiver->slotSetConnectionSpeed(static_cast<int>(value));
            m_awaitingReceiverAnswer = eAnsModulationSpeed;
        }break;

    default:
        m_awaitingReceiverAnswer = static_cast<TInfoType_RadioModem>(0);
        break;
    }
}

void TestHelper::transmitterModulationTypeChanged(CTransceiver::T_ModulationType value)
{
    if (eAnsModulationType == m_awaitingTransmitterAnswer)
    {
        m_iTransmitterAnswer = static_cast<int>(value);
        m_awaitingTransmitterAnswer = static_cast<TInfoType_RadioModem>(0);
    }
}
void TestHelper::receiverModulationTypeChanged(CTransceiver::T_ModulationType value)
{
    if (eAnsModulationType == m_awaitingReceiverAnswer)
    {
        m_iReceiverAnswer = static_cast<int>(value);
        m_awaitingReceiverAnswer = static_cast<TInfoType_RadioModem>(0);
    }
}

void TestHelper::transmitterModulationSpeedChanged(int value)
{
    if (eAnsModulationSpeed == m_awaitingTransmitterAnswer)
    {
        m_iTransmitterAnswer = static_cast<int>(value);
        m_awaitingTransmitterAnswer = static_cast<TInfoType_RadioModem>(0);
    }
}
void TestHelper::receiverModulationSpeedChanged(int value)
{
    if (eAnsModulationSpeed == m_awaitingReceiverAnswer)
    {
        m_iReceiverAnswer = static_cast<int>(value);
        m_awaitingReceiverAnswer = static_cast<TInfoType_RadioModem>(0);
    }
}
