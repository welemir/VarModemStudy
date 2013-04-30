#include <QMutex>
#include <QStringList>
#include <QTextCodec>

#include "ckernel.h"
#include "BitBusPipes/USB_Communicator.h"
#include "connectionControl.h"
#include "BitBusPipes/ctcp_client_communicator.h"
#include "programsettings.h"

QByteArray txPacket; // тк передаем всегда один и тот же пакет, сохраним его тут

CKernel* CKernel::m_pInstance = NULL;

CKernel* CKernel::GetInstance()
{
    static QMutex mutex;
    if (!m_pInstance)
    {
        mutex.lock();
        if (!m_pInstance)
            m_pInstance = new CKernel;
        mutex.unlock();
    }
    return m_pInstance;
}

CKernel::CKernel():
    m_usDestinationAddress(0),
    m_usGateway(0),
    m_pPipeCmd(0),
    m_ProgrammState(eDisconnected),
    m_DataToSendLength(1000),
    m_PacketLength(100)
{
    m_Transmitter = new CTransceiver(CTransceiver::eTransmitter, this);
    m_Receiver    = new CTransceiver(CTransceiver::eReceiver, this);

    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance(this);
    connect(pConnectionControl, SIGNAL(signalTransmitterConnected()), this, SLOT(slotTransmitterConnected()));
    connect(pConnectionControl, SIGNAL(signalTransmitterDisconnected()), this, SLOT(slotTransmitterDisconnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverrConnected()), this, SLOT(slotReceiverConnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverDisconnected()), this, SLOT(slotReceiverDisconnected()));

    connect(m_Transmitter, SIGNAL(signalNewModulationType( T_ModulationType )), this, SLOT(slotNewModulationType(CTransceiver::T_ModulationType)));
    connect(m_Transmitter, SIGNAL(signalNewConnectionSpeed( int )), this, SLOT(slotNewConnectionSpeed(int)));
    connect(m_Transmitter, SIGNAL(signalNewOutputPower( int )), this, SLOT(slotNewOutputPower(int)));
//    connect(m_Transmitter, SIGNAL(signalNewBitSynchLength( int )), this, SLOT();
//    connect(m_Transmitter, SIGNAL(signalNewSychnroSequence( QByteArray )), this, SLOT());
//    connect(m_Transmitter, SIGNAL(signalNewDataPacketLength( int )), this, SLOT());
//    connect(m_Transmitter, SIGNAL(signalNewCrcType( T_CrcType )), this, SLOT()));

    connect(m_Transmitter, SIGNAL(signalTxInProgress(bool)), this, SIGNAL(signalTxInProgress(bool)));
    connect(m_Transmitter, SIGNAL(signalTxProgress(int)), this, SIGNAL(signalTxProgress(int)));
    connect(m_Transmitter, SIGNAL(signalDiagMsg(QString)), this, SIGNAL(signalPrintDiagMeaasge(QString)) );
    connect(m_Receiver,    SIGNAL(signalDiagMsg(QString)), this, SIGNAL(signalPrintDiagMeaasge(QString)) );
    connect(pConnectionControl, SIGNAL(signalDiagMsg(QString)), this, SIGNAL(signalPrintDiagMeaasge(QString)) );
    connect(m_Receiver, SIGNAL(signalNewRawPacketReceived(QByteArray)), this, SLOT(slotNewPacketReceived(QByteArray)));

}

void CKernel::slotRunCommandFromUI(const CUICommand UIcommand)
{
    qDebug() << (int) UIcommand.teUIcommand ;
    qDebug() <<  QString(UIcommand.qsArguments) ;

    switch(UIcommand.teUIcommand)
    {
        case CUICommand::eCmdConnect:
        {
            qDebug() <<  "Selected device: " << UIcommand.qsArguments;
            setProgrammState(eDisconnected);
            m_usDestinationAddress = UIcommand.qsArguments.toUShort();

            if( (m_pPipeCmd) &&( m_usDestinationAddress ))
              {
                QByteArray data;

                data.clear();
                data.append(0x20);
                data.append(0x21);
                data.append(0x22);
                m_pPipeCmd->WriteData(data, m_usDestinationAddress);

                data.clear();
                data.append(0x23);
                data.append((char)0x00);
                m_pPipeCmd->WriteData(data, m_usDestinationAddress);
            }

        }break;


        default:
        {
        }break;

    }
}

void CKernel::slotProcessDataFromPipeOfCommand(QByteArray baData, unsigned short usSenderID)
{
    int iSeek = 0;

    while ( iSeek < baData.length() )
    {
        unsigned char ucCommand = baData[iSeek++];
        switch(ucCommand)
        {

        }// switch
    }
}

void CKernel::slotTransmitterConnected()
{
    QString msg = " CKernel::slotTransmitterConnected()";
    emit signalPrintDiagMeaasge(msg);
    emit signalTxStateUpdated(true);

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetTxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    CPipe    *pipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);

    connect(m_Transmitter, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            m_Transmitter, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(m_Transmitter, SIGNAL(signalNewRawPacket(QByteArray,unsigned short)),
            pipeRadioRaw, SLOT(WriteData(QByteArray,unsigned short)));

    m_Transmitter->slotSetDeviceMode(CTransceiver::eTransmitter);

}

