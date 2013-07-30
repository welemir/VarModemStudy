#include <QDataStream>
#include <QDebug>
#include <QCoreApplication>
#include"BitBusPipes/CommunicationStructs.h"
#include "CommandCode_RadioModem.h"
#include "ctransceiver.h"

CTransceiver::CTransceiver(QObject *parent)
  :QObject(parent)
  ,m_iPreambleLength(2)
  ,m_uiSendPeriod(MODEM_CMD_TX_INTERVAL)
  ,m_RxEnabled(false)
{
  m_timerSendTimeout.setSingleShot(true);

    connect(&m_SenderTimer, SIGNAL(timeout()), this, SLOT(slotTxTimer()));
    connect(&m_timerSendTimeout, SIGNAL(timeout()), this, SLOT(slotTimeoutSendToDevice()));
}

void CTransceiver::getTranscieverStatistics(int &payloadDataSize, int &serviceDataSize, int &connectionSpeed)
{
    payloadDataSize = m_iDataFieldSize;
    serviceDataSize = getLenghtService();
    connectionSpeed = m_connectionSpeed;
}

int CTransceiver::packetsToSend()
{
    return m_TxQueue.length();
}

void CTransceiver::queueSendCommand(QByteArray baPacket)
{
  m_QueueCommandToSend.enqueue(baPacket);
  if(0 == m_baCommandSendedLast.length())
    trySendCommand();
}

void CTransceiver::trySendCommand()
{
  m_baCommandSendedLast.clear();
  if(m_QueueCommandToSend.isEmpty()){
    m_timerSendTimeout.stop();
  }else{
    m_baCommandSendedLast = m_QueueCommandToSend.dequeue();
    emit signalNewCommand(m_baCommandSendedLast, MODEM_DEVICE_ID);
    if(eSubmitRawData == m_baCommandSendedLast[0])
      m_baCommandSendedLast.clear();

    m_timerSendTimeout.start(m_uiSendPeriod);
  }
}

int CTransceiver::getFieldSizeCrc()
{
  int iSize = 0;
  switch(m_crcType){
    case eCrcXOR:
    case eCrc8dallas:
      iSize = 1;
      break;
    case eCrc16_IBM:
    case eCrc16_CCIT:
      iSize = 2;
      break;
    case eCrcNone:
    default:
      iSize = 0;
      break;
  }
  return iSize;
}

int CTransceiver::getLenghtService()
{
  return m_iPreambleLength + m_baStartPattern.length() + getFieldSizeCrc();
}

void CTransceiver::slotParceCommand(QByteArray baData, unsigned short usSenderID)
{
    int iSeek = 0;

    while ( iSeek < baData.length() )
    {
        unsigned char ucCommand = baData[iSeek++];
        switch(ucCommand)
        {
        case eAnsWorkMode:
        {
        //    unsigned char ucNewMode =  baData[iSeek++];
       //     m_mode = (CTransceiver::T_DeviceModes) ucNewMode;
      //      emit signalNewDeviceMode(m_mode);
        }break;

        case eAnsModulationType:
        {
            unsigned char ucNewMode =  baData[iSeek++];
            emit signalNewModulationType(ucNewMode);
        }break;

        case eAnsModulationSpeed: // speed
        {
            unsigned short usCurrentSpeed;
            memcpy(&usCurrentSpeed, baData.data() + iSeek, sizeof(usCurrentSpeed) );
            iSeek += sizeof(usCurrentSpeed);
         //   m_connectionSpeed = usCurrentSpeed;

            emit signalNewConnectionSpeed(usCurrentSpeed);
        }break;

        case eAnsTxPower: // power
        {
            short sCurrentPower;
            memcpy(&sCurrentPower, baData.data() + iSeek, sizeof(sCurrentPower) );
            iSeek += sizeof(sCurrentPower);
           // m_TxPower = usCurrentPower;

            emit signalNewOutputPower(sCurrentPower/2);
        }break;

        case eAnsModemStatusGet:
        {
            // буфер трансивера способен вместить хотя бы 1 пакет
            unsigned char ucPacketQueueFree = baData[iSeek++];
            m_PermitedToTxPacketsCount = ucPacketQueueFree;//(0 < ucPacketQueueFree) ? 1 : 0;

            // передатчик трансивера отправил все переданные ему пакеты и выключился
            unsigned char TxOn = baData[iSeek++]&0x1;

            // Передачу считаем завершенной, если:
            // 1. Все пакеты отправлены трансиверу.
            // 2. Буфер трансивера освободился.
            // 3. Передача в радиоканал трансивером окончена.
            if ((0 == packetsToSend()) && (3 == ucPacketQueueFree) && (0 == TxOn))
                emit signalTxQueueTransmitFinished();


        }break;
        default:
         break;
        }// switch
    }
  trySendCommand();
}

