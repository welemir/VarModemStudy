#ifndef CTRANSCEIVER_H
#define CTRANSCEIVER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QBitArray>

#define MODEM_DEVICE_ID 21
#define MODEM_RAWPIPE_TX_INTERVAL 10
#define MODEM_STATUS_INTERVAL 10

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
    CTransceiver(T_DeviceModes role, QObject *parent = 0 );
    void getTranscieverStatistics( int &payloadDataSize, int &serviceDataSize, int &connectionSpeed);

signals:
    void signalNewCommand(QByteArray, unsigned short);
    void signalNewRawPacket(QByteArray, unsigned short);

    void signalNewRawPacketReceived(QByteArray);

    void signalTxInProgress(bool inProgress);
    void signalTxProgress(int percent);
    void signalDiagMsg(QString);

    void signalNewDeviceMode( T_DeviceModes );
    void signalNewModulationType( CTransceiver::T_ModulationType );
    void signalNewConnectionSpeed( int );
    void signalNewOutputPower( int );
    void signalNewBitSynchLength( int );
    void signalNewSychnroSequence( QByteArray );
    void signalNewDataPacketLength( int );
    void signalNewCrcType( T_CrcType );
    void signalNewCarrierFrequency( int );

public slots:
    void slotParceCommand(QByteArray baData, unsigned short usSenderID);
    void slotParceRadioData(QByteArray baData, unsigned short usSenderID);

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
    void slotAppendRawPacket(QByteArray newPacket);
    void slotUploadAllSettingsToModem();

private slots:
    void slotTxTimer();
    void slotStatusTimer();
    void slotTxStart();
    void slotTxStop();
    void slotRxStart();
    void slotRxStop();
protected:
    void processData(QByteArray baData);

private:
    const T_DeviceModes m_role;
    T_ModulationType m_modulation;
    T_CrcType m_CrcType;
    int m_connectionSpeed;
    int m_TxPower;
    int m_Length;
    int m_SynchroLength;

    QByteArray m_SynchroSequence;
    QTimer *m_SenderTimer;
    QTimer *m_TransceiverStatusTimer;
    QList<QByteArray> m_TxQueue;
    int m_PermitedToTxPacketsCount;
    bool m_RxEnabled;
    QBitArray m_RxArray;
    QBitArray m_RxSynchro;
    QTime m_tmeTxReqDelta;
    QTime m_tmeTxTmrDelta;
    int m_MaxTxReqInterval;
    int m_MaxTxTmrInterval;

    int m_iPacketsToSend;
};

#endif // CTRANSCEIVER_H
