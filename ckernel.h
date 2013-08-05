#ifndef CKERNEL_H
#define CKERNEL_H

#include "BitBusPipes/cbitbuspipes.h"
//#include "BitBusPipes/CCommunicator.h"
#include "BitBusPipes/PipeMgr.h"
#include "ctransceiver.h"
#include "modelanalizedata.h"

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QList>
#include <QDataStream>

class CUICommand
{
public:
    typedef enum
    {
        eCmdConnect,
        eCmdSetStatus,
        eCmdSetGateway,
    }T_UICommands;

    CUICommand(T_UICommands teNewCommand):teUIcommand(teNewCommand){};

    T_UICommands teUIcommand;
    QString   qsArguments;
};

class TestPacketsAnalyseTest;
class CKernel : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString fileNameToPlayback READ fileNameToPlayback WRITE setFileNameToPlayback)

  friend class TestPacketsAnalyseTest;
public:
    static CKernel* GetInstance();

    QAbstractItemModel *getDataModel(){return &m_analizeData;}
    QAbstractItemModel *getCompareModel(){return &m_packetCompareData;}
    int getBitErrorsDetected(){return m_iBitErrorsDetected;}
    int getBitErrorsMissed(){return m_iBitErrorsTotal - m_iBitErrorsDetected;}
    QString fileNameToPlayback(){return m_sFileNameToPlayback;}
    void setFileNameToPlayback(QString fileNameNew){m_sFileNameToPlayback = fileNameNew;}

public slots:
    void slotRunCommandFromUI(const CUICommand UIcommand);
    void slotProcessDataFromPipeOfCommand(QByteArray baData, unsigned short usSenderID);
    void slotTransmitterConnected();
    void slotTransmitterDisconnected();
    void slotReceiverConnected();
    void slotReceiverDisconnected();

    void slotSetConnectionSpeed(int newSpeed);
    void slotSetOutputPower(int newPower);
    void slotSetModulationType(int newModIndex);
    void slotSetPreambleLength(int newLength);
    void slotSetSyncPatternLength(int newLength);
    void slotSetDataPacketLength(int newLength);
    void slotSetTotalDataLength(int newLength);
    void slotSetCrcType(int newCrcIndex);

    void slotNewModulationType(CTransceiver::T_ModulationType newModulaton );
    void slotNewConnectionSpeed(int newSpeed);
    void slotNewOutputPower(int newPower);
    void slotNewCrcType(int iCRCTypeIndexNew);

    void slotStartOperation();
    void slotStopOperation();
    void slotNewPacketReceived(TReceivedPacketDescription packetNew);
    void slotSetDefaultValuesOnStart();
    void slotParceRawDataStart(QByteArray *baRawDataNew);
    void slotParceRawDataEnd();

signals:
    void signalWriteDataToPipe(const QByteArray buff, unsigned short usDestAddr,
                               CBitBusPipes::TPipeType PipeID);
    void signalPrintDiagMeaasge(QString);
    void signalTxStateUpdated(bool);
    void signalRxStateUpdated(bool);
    void signalTxInProgress(bool inProgress);
    void signalTxProgress(int percent);
    void signalRxProgress(int percent);
    void signalShowBER(qreal);
    void signalShowPER(qreal);
    void signalShowChannelUtilizationPayload(qreal);
    void signalShowChannelUtilizationSerivce(qreal);
    void signalShowRxSpeed(qreal);
    void signalUpdateStatistics();

    void signalNewConnectionSpeed(int);
    void signalNewOutputPower(int);
    void signalNewModulationType(int);
    void signalNewBitSynchLength(int);
    void signalNewSychnroSequenceLength(int);
    void signalNewDataPacketLength(int);
    void signalNewTotalDataLength(int);
    void signalNewCrcType(int);

private slots:
    void slotTxFinished();
    void slotTransmitterPacketSent(QByteArray,unsigned short);
    void slotPlaybackTimerEvent();

private:
    CKernel();
    CKernel(const CKernel &);
    CKernel& operator=(const CKernel &);

    void configureDevices(); // Передача настроек всем подключённым устройствам (напр. в начале эксперимента)
    void comparePackets(const QByteArray &PacketOne, const QByteArray &pPacketTwo, int *piErrorCounterBytes, int *piErrorCounterBits, bool bUpdateStat=false);

private:
    static CKernel* m_pInstance;

    enum stateOfProcess{
      eIdle,
      ePrepare,
      eWork,
      eAwaitingEnd,
      eStop,
    };

    CPipe *m_pPipeCmd;
    CTransceiver *m_Transmitter;
    CTransceiver *m_Receiver;

    // Имя файла лога для чтения в режиме воспроизведения
    QString m_sFileNameToPlayback;
    // Состояние для управления ходом эксперимента
    stateOfProcess m_State;
    // Текущие настройки радиоканала для эксперимента
    int m_iConnectionSpeed;
    int m_iOutputPower;   // Выходная мощность для передатчика (0.5 dBm на бит)
    CTransceiver::T_ModulationType m_ModulationType;
    int m_iPreambleLength;
    int m_iSyncPatternLength;
    int m_iPacketDataLength;  // Количество данных в пакете
    CTransceiver::T_CrcType m_crcType;

    int m_iTotalDataLength;
    QList<QByteArray> m_baPacketsTx;    // Пакеты подготовленные для отправки в ходе эксперимента
    QList<QByteArray> m_baReceivedRaw;  // Пакеты битового потока от приёмника принятые в ходе эксперимента
    QList<QByteArray> m_baReceivedRawToPlay;  // Пакеты битового потока в режиме воспроизведения
    QList<QByteArray>::ConstIterator m_rawIterator;
    int m_iPlaybackPacketCounter;       // Счётчик пакетов обраотанных в режиме воспроизведения
    QList<TReceivedPacketDescription> m_baPacketsRx;    // Пакеты принятые в ходе эксперимента
    int m_iLastPacketRx;

    // Статистика обмена
    int m_bytes_received;
    int m_packets_to_send;
    int m_iPacketsSentByTransmitter;
    int m_packets_received;
    int m_packets_received_ok;
    int m_iBitErrorsTotal;
    int m_iBitErrorsDetected;

    bool m_bLogSave;
    QDataStream m_streamRawLogger;
    QTimer m_PlaybackTimer;

    CModelAnalizeData m_analizeData;
    CModelAnalizeData m_packetCompareData;
};

#endif // CKERNEL_H