void CTransceiver::slotParceRadioData(QByteArray baData, unsigned short usSenderID)
{
  processData(baData);
}

void CTransceiver::slotSetModulationType(T_ModulationType newModulaton)
{
    m_modulation = newModulaton;
    QByteArray baPacket;
    baPacket.append(eModulationTypeSet);
    // так как T_ModulationType совпадает с аргументами данной команды, просто передадим его
    baPacket.append((char) newModulaton);
    queueSendCommand(baPacket);
}

void CTransceiver::slotSetConnectionSpeed(int newSpeed)
{
    m_connectionSpeed = newSpeed;
    QByteArray baPacket;
    baPacket.append(eModulationSpeedSet);
    unsigned short modulationSpeed = newSpeed;
    baPacket.append((char*)&modulationSpeed, sizeof(modulationSpeed));
    queueSendCommand(baPacket);
}

void CTransceiver::slotSetOutputPower(int newPower)
{
    m_TxPower = newPower;
    QByteArray baPacket;
    baPacket.append(eTxPowerSet);
    signed short txPower = 2*newPower;
    baPacket.append((char*)&txPower, sizeof(txPower));
    queueSendCommand(baPacket);
}

void CTransceiver::slotSetPreambleLength(int newLength)
{
    m_iPreambleLength = newLength;
    QByteArray baPacket;
    baPacket.append(ePreambleLengthSet);
    baPacket.append((char) newLength);
    queueSendCommand(baPacket);
}

void CTransceiver::slotSetSyncPattern(QByteArray pattern)
{
  m_baStartPattern = pattern;
  QByteArray baPacket;
  baPacket.append(eSyncroStartParamSet);
  baPacket.append(pattern);
  queueSendCommand(baPacket);
}

void CTransceiver::slotSetDataPacketLength(int newLength)
{
    m_iDataFieldSize = newLength;
    QByteArray baPacket;
    baPacket.append(eDataFieldSizeSet);
    baPacket.append((char) newLength);
    queueSendCommand(baPacket);
}

void CTransceiver::slotSetCrcType(CTransceiver::T_CrcType newCrcType)
{
    m_crcType = newCrcType;
    QByteArray baPacket;
    baPacket.append(eCrcModeSet);
    // так как T_CrcType совпадает с аргументами данной команды, просто передадим его
    baPacket.append((char) m_crcType);
    queueSendCommand(baPacket);
}

void CTransceiver::slotSetCarrierFrequency(int newFrequency)
{
}

void CTransceiver::slotAppendRawPacket(QByteArray newPacket)
{
    m_TxQueue.enqueue(newPacket);
}

void CTransceiver::slotUploadAllSettingsToModem()
{
    slotSetModulationType(m_modulation);
    slotSetConnectionSpeed(m_connectionSpeed);
    slotSetOutputPower(m_TxPower);
    slotSetPreambleLength(m_iPreambleLength);
    slotSetDataPacketLength(m_iDataFieldSize);
    slotSetCrcType(m_crcType);
}

void CTransceiver::slotTxTimer()
{
    TxSendPacket();
    slotStatusTimer();
}

void CTransceiver::TxSendPacket()
{
    if((0 < m_PermitedToTxPacketsCount)
       &&(!m_TxQueue.isEmpty()))
    {
        QByteArray packetToSend = m_TxQueue.dequeue();

        m_PermitedToTxPacketsCount--;

        packetToSend.prepend(m_baStartPattern);
        emit signalNewRawPacket(packetToSend, MODEM_DEVICE_ID);
    }
}

int CTransceiver::calculateCrc(QByteArray baData)
{
  switch(m_crcType){
  case CTransceiver::eCrcXOR:{
      unsigned char crc = 0;
      for(int iInd = 0; iInd < baData.size(); iInd++){
          crc = crc ^ baData[iInd];
      }
      return crc;
      }break;
    case CTransceiver::eCrc8dallas:{
        CCRC_Checker crc(CCRC_Checker::eCRC8_iButton);
        for(int iInd = 0; iInd < baData.size(); iInd++){
            crc.Add(baData[iInd]);
        }
        return crc;
        }break;
    case CTransceiver::eCrc16_IBM:{
        CCRC_Checker crc(CCRC_Checker::eCrc16_IBM);
        for(int iInd = 0; iInd < baData.size(); iInd++){
            crc.Add(baData[iInd]);
        }
        return crc;
        }break;
    case CTransceiver::eCrc16_CCIT:{
        CCRC_Checker crc(CCRC_Checker::eCrc16_CCIT);
        for(int iInd = 0; iInd < baData.size(); iInd++){
            crc.Add(baData[iInd]);
        }
        return crc;
        }break;
    case CTransceiver::eCrcNone:
    default:
        return 0;
  }
  return 0;
}

