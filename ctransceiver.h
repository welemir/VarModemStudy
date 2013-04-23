#ifndef CTRANSCEIVER_H
#define CTRANSCEIVER_H

#include <QObject>

#define MODEM_DEVICE_ID 21

class CTransceiver : public QObject
{
    Q_OBJECT
public:
    typedef enum
    {
        eReceiver   = 0,
        eTransmitter= 1,
        ePowerOff   = 3,
        eLast_Mode
    }T_DeviceModes;

    typedef enum
    {
        eOOK   = 0,
        eFSK   = 1,
        eLast_Modulation
    }T_ModulationType;

    typedef enum
    {
        eCrcNone     = 0,
        eCrcSimple   = 1,
        eCrc8dallas  = 2,
        eCrc16_IBM   = 3,
        eCrc16_CCIT  = 4,
        eCrcLast
    }T_CrcType;

public:
    explicit CTransceiver(QObject *parent = 0);
    
signals:
    void signalNewCommand(QByteArray, unsigned short);

    void signalNewDeviceMode( T_DeviceModes );
    void signalNewModulationType( T_ModulationType );
    void signalNewConnectionSpeed( int );
    void signalNewOutputPower( int );
    void signalNewBitSynchLength( int );
    void signalNewSychnroSequence( QByteArray );
    void signalNewDataPacketLength( int );
    void signalNewCrcType( T_CrcType );
    void signalNewCarrierFrequency( int );


public slots:
    void slotParceCommand(QByteArray baData, unsigned short usSenderID);

    void slotSetDeviceMode( T_DeviceModes newMode );
    void slotSetModulationType( T_ModulationType newModulaton );
    void slotSetConnectionSpeed( int newSpeed );
    void slotSetOutputPower( int newPower );
    void slotSetBitSynchLength( int newLength );
    void slotSetSychnroSequence( QByteArray sequence );
    void slotSetDataPacketLength( int newLength );
    void slotSetCrcType( T_CrcType newCrc );
    void slotSetCarrierFrequency( int newFrequency);

    void slotStartOperation();
    void slotStopOperation();
private:
   // void updatesettings();
private:
    T_DeviceModes m_mode;
    T_ModulationType m_modulation;
    int m_connectionSpeed;
    int m_TxPower;

};

#endif // CTRANSCEIVER_H
