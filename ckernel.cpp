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
    slotSetDefaultValuesOnStart();

    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance(this);
    connect(pConnectionControl, SIGNAL(signalTransmitterConnected()), this, SLOT(slotTransmitterConnected()));
    connect(pConnectionControl, SIGNAL(signalTransmitterDisconnected()), this, SLOT(slotTransmitterDisconnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverrConnected()), this, SLOT(slotReceiverConnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverDisconnected()), this, SLOT(slotReceiverDisconnected()));

    connect(m_Transmitter, SIGNAL(signalNewModulationType( CTransceiver::T_ModulationType )), this, SLOT(slotNewModulationType(CTransceiver::T_ModulationType)));
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
    m_Transmitter->slotUploadAllSettingsToModem();

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

    m_Receiver->slotUploadAllSettingsToModem();
}

void CKernel::slotReceiverDisconnected()
{
    QString msg = " CKernel::slotTransmitterDisconnected()";
    emit signalPrintDiagMeaasge(msg);
    emit signalRxStateUpdated(false);
}

void CKernel::slotSetConnectionSpeed(int newSpeed)
{
    m_Transmitter->slotSetConnectionSpeed(newSpeed);
    m_Receiver->slotSetConnectionSpeed(newSpeed);
}

void CKernel::slotSetOutputPower(int newPower)
{
    m_Transmitter->slotSetOutputPower(newPower);
    m_Receiver->slotSetOutputPower(newPower);
}

void CKernel::slotSetModulationType(int newModIndex)
{
    m_Transmitter->slotSetModulationType( (CTransceiver::T_ModulationType) newModIndex );
    m_Receiver->slotSetModulationType( (CTransceiver::T_ModulationType) newModIndex );}

void CKernel::slotSetBitSynchLength(int newLength)
{
    m_Transmitter->slotSetBitSynchLength(newLength);
    m_Receiver->slotSetBitSynchLength(newLength);
}

void CKernel::slotSetSychnroSequenceLength(int newLength)
{
    QByteArray baSequence;
    m_Transmitter->slotSetSychnroSequence(baSequence);
    m_Receiver->slotSetSychnroSequence(baSequence);
}

void CKernel::slotSetDataPacketLength(int newLength)
{
    m_PacketLength = newLength;
    m_Transmitter->slotSetDataPacketLength(m_PacketLength);
    m_Receiver->slotSetDataPacketLength(m_PacketLength);
}

void CKernel::slotSetTotalDataLength(int newLength)
{
    m_DataToSendLength = newLength;
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
    emit signalNewConnectionSpeed(newSpeed);
}

void CKernel::slotNewOutputPower(int newPower)
{
    emit signalNewOutputPower(newPower);
}

void CKernel::slotStartOperation()
{
    QByteArray newPacket;
    for( int i = 0; i < m_PacketLength; i++)
        newPacket.append(qrand());

    txPacket = newPacket;
    m_packets_to_send = 0;
    m_packets_received = 0;
    m_packets_received_ok = 0;
    m_bytes_received = 0;
    m_errors_total = 0;

    for (int i = 0; i<(m_DataToSendLength); i+=m_PacketLength)
    {
        m_packets_to_send++;
        m_Transmitter->slotAppendRawPacket(newPacket);
    }
    m_Receiver->slotStartOperation();
    m_Transmitter->slotStartOperation();

    QString zero = "0";
    emit signalShowBER(zero);
    emit signalShowPER(zero);

    int payloadDataSize, serviceDataSize, connectionSpeed;
    m_Transmitter->getTranscieverStatistics( payloadDataSize, serviceDataSize, connectionSpeed );

    int payloadPercent = (100 * payloadDataSize )/(payloadDataSize + serviceDataSize);
    int serviceDataPercent = ( 100 * serviceDataSize )/(payloadDataSize + serviceDataSize);
    QString qsNewMessaage = QString("%1%").arg( payloadPercent );
    emit signalShowChannelUtilizationPayload(qsNewMessaage);
    qsNewMessaage = QString("%1%").arg(serviceDataPercent);
    emit  signalShowChannelUtilizationSerivce(qsNewMessaage);
    qsNewMessaage = QString("%1").arg( payloadPercent*connectionSpeed/100 );
    emit  signalShowRxSpeed(qsNewMessaage);
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
        unsigned int uiErrors = 0xff &(packet[i] ^ txPacket[i]);
        while(0 != uiErrors){
            if(1 == (1 & uiErrors))
                iErrorCounter++;
            uiErrors >>= 1;
        }
    }
    if (0==iErrorCounter)
        m_packets_received_ok++;
    m_bytes_received += iPacketLength;
    m_errors_total += iErrorCounter;

    QString packetToDiag = QString("%1 из %2, %3 ошибок в пакете").arg(++m_packets_received).arg(m_packets_to_send).arg(iErrorCounter);
    emit signalPrintDiagMeaasge(packetToDiag);
    packetToDiag = packet.toHex();
    emit signalPrintDiagMeaasge( packetToDiag);

    QString  errorRate;
    int per = (100 * (m_packets_to_send - m_packets_received_ok)) / m_packets_to_send;
    errorRate = QString("%1").arg(per);
    emit signalShowPER(errorRate);

    int ber = (100 *  m_errors_total) / (m_bytes_received*8);
    errorRate = QString("%1").arg(ber);
    emit signalShowBER(errorRate);


}

void CKernel::slotSetDefaultValuesOnStart()
{
    slotSetConnectionSpeed(9600);
    slotSetOutputPower(0);
    slotSetModulationType(1);
    slotSetBitSynchLength(2);
    slotSetSychnroSequenceLength(2);
    slotSetDataPacketLength(10);
    slotSetTotalDataLength(1000);
    slotSetCrcType(0);
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
}

