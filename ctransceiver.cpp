#include "ctransceiver.h"
#include"BitBusPipes/CommunicationStructs.h"
#include <QDataStream>
#include <QDebug>

const char syncro_sequence_barker11[] = {0b00000111, 0b00010010};
const char syncro_sequence_barker13[] = {0b00011111, 0b00110101};

CTransceiver::CTransceiver( T_DeviceModes role, QObject *parent) :
    QObject(parent),
    m_role(role),
    m_RxEnabled(false)
{
    m_SenderTimer = new QTimer(this);
    m_TransceiverStatusTimer = new QTimer(this);

    connect( m_SenderTimer, SIGNAL(timeout()), this, SLOT(slotTxTimer()));
    connect( m_TransceiverStatusTimer, SIGNAL(timeout()), this, SLOT(slotStatusTimer()));

}

void CTransceiver::getTranscieverStatistics(int &payloadDataSize, int &serviceDataSize, int &connectionSpeed)
{
    payloadDataSize = m_Length;
    serviceDataSize = m_SynchroSequence.length();
    connectionSpeed = m_connectionSpeed;
}

void CTransceiver::slotParceCommand(QByteArray baData, unsigned short usSenderID)
{
    int iSeek = 0;

    while ( iSeek < baData.length() )
    {
        unsigned char ucCommand = baData[iSeek++];
        switch(ucCommand)
        {
        case 0xC1:
        {
        //    unsigned char ucNewMode =  baData[iSeek++];
       //     m_mode = (CTransceiver::T_DeviceModes) ucNewMode;
      //      emit signalNewDeviceMode(m_mode);
        }break;

        case 0xC3:
        {
            unsigned char ucNewMode =  baData[iSeek++];
            T_ModulationType modulation = (CTransceiver::T_ModulationType) ucNewMode;
            emit signalNewModulationType(modulation);
        }break;

        case 0xC5: // speed
        {
            unsigned short usCurrentSpeed;
            memcpy(&usCurrentSpeed, baData.data() + iSeek, sizeof(usCurrentSpeed) );
            iSeek += sizeof(usCurrentSpeed);
         //   m_connectionSpeed = usCurrentSpeed;

            emit signalNewConnectionSpeed(usCurrentSpeed);
        }break;

        case 0xC7: // power
        {
            unsigned short usCurrentPower;
            memcpy(&usCurrentPower, baData.data() + iSeek, sizeof(usCurrentPower) );
            iSeek += sizeof(usCurrentPower);
           // m_TxPower = usCurrentPower;

            emit signalNewOutputPower(usCurrentPower);
        }break;

        case 0xe0: // queue size
        {
            unsigned char ucPacketQueueFree = baData[iSeek++];
//            unsigned char ucPacketRawQueueFree = baData[iSeek++];
            m_PermitedToTxPacketsCount = ucPacketQueueFree;
        }break;
        default:
        {
            return;
        }break;
        }// switch
    }
}

void CTransceiver::slotParceRadioData(QByteArray baData, unsigned short usSenderID)
{
    if (m_role == eReceiver){
//        emit signalNewRawPacketReceived(baData);
//        return;

        processData(baData);
    }
//        if (m_RxEnabled)
//        {


//        }

}