void CKernel::slotTransmitterDisconnected()
{
    QString msg = " CKernel::slotTransmitterDisconnected()";
    emit signalPrintDiagMeaasge(msg);
    emit signalTxStateUpdated(false);
}

void CKernel::slotReceiverConnected()
{
    QString msg = " CKernel::slotReceiverConnected()";
    emit signalPrintDiagMeaasge(msg);
    emit signalRxStateUpdated(true);

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetRxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    CPipe    *pipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);
    connect(m_Receiver, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            m_Receiver, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(pipeRadioRaw, SIGNAL(ReadData(QByteArray,unsigned short)),
            m_Receiver, SLOT(slotParceRadioData(QByteArray,unsigned short)) );
}

void CKernel::slotReceiverDisconnected()
{
    QString msg = " CKernel::slotTransmitterDisconnected()";
    emit signalPrintDiagMeaasge(msg);
    emit signalRxStateUpdated(false);
}

void CKernel::slotSetConnectionSpeed(QString newSpeed)
{
    m_Transmitter->slotSetConnectionSpeed( newSpeed.toInt() );
    m_Receiver->slotSetConnectionSpeed( newSpeed.toInt() );
}

void CKernel::slotSetOutputPower(QString newPower)
{
    m_Transmitter->slotSetOutputPower( newPower.toInt() );
    m_Receiver->slotSetOutputPower( newPower.toInt() );
}

void CKernel::slotSetModulationType(int newModIndex)
{
    m_Transmitter->slotSetModulationType( (CTransceiver::T_ModulationType) newModIndex );
    m_Receiver->slotSetModulationType( (CTransceiver::T_ModulationType) newModIndex );}

void CKernel::slotSetBitSynchLength(QString newLength)
{
    m_Transmitter->slotSetBitSynchLength( newLength.toInt() );
    m_Receiver->slotSetBitSynchLength( newLength.toInt() );
}

void CKernel::slotSetSychnroSequenceLength(QString newLength)
{
    QByteArray baSequence;
    m_Transmitter->slotSetSychnroSequence( baSequence );
    m_Receiver->slotSetSychnroSequence( baSequence );
}

void CKernel::slotSetDataPacketLength(QString newLength)
{
    m_PacketLength = newLength.toInt();
    m_Transmitter->slotSetDataPacketLength( m_PacketLength );
    m_Receiver->slotSetDataPacketLength( m_PacketLength );
}

void CKernel::slotSetTotalDataLength(QString newLength)
{
    m_DataToSendLength = newLength.toInt();
}

void CKernel::slotSetCrcType(int newCrcIndex)
{
    m_Transmitter->slotSetCrcType( (CTransceiver::T_CrcType) newCrcIndex );
    m_Receiver->slotSetCrcType( (CTransceiver::T_CrcType) newCrcIndex );
}

void CKernel::slotNewModulationType(CTransceiver::T_ModulationType newModulaton)
{
    int newModulatonInd = newModulaton;
    emit signalNewModulationType(newModulatonInd);
}

void CKernel::slotNewConnectionSpeed(int newSpeed)
{
    QString qsNewMessaage = QString("%1").arg(newSpeed);
    emit signalNewConnectionSpeed(qsNewMessaage);
}

void CKernel::slotNewOutputPower(int newPower)
{
    QString qsNewMessaage = QString("%1").arg(newPower);
    emit signalNewOutputPower(qsNewMessaage);
}

void CKernel::slotStartOperation()
{
    QByteArray newPacket;
    for( int i = 0; i < m_PacketLength; i++)
    newPacket.append(0x33);
    //  newPacket.append(qrand());
    txPacket = newPacket;
    m_packets_to_send = 0;
    m_packets_received = 0;
    for (int i = 0; i<(m_DataToSendLength); i+=m_PacketLength)
    {
        m_packets_to_send++;
        m_Transmitter->slotAppendRawPacket(newPacket);
    }
    m_Receiver->slotStartOperation();
    m_Transmitter->slotStartOperation();

}

void CKernel::slotStopOperation()
{
    m_Transmitter->slotStopOperation();
    m_Receiver->slotStopOperation();
}

void CKernel::slotNewPacketReceived(QByteArray packet)
{
    int iErrorCounter = 0;
    int iPacketLength     = packet.length();
    for(int i = 0; i< iPacketLength; i++ )
    {
        if (packet[i] != txPacket[i])
            iErrorCounter++;
    }
    QString packetToDiag = QString("%1 из %2, %3 ошибок в пакете").arg(m_packets_received).arg(m_packets_to_send).arg(iErrorCounter);
    emit signalPrintDiagMeaasge(packetToDiag);
    packetToDiag = packet.toHex();
    emit signalPrintDiagMeaasge( packetToDiag);
}

void CKernel::setProgrammState(CKernel::T_ProgrammState newProgrammState)
{
    if (m_ProgrammState == newProgrammState)
        return;

    m_ProgrammState = newProgrammState;

    QString qsMessage;
    switch( m_ProgrammState )
    {
        case eConnected:
        {
            qsMessage = "ѕодключено";
        } break;

        default:
        {
        } break;
    }// switch

    CUICommand NewCommand(CUICommand::eCmdSetStatus);
    NewCommand.qsArguments = qsMessage;
    emit signalNewMessageToUI(NewCommand);

}

