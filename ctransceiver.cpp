#include "ctransceiver.h"

CTransceiver::CTransceiver(QObject *parent) :
    QObject(parent)
{
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
            unsigned char ucNewMode =  baData[iSeek++];
            m_mode = (CTransceiver::T_DeviceModes) ucNewMode;
            emit signalNewDeviceMode(m_mode);
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

        default:
        {
            return;
        }break;
        }// switch
    }
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
}

void CTransceiver::slotStopOperation()
{
}