void CTransceiver::slotSetDeviceMode(CTransceiver::T_DeviceModes newMode)
{
    QByteArray baPacket;
    baPacket.append(0x40);
    // ��� ��� T_DeviceModes ��������� � ����������� ������ �������, ������ ��������� ���
    baPacket.append((char) newMode);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetModulationType(CTransceiver::T_ModulationType newModulaton)
{
    m_modulation = newModulaton;
    QByteArray baPacket;
    baPacket.append(0x42);
    // ��� ��� T_ModulationType ��������� � ����������� ������ �������, ������ ��������� ���
    baPacket.append((char) newModulaton);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetConnectionSpeed(int newSpeed)
{
    m_connectionSpeed = newSpeed;
    QByteArray baPacket;
    baPacket.append(0x44);
    unsigned short modulationSpeed = newSpeed;
    baPacket.append((char*)&modulationSpeed, sizeof(modulationSpeed));
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetOutputPower(int newPower)
{
    m_TxPower = newPower;
    QByteArray baPacket;
    baPacket.append(0x46);
    signed short txPower = 2*newPower;
    baPacket.append((char*)&txPower, sizeof(txPower));
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetBitSynchLength(int newLength)
{
    m_SynchroLength = newLength;
    QByteArray baPacket;
    baPacket.append(0x48);
    baPacket.append((char) newLength);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetSychnroSequence(QByteArray sequence)
{
    QByteArray baPacket;
    baPacket.append(0x4a);
    baPacket.append(sequence);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetDataPacketLength(int newLength)
{
    m_Length = newLength;
    QByteArray baPacket;
    baPacket.append(0x4c);
    baPacket.append((char) newLength);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetCrcType(CTransceiver::T_CrcType newCrc)
{
    m_CrcType = newCrc;
    QByteArray baPacket;
    baPacket.append(0x4e);
    // ��� ��� T_CrcType ��������� � ����������� ������ �������, ������ ��������� ���
    baPacket.append((char) newCrc);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetCarrierFrequency(int newFrequency)
{
}

void CTransceiver::slotStartOperation()
{
    m_SynchroSequence = QByteArray(syncro_sequence_barker13, sizeof(syncro_sequence_barker13));
    if(eTransmitter == m_role)
    {
        slotTxStart();
    }
    if(eReceiver == m_role)
    {
        slotRxStart();
    }

}

void CTransceiver::slotStopOperation()
{
    if(eTransmitter == m_role)
    {
        slotTxStop();
    }
    if(eReceiver == m_role)
    {
        slotRxStop();
    }
}

void CTransceiver::slotAppendRawPacket(QByteArray newPacket)
{
    m_TxQueue.append(newPacket);
}

void CTransceiver::slotUploadAllSettingsToModem()
{
    slotSetModulationType(m_modulation);
    slotSetConnectionSpeed(m_connectionSpeed);
    slotSetOutputPower(m_TxPower);
    slotSetBitSynchLength(m_SynchroLength);
    slotSetDataPacketLength(m_Length);
    slotSetCrcType(m_CrcType);
}

int iTotalpackets;
void CTransceiver::slotTxTimer()
{
    if (0 < m_PermitedToTxPacketsCount)
    {
        emit signalTxProgress( (100 * (iTotalpackets - m_TxQueue.length() + 1)) / iTotalpackets);
        QByteArray packetToSend = m_TxQueue[0];
        m_TxQueue.removeFirst();

        m_PermitedToTxPacketsCount--;


        CCRC_Checker crc(CCRC_Checker::eCRC8_iButton);
        for(int iInd = 0; iInd < packetToSend.size(); iInd++)
        {
            crc.Add(packetToSend[iInd]);
        }
        char cCRC = crc;

        packetToSend.prepend(m_SynchroSequence);
        packetToSend.append(cCRC);
        emit signalNewRawPacket(packetToSend, MODEM_DEVICE_ID);

        int txInterval = m_tmeTxTmrDelta.elapsed();
        m_tmeTxTmrDelta.restart();
        if (m_MaxTxTmrInterval < txInterval)
            m_MaxTxTmrInterval = txInterval;
        qDebug() << "slotTxTimer, Interval = " << txInterval;
    }


    if (m_TxQueue.empty())
    {
        slotTxStop();
    }
}

void CTransceiver::slotStatusTimer()
{
    if(m_PermitedToTxPacketsCount == 0)
    {
        QByteArray newPacket;
        newPacket.append(0x60);
        emit signalNewCommand(newPacket, MODEM_DEVICE_ID);

        int txReqInterval = m_tmeTxReqDelta.elapsed();
        m_tmeTxReqDelta.restart();
        if (m_MaxTxReqInterval < txReqInterval)
            m_MaxTxReqInterval = txReqInterval;
        qDebug() << "slotStatusTimer, Interval = " << txReqInterval;
    }
    else
    {
      //  m_TransceiverStatusTimer.start(MODEM_STATUS_INTERVAL);
    }
}

void CTransceiver::slotTxStart()
{
    // ��������� ��������� "������ ��������" �� ���������
    slotSetDeviceMode(eTransmitter);
    m_PermitedToTxPacketsCount = 0;
    m_TransceiverStatusTimer->start(MODEM_STATUS_INTERVAL);
    m_SenderTimer->start(MODEM_RAWPIPE_TX_INTERVAL);
    iTotalpackets = m_TxQueue.length();
    emit signalTxInProgress(true);

    m_tmeTxReqDelta.restart();
    m_tmeTxTmrDelta.restart();
    m_MaxTxReqInterval = m_MaxTxTmrInterval = 0;
}

void CTransceiver::slotTxStop()
{
    // ��������� ���������
    slotSetDeviceMode(ePowerOff);
    m_TransceiverStatusTimer->stop();
    m_SenderTimer->stop();
    emit signalTxInProgress(false);

    qDebug() << "MAX Req ask interval = " << m_MaxTxReqInterval;
    qDebug() << "MAX Tx send interval = " << m_MaxTxTmrInterval;
    m_TxQueue.clear();
}

void CTransceiver::slotRxStart()
{
    // ��������� ��������� "������ �����" �� ���������
    slotSetDeviceMode(eReceiver);
    QDataStream synch_read(m_SynchroSequence);
    m_RxSynchro.clear();
    //m_RxSynchro << synch_read;

    // ����������� ���� � �������� ���������� "������" ������ �� ��������� ��� �������
    QByteArray baPacket;
    baPacket.append(0x61);
    baPacket.append((char) 0xfff0/*CUSB_Communicator::eDevID_Usb*/);
    baPacket.append((char) 0xfff0/*CUSB_Communicator::eDevID_Usb*/>>8);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotRxStop()
{
    // ��������� ���������
    slotSetDeviceMode(ePowerOff);

    // ��������� ���������� "������" ������ ������
    QByteArray baPacket;
    baPacket.append(0x61);
    baPacket.append((char)0);
    baPacket.append((char)0);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);

}

void CTransceiver::processData(QByteArray baData)
{
    qDebug() << "FindData02 <======";
    static QByteArray baDataObtained;
    static unsigned long st_ulByteNext = 0; // ��������� ������ ����� �� �������� ������
    static int st_iByteCounter = -1;
    static unsigned int st_uiOffsetBit = 0;  // ����� ��� ������� ����� �����
    unsigned long ulMarkerPattern = 0x1f350000;
    int iSourceInd = 0;
    while(baData.length() > iSourceInd){
        st_ulByteNext += (((unsigned long)(unsigned char)baData[iSourceInd++])<<8);

        if(0 > st_iByteCounter){
            st_uiOffsetBit = 8;
            while(0 < st_uiOffsetBit){
                st_ulByteNext <<= 1;
                st_uiOffsetBit--;
                if(ulMarkerPattern  == (st_ulByteNext & 0xffff0000)){
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

            if(st_iByteCounter >= /*m_iDataFieldSize*/m_Length){
              emit signalNewRawPacketReceived(baDataObtained);

              st_iByteCounter = -1;
              st_uiOffsetBit = 0;
            }
        }
    }
}
