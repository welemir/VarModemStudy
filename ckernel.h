#ifndef CKERNEL_H
#define CKERNEL_H

#include "BitBusPipes/cbitbuspipes.h"
//#include "BitBusPipes/CCommunicator.h"
#include "BitBusPipes/PipeMgr.h"
#include "ctransceiver.h"

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QList>

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

class CKernel : public QObject
{
  Q_OBJECT

public:
    static CKernel* GetInstance();

    int getBitErrorsDetected(){return m_iBitErrorsDetected;}
    int getBitErrorsMissed(){return m_iBitErrorsTotal - m_iBitErrorsDetected;}

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
    void slotParceRawDataStart();
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

private:
    CKernel();
    CKernel(const CKernel &);
    CKernel& operator=(const CKernel &);

    void configureDevices(); // �������� �������� ���� ������������ ����������� (����. � ������ ������������)
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

    // ��������� ��� ���������� ����� ������������
    stateOfProcess m_State;
    // ������� ��������� ����������� ��� ������������
    int m_iConnectionSpeed;
    int m_iOutputPower;   // �������� �������� ��� ����������� (0.5 dBm �� ���)
    CTransceiver::T_ModulationType m_ModulationType;
    int m_iPreambleLength;
    int m_iSyncPatternLength;
    int m_iPacketDataLength;  // ���������� ������ � ������
    CTransceiver::T_CrcType m_crcType;

    int m_iTotalDataLength;

    // ���������� ������
    int m_bytes_received;
    int m_packets_to_send;
    int m_iPacketsSentByTransmitter;
    int m_packets_received;
    int m_packets_received_ok;
    int m_iBitErrorsTotal;
    int m_iBitErrorsDetected;
};

#endif // CKERNEL_H
