#include "ctransceiver.h"
#include"BitBusPipes/CommunicationStructs.h"
#include <QDataStream>

const char syncro_sequence_barker11[] = {0b00000111, 0b00010010};
const char syncro_sequence_barker13[] = {0b00011111, 0b00110101};

CTransceiver::CTransceiver( T_DeviceModes role, QObject *parent) :
    QObject(parent),
    m_role(role),
    m_RxEnabled(false)
{
    connect( &m_SenderTimer, SIGNAL(timeout()), this, SLOT(slotTxTimer()));
    connect( &m_TransceiverStatusTimer, SIGNAL(timeout()), this, SLOT(slotStatusTimer()));

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
            m_modulation = (CTransceiver::T_ModulationType) ucNewMode;
            emit signalNewModulationType(m_modulation);
        }break;

        case 0xC5: // speed
        {
            unsigned short usCurrentSpeed;
            memcpy(&usCurrentSpeed, baData.data() + iSeek, sizeof(usCurrentSpeed) );
            iSeek += sizeof(usCurrentSpeed);
            m_connectionSpeed = usCurrentSpeed;

            emit signalNewConnectionSpeed(m_connectionSpeed);
        }break;

        case 0xC7: // power
        {
            unsigned short usCurrentPower;
            memcpy(&usCurrentPower, baData.data() + iSeek, sizeof(usCurrentPower) );
            iSeek += sizeof(usCurrentPower);
            m_TxPower = usCurrentPower;

            emit signalNewOutputPower(m_TxPower);
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
    if (m_role == eReceiver)
        emit signalNewRawPacketReceived(baData);
//        if (m_RxEnabled)
//        {
//            QString packetToDiag = baData.toHex();
//            emit signalDiagMsg(packetToDiag);

//        }

}

void CTransceiver::slotSetDeviceMode(CTransceiver::T_DeviceModes newMode)
{
    QByteArray baPacket;
    baPacket.append(0x40);
    // так как T_DeviceModes совпадает с аргументами данной команды, просто передадим его
    baPacket.append((char) newMode);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetModulationType(CTransceiver::T_ModulationType newModulaton)
{
    QByteArray baPacket;
    baPacket.append(0x42);
    // так как T_ModulationType совпадает с аргументами данной команды, просто передадим его
    baPacket.append((char) newModulaton);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetConnectionSpeed(int newSpeed)
{
    QByteArray baPacket;
    baPacket.append(0x44);
    unsigned short modulationSpeed = newSpeed;
    baPacket.append((char*)&modulationSpeed, sizeof(modulationSpeed));
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetOutputPower(int newPower)
{
    QByteArray baPacket;
    baPacket.append(0x46);
    signed short txPower = 2*newPower;
    baPacket.append((char*)&txPower, sizeof(txPower));
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetBitSynchLength(int newLength)
{
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
    QByteArray baPacket;
    baPacket.append(0x4c);
    baPacket.append((char) newLength);
    emit signalNewCommand(baPacket, MODEM_DEVICE_ID);
}

void CTransceiver::slotSetCrcType(CTransceiver::T_CrcType newCrc)
{
    QByteArray baPacket;
    baPacket.append(0x4e);
    // так как T_CrcType совпадает с аргументами данной команды, просто передадим его
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
}

void CTransceiver::slotAppendRawPacket(QByteArray newPacket)
{
    m_TxQueue.append(newPacket);
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
    }
    else
    {
      //  m_TransceiverStatusTimer.start(MODEM_STATUS_INTERVAL);
    }
}

void CTransceiver::slotTxStart()
{
    // отправить сообщение "начать передачу" на трансивер
    slotSetDeviceMode(eTransmitter);
    m_PermitedToTxPacketsCount = 0;
    m_TransceiverStatusTimer.start(MODEM_STATUS_INTERVAL);
    m_SenderTimer.start(MODEM_RAWPIPE_TX_INTERVAL);
    iTotalpackets = m_TxQueue.length();
    emit signalTxInProgress(true);
}

void CTransceiver::slotTxStop()
{
    // выключить трансивер
    slotSetDeviceMode(ePowerOff);
    m_TransceiverStatusTimer.stop();
    m_SenderTimer.stop();
    emit signalTxInProgress(false);
}

void CTransceiver::slotRxStart()
{
    // отправить сообщение "начать прием" на трансивер
    slotSetDeviceMode(eReceiver);
    QDataStream synch_read(m_SynchroSequence);
    m_RxSynchro.clear();
    //m_RxSynchro << synch_read;

}

void CTransceiver::slotRxStop()
{
    // выключить трансивер
    slotSetDeviceMode(ePowerOff);
}