void CTransceiver::appendCrc(QByteArray *pbaData)
{
  unsigned int uiCrc = calculateCrc(*pbaData);
  switch(m_crcType){
    case eCrcXOR:
    case eCrc8dallas:
      pbaData->append(static_cast<char>(uiCrc));
      break;
    case eCrc16_IBM:
    case eCrc16_CCIT:
      pbaData->append(static_cast<char>(uiCrc>>8));
      pbaData->append(static_cast<char>(uiCrc));
      break;
    case eCrcNone:
    default:
      break;
  }
}

void CTransceiver::slotStatusTimer()
{
    // в Poll режиме опрашивается трансивер. Запрашиваем статус модема
    // статус модема содаржит 1 char  количество свободных на отправку мест в очереди
    // и второй char со статусом модема
    if((0 == m_PermitedToTxPacketsCount)
       ||(0 == packetsToSend()))
    {
        QByteArray newPacket;
        newPacket.append(eModemStatusGet);
        emit signalNewCommand(newPacket, MODEM_DEVICE_ID);
    }
}

void CTransceiver::slotTimeoutSendToDevice()
{
  if(0 != m_baCommandSendedLast.length()){
    qDebug() << "+";
    emit signalCommandSendRetry();
    emit signalNewCommand(m_baCommandSendedLast, MODEM_DEVICE_ID);
    m_timerSendTimeout.start(m_uiSendPeriod);
  }
  else
  {
    trySendCommand();
  }
}

void CTransceiver::slotTxStart()
{
    m_PermitedToTxPacketsCount = 0;
    m_SenderTimer.start(m_uiSendPeriod); // таймер отправки сообщений

    m_iPacketsToSend = m_TxQueue.length();
    emit signalTxInProgress(true);

    m_bStopThread = false;
}

void CTransceiver::slotTxStop()
{
    // выключить трансивер
    m_SenderTimer.stop();
    m_bStopThread = true;
    emit signalTxInProgress(false);

    m_TxQueue.clear();

    m_bStopThread = true;
}

void CTransceiver::slotRxStart()
{
    m_TxQueue.clear();

    // Регистрация себя в качестве получателя "сырого" потока от приемника для разбора
    QByteArray baPacket;
    baPacket.append(eSubmitRawData);
    baPacket.append((char) 0xfff0/*CUSB_Communicator::eDevID_Usb*/);
    baPacket.append((char) 0xfff0/*CUSB_Communicator::eDevID_Usb*/>>8);
    queueSendCommand(baPacket);
}

void CTransceiver::slotRxStop()
{
    // Отключене трансляции "сырого" потока приёмма
    QByteArray baPacket;
    baPacket.append(eSubmitRawData);
    baPacket.append((char)0);
    baPacket.append((char)0);
    queueSendCommand(baPacket);
}

void CTransceiver::processData(QByteArray baData)
{
    emit signalParceRawDataStart();

    static QByteArray baDataObtained;
    static unsigned long st_ulByteNext = 0; // Очередная порция битов из входного буфера
    static int st_iByteCounter = -1;
    static unsigned int st_uiOffsetBit = 0;  // Сдвиг для старшей части байта

    unsigned long ulMarkerPattern = static_cast<unsigned char>(m_baStartPattern[0]);
    unsigned long ulMarkerMask = 0x00ff0000;
    if(2 == m_baStartPattern.length()){
      ulMarkerPattern <<= 8;
      ulMarkerPattern += static_cast<unsigned char>(m_baStartPattern[1]);
      ulMarkerMask = 0xffff0000;
    }
    ulMarkerPattern <<= 16;

    int iSourceInd = 0;
    while(baData.length() > iSourceInd){
        st_ulByteNext += (((unsigned long)(unsigned char)baData[iSourceInd++])<<8);

        if(0 > st_iByteCounter){
            st_uiOffsetBit = 8;
            while(0 < st_uiOffsetBit){
                st_ulByteNext <<= 1;
                st_uiOffsetBit--;
                if(ulMarkerPattern  == (st_ulByteNext & ulMarkerMask)){
                    st_ulByteNext <<= st_uiOffsetBit;
                    baDataObtained.clear();
                    st_iByteCounter = 0;
                    break;
                }
            }
        }else{
            unsigned char ucNext = st_ulByteNext>>(8 + st_uiOffsetBit);
            if(0 == ucNext)
                ucNext = 0;
            baDataObtained.append(ucNext);
            st_iByteCounter++;
            st_ulByteNext <<= 8/*uiOffsetBitRight*/;

            if(st_iByteCounter >= (m_iDataFieldSize + getFieldSizeCrc())){
              TReceivedPacketDescription packetNew;
              packetNew.baData = baDataObtained;
              // Проверка Crc
              packetNew.bCrcOk = ( 0 == calculateCrc(packetNew.baData));

              emit signalNewRawPacketReceived(packetNew);

                st_iByteCounter = -1;
                st_uiOffsetBit = 0;
            }
        }
    }

    emit signalParceRawDataEnd();
}
