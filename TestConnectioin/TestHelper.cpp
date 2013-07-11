#include "TestHelper.h"

#include <QDebug>

#include <connectioncontrol.h>
#include "ctransceiver.h"

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
{
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
    connect(m_Receiver, SIGNAL(signalParceRawDataEnd()),
            this, SLOT(slotParceRawDataEnd()) );

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

    connect(m_Transmitter, SIGNAL(signalNewRawPacket(QByteArray,unsigned short)),
            this, SLOT(slotTransmitterPacketSent(QByteArray,unsigned short)));

    connect(m_Transmitter, SIGNAL(signalTxQueueTransmitFinished()),
            this, SLOT(slotTxFinished()));
}

void TestHelper::slotTransmitterConnected()
{  
    m_bTransmitterConnected = true;
}

void TestHelper::slotReceiverConnected()
{
    m_bReceiverConnected = true;
}
