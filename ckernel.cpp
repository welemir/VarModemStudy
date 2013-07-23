#include <QMutex>
#include <QStringList>
#include <QTextCodec>
#include <QThread>

#include "ckernel.h"
#include "BitBusPipes/USB_Communicator.h"
#include "BitBusPipes/CommunicationStructs.h"
#include "connectioncontrol.h"
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
  m_pPipeCmd(0),
  m_State(eIdle),
  m_iConnectionSpeed(9600),
  m_iOutputPower(20),
  m_ModulationType(CTransceiver::eFSK),
  m_iPreambleLength(2),
  m_iSyncPatternLength(2),
  m_crcType(CTransceiver::eCrcNone),
  m_iPacketDataLength(50),
  m_iTotalDataLength(1000)
{
    m_Transmitter = new CTransceiver(this);
    m_Receiver    = new CTransceiver(this);

    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance(this);
    connect(pConnectionControl, SIGNAL(signalTransmitterConnected()), this, SLOT(slotTransmitterConnected()));
    connect(pConnectionControl, SIGNAL(signalTransmitterDisconnected()), this, SLOT(slotTransmitterDisconnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverrConnected()), this, SLOT(slotReceiverConnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverDisconnected()), this, SLOT(slotReceiverDisconnected()));

    // Сигналы ответов трансивера на изменение настроек для которых могут быть ограничения
    connect(m_Transmitter, SIGNAL(signalNewModulationType( CTransceiver::T_ModulationType )), this, SLOT(slotNewModulationType(CTransceiver::T_ModulationType)));
    connect(m_Transmitter, SIGNAL(signalNewConnectionSpeed( int )), this, SLOT(slotNewConnectionSpeed(int)));
    connect(m_Transmitter, SIGNAL(signalNewOutputPower( int )), this, SLOT(slotNewOutputPower(int)));

    connect(m_Transmitter, SIGNAL(signalTxProgress(int)), this, SIGNAL(signalTxProgress(int)));
    connect(m_Receiver, SIGNAL(signalNewRawPacketReceived(TReceivedPacketDescription)), this, SLOT(slotNewPacketReceived(TReceivedPacketDescription)));

    connect(pConnectionControl, SIGNAL(signalDiagMsg(QString)), this, SIGNAL(signalPrintDiagMeaasge(QString)) );
    connect(m_Transmitter, SIGNAL(signalDiagMsg(QString)), this, SIGNAL(signalPrintDiagMeaasge(QString)) );
    connect(m_Receiver,    SIGNAL(signalDiagMsg(QString)), this, SIGNAL(signalPrintDiagMeaasge(QString)) );
}

