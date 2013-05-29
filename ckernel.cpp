#include <QMutex>
#include <QStringList>
#include <QTextCodec>

#include "ckernel.h"
#include "BitBusPipes/USB_Communicator.h"
#include "BitBusPipes/CommunicationStructs.h"
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
    m_PacketLength(100),
    m_isLastPacketSent(false),
    m_isLastRawBufferProcessed(false),
    m_crcType(CTransceiver::eCrcNone)
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
    connect(m_Receiver, SIGNAL(signalNewRawPacketReceived(TReceivedPacketDescription)), this, SLOT(slotNewPacketReceived(TReceivedPacketDescription)));

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

    connect(m_Transmitter, SIGNAL(signalTxFinished()),
            this, SLOT(slotTxFinished()));

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
    connect(m_Receiver, SIGNAL(signalParceRawDataStart()),
            this, SLOT(slotParceRawDataStart()) );
    connect(m_Receiver, SIGNAL(signalParceRawDataEnd()),
            this, SLOT(slotParceRawDataEnd()) );

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
    m_Receiver->slotSetModulationType( (CTransceiver::T_ModulationType) newModIndex );
}

void CKernel::slotSetPatternLength(int newLength)
{
    m_Transmitter->slotSetPatternLength(newLength);
    m_Receiver->slotSetPatternLength(newLength);
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
  switch(newCrcIndex){
  case 0:
      m_crcType = CTransceiver::eCrcNone;
      break;
  case 1:
      m_crcType = CTransceiver::eCrcXOR;
      break;
  }

  m_Transmitter->slotSetCrcType( (CTransceiver::T_CrcType) newCrcIndex );
  m_Receiver->slotSetCrcType( (CTransceiver::T_CrcType) newCrcIndex );
}

void CKernel::slotNewModulationType(CTransceiver::T_ModulationType newModulaton)
{
    emit signalNewModulationType(static_cast<int>(newModulaton));
}

void CKernel::slotNewConnectionSpeed(int newSpeed)
{
    emit signalNewConnectionSpeed(newSpeed);
}

void CKernel::slotNewOutputPower(int newPower)
{
    emit signalNewOutputPower(newPower);
}

void CKernel::slotNewCrcType(int iCRCTypeIndexNew)
{
}

void CKernel::slotStartOperation()
{
    QByteArray newPacket;
    for( int i = 0; i < m_PacketLength; i++)
        newPacket.append(qrand());

    m_Transmitter->appendCrc(&newPacket);

    txPacket = newPacket;

    // Сброс счётчиков статистики омена
    m_packets_to_send = 0;
    m_packets_received = 0;
    m_packets_received_ok = 0;
    m_bytes_received = 0;
    m_iBitErrorsTotal = 0;
    m_iBitErrorsDetected = 0;
    m_isLastPacketSent = false;
    m_isLastRawBufferProcessed = false;

    for (int i = 0; i<(m_DataToSendLength); i+=m_PacketLength)
    {
        m_packets_to_send++;
        m_Transmitter->slotAppendRawPacket(newPacket);
    }
    m_Receiver->slotStartOperation();
    m_Transmitter->slotStartOperation();

    emit signalShowBER(0);
    emit signalShowPER(0);

    int payloadDataSize, serviceDataSize, connectionSpeed;
    m_Transmitter->getTranscieverStatistics( payloadDataSize, serviceDataSize, connectionSpeed );

    float payloadPercent = (100. * payloadDataSize )/(payloadDataSize + serviceDataSize);
    float serviceDataPercent = ( 100. * serviceDataSize )/(payloadDataSize + serviceDataSize);

    emit signalShowChannelUtilizationPayload(payloadPercent);
    emit signalShowChannelUtilizationSerivce(serviceDataPercent);
    emit signalShowRxSpeed(payloadPercent*connectionSpeed/100);
}

void CKernel::slotStopOperation()
{
    float fPer = (100. *(m_packets_to_send - m_packets_received_ok)/ m_packets_to_send);
    emit signalShowPER(fPer);

    float fBer = (100. *  m_iBitErrorsTotal) / (m_bytes_received*8);
    emit signalShowBER(fBer);

    m_Transmitter->slotStopOperation();
    m_Receiver->slotStopOperation();
}

void CKernel::slotNewPacketReceived(TReceivedPacketDescription packetNew)
{
  // Определение коилчества ошибок в пакете
  int iErrorCounterBits = 0;
  int iErrorCounterBytes = 0;
    int iPacketLength = packetNew.baData.length();
    for(int i = 0; i< iPacketLength; i++ )
    {
        unsigned int uiErrors = 0xff &(packetNew.baData[i] ^ txPacket[i]);
        if(0 != uiErrors)
          iErrorCounterBytes++;
        while(0 != uiErrors){
            if(1 == (1 & uiErrors))
                iErrorCounterBits++;
            uiErrors >>= 1;
        }
    }

    // Заполнение структуры-описателя пакета данными об ошибках
    packetNew.iErrorsBit = iErrorCounterBits;
    packetNew.iErrorsByte = iErrorCounterBytes;

    m_packets_received++;
    if (0==iErrorCounterBits)
        m_packets_received_ok++;
    m_bytes_received += iPacketLength;
    m_iBitErrorsTotal += iErrorCounterBits;
    // Если контроль целостности выявил ошибочность пакета - учитываем все его битовые ошибки как обнаруженные
    if(!packetNew.bCrcOk)
      m_iBitErrorsDetected += iErrorCounterBits;

    QString packetToDiag = QString("%1 из %2, ошибок в пакете %3 (%4/%5)")
                           .arg(m_packets_received)
                           .arg(m_packets_to_send)
                           .arg(iErrorCounterBits)
                           .arg(iErrorCounterBytes)
                           .arg(packetNew.baData.length());
    if(!packetNew.bCrcOk)
      packetToDiag.append(" Crc Err");
    emit signalPrintDiagMeaasge(packetToDiag);
    packetToDiag = packetNew.baData.toHex();
    emit signalPrintDiagMeaasge( packetToDiag);
    if(0 != iErrorCounterBits){
        packetToDiag = txPacket.toHex();
        emit signalPrintDiagMeaasge( packetToDiag);
    }

    if (m_isLastPacketSent)
    {
        if(m_packets_to_send == m_packets_received_ok)
        {
            slotStopOperation();
        }
    }
    emit signalUpdateStatistics();
}

void CKernel::slotSetDefaultValuesOnStart()
{
    slotSetConnectionSpeed(9600);
    slotSetOutputPower(0);
    slotSetModulationType(1);
    slotSetPatternLength(2);
    slotSetSychnroSequenceLength(2);
    slotSetDataPacketLength(10);
    slotSetTotalDataLength(1000);
    slotSetCrcType(0);
}

void CKernel::slotTxFinished()
{
    m_isLastPacketSent = true;
}

void CKernel::slotParceRawDataStart()
{
  if(m_isLastPacketSent)
    m_isLastRawBufferProcessed = true;
}

void CKernel::slotParceRawDataEnd()
{
  if(m_isLastRawBufferProcessed)
    slotStopOperation();
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
            qsMessage = "Подключено";
        } break;

        default:
        {
        } break;
    }// switch
}

