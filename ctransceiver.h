#ifndef CTRANSCEIVER_H
#define CTRANSCEIVER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QBitArray>
#include <QQueue>

#define MODEM_DEVICE_ID 21
#define MODEM_RAWPIPE_TX_INTERVAL 10
#define MODEM_STATUS_INTERVAL 10

typedef struct{
    QByteArray baData;      // Данные пакета извлечённые приёмником
    int iStartBitNumber;    // Порядковый номер бита в принятой последовательности с момента начала эксперимента
    int iErrorsBit;         // Количество битовых ошибок в пакете
    int iErrorsByte;        // Количество байтовых ошибок в пакете
    bool bCrcOk;            // Флаг правильности контрольной суммы пакета
    int iTxCorrespondIndex; // Индекс пакета в переданной последовательности соответствующего данному
}TReceivedPacketDescription;

class CTransceiver : public QObject
{
    Q_OBJECT
public:
    typedef enum
    {
        eOOK   = 0,
        eFSK   = 1,
        eLast_Modulation
    }T_ModulationType;

    typedef enum
    {
        eCrcNone     = 0,
        eCrcXOR      = 1,
        eCrc8dallas  = 2,
        eCrc16_IBM   = 3,
        eCrc16_CCIT  = 4,
        eCrcLast
    }T_CrcType;

public:
    CTransceiver(QObject *parent = 0 );
    void getTranscieverStatistics( int &payloadDataSize, int &serviceDataSize, int &connectionSpeed);
    int packetsToSend();
    int getFieldSizeCrc();
    int getLenghtService();
    int calculateCrc(QByteArray baData);
    void appendCrc(QByteArray *pbaData);

signals:
    void signalNewCommand(QByteArray, unsigned short);
    void signalNewRawPacket(QByteArray, unsigned short);

    void signalNewRawPacketReceived(TReceivedPacketDescription);

    void signalTxInProgress(bool inProgress);
    void signalTxProgress(int percent);
    void signalTxQueueTransmitFinished();
    void signalDiagMsg(QString);

    void signalNewModulationType( CTransceiver::T_ModulationType );
    void signalNewConnectionSpeed( int );
    void signalNewOutputPower( int );
    void signalNewBitSynchLength( int );
    void signalNewSychnroSequence( QByteArray );
    void signalNewDataPacketLength( int );
    void signalNewCrcType( T_CrcType );
    void signalNewCarrierFrequency( int );
    // Сигналы начала и окончания разбора буфера приёма для правильной отработки последнего буфера экспериминта
    void signalParceRawDataStart();
    void signalParceRawDataEnd();

public slots:
    void slotParceCommand(QByteArray baData, unsigned short usSenderID);
    void slotParceRadioData(QByteArray baData, unsigned short usSenderID);

    void slotSetModulationType( T_ModulationType newModulaton );
    void slotSetConnectionSpeed( int newSpeed );
    void slotSetOutputPower( int newPower );
    void slotSetPreambleLength( int newLength );
    void slotSetSyncPattern( QByteArray pattern);
    void slotSetDataPacketLength( int newLength );
    void slotSetCrcType( T_CrcType newCrcType );
    void slotSetCarrierFrequency( int newFrequency);

    void slotTxStart();
    void slotTxStop();
    void slotRxStart();
    void slotRxStop();
    void slotAppendRawPacket(QByteArray newPacket);
    void slotUploadAllSettingsToModem();

private slots:
    void slotTxTimer();
    void slotStatusTimer();

protected:
    void processData(QByteArray baData);
    void TxSendPacket();

private:
    T_ModulationType m_modulation;
    T_CrcType m_crcType;
    int m_connectionSpeed;
    int m_TxPower;
    int m_iDataFieldSize;
    int m_iPreambleLength;

    QByteArray m_baStartPattern;
    QTimer m_SenderTimer;
    QTimer m_TransceiverStatusTimer;
    QQueue<QByteArray> m_TxQueue;
    int m_PermitedToTxPacketsCount;
    bool m_RxEnabled;
    QBitArray m_RxArray;

    int m_iPacketsToSend;

    bool m_bStopThread;

};

#endif // CTRANSCEIVER_H