void CKernel::slotRunCommandFromUI(const CUICommand UIcommand)
{
    qDebug() << (int) UIcommand.teUIcommand ;
    qDebug() <<  QString(UIcommand.qsArguments) ;

    switch(UIcommand.teUIcommand)
    {
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

    connect(m_Transmitter, SIGNAL(signalNewRawPacket(QByteArray,unsigned short)),
            this, SLOT(slotTransmitterPacketSent(QByteArray,unsigned short)));

    connect(m_Transmitter, SIGNAL(signalTxQueueTransmitFinished()),
            this, SLOT(slotTxFinished()));

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
  m_iConnectionSpeed = newSpeed;
  m_Transmitter->slotSetConnectionSpeed(newSpeed);
  m_Receiver->slotSetConnectionSpeed(newSpeed);
}

void CKernel::slotSetOutputPower(int newPower)
{
  m_iOutputPower = newPower;
    m_Transmitter->slotSetOutputPower(newPower);
    m_Receiver->slotSetOutputPower(newPower);
}

void CKernel::slotSetModulationType(int newModIndex)
{
  switch(newModIndex){
  case 0:
      m_ModulationType = CTransceiver::eOOK;
      break;
  case 1:
      m_ModulationType = CTransceiver::eFSK;
      break;
  }
    m_Transmitter->slotSetModulationType(m_ModulationType);
    m_Receiver->slotSetModulationType(m_ModulationType);
}

void CKernel::slotSetPreambleLength(int newLength)
{
  m_iPreambleLength = newLength;
  m_Transmitter->slotSetPreambleLength(newLength);
  m_Receiver->slotSetPreambleLength(newLength);
}

void CKernel::slotSetSyncPatternLength(int newLength)
{
  QByteArray baSequence;
  m_iSyncPatternLength = newLength;
  switch(m_iSyncPatternLength){
    case 1:
      baSequence.append(0x2d);  // Синхропосылка рекомендованная для MRF49XA
      break;
    case 2:
      baSequence.append(0xd3);  // Синхропосылка рекомендованная для CC1020
      baSequence.append(0x91);  //
      break;
  }
    m_Transmitter->slotSetSyncPattern(baSequence);
    m_Receiver->slotSetSyncPattern(baSequence);
}

void CKernel::slotSetDataPacketLength(int newLength)
{
    m_iPacketDataLength = newLength;
    m_Transmitter->slotSetDataPacketLength(m_iPacketDataLength);
    m_Receiver->slotSetDataPacketLength(m_iPacketDataLength);
}

void CKernel::slotSetTotalDataLength(int newLength)
{
    m_iTotalDataLength = newLength;
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
  emit signalTxInProgress(true);
    if (m_iPacketDataLength <= 50)
        signalNewDataPacketLength(50);

    // Синхронизация всех настроек используемых устройств и програмных модулей
    configureDevices();

    QByteArray newPacket;
    for( int i = 0; i < m_iPacketDataLength; i++)
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
    m_iPacketsSentByTransmitter = 0;

    for (int i = 0; i<(m_iTotalDataLength); i+=m_iPacketDataLength)
    {
        m_packets_to_send++;
        m_Transmitter->slotAppendRawPacket(newPacket);
    }
    // Переключение в режим ожидания включения приёмника и передача ему команды на включение "сырого" приёма
    m_State = ePrepare;
    m_Receiver->slotRxStart();

    emit signalShowBER(0);
    emit signalShowPER(0);
    emit signalTxProgress(0);
    emit signalRxProgress(0);
    emit signalUpdateStatistics();

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

    m_Transmitter->slotTxStop();
    m_Receiver->slotRxStop();
    emit signalTxInProgress(false);
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

    emit signalRxProgress((100 * (m_packets_received_ok)) / m_packets_to_send);
    emit signalUpdateStatistics();
}

void CKernel::slotSetDefaultValuesOnStart()
{
}

void CKernel::slotTxFinished()
{
    if (m_packets_to_send == m_iPacketsSentByTransmitter){
        m_State = eAwaitingEnd;
      }
}
void CKernel::slotTransmitterPacketSent(QByteArray,unsigned short)
{
    m_iPacketsSentByTransmitter++;
    emit signalTxProgress( (100 * (m_packets_to_send - m_Transmitter->packetsToSend() + 1)) / m_packets_to_send);

    m_Receiver->slotRxStart();
}

void CKernel::slotParceRawDataStart()
{
  switch(m_State){
    case ePrepare:
      m_Transmitter->slotTxStart();
      m_State = eWork;
      break;

    case eAwaitingEnd:
      m_State = eIdle;
    }
}

void CKernel::slotParceRawDataEnd()
{
  if(eIdle == m_State)
    slotStopOperation();
}

void CKernel::configureDevices()
{
  slotSetConnectionSpeed(m_iConnectionSpeed);
  slotSetOutputPower(m_iOutputPower);
  slotSetModulationType(m_ModulationType);
  slotSetPreambleLength(m_iPreambleLength);
  slotSetSyncPatternLength(m_iSyncPatternLength);
  slotSetDataPacketLength(m_iPacketDataLength);
  slotSetCrcType(m_crcType);
}

