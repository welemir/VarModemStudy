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
{ Q_OBJECT
private:

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
    void slotSetPatternLength(int newLength);
    void slotSetSychnroSequenceLength(int newLength);
    void slotSetDataPacketLength(int newLength);
    void slotSetTotalDataLength(int newLength);
    void slotSetCrcType(int newCrcIndex);

    void slotNewModulationType(CTransceiver::T_ModulationType newModulaton );
    void slotNewConnectionSpeed(int newSpeed);
    void slotNewOutputPower(int newPower);
//    void slotNewBitSynchLength( int newLength );
//    void slotNewSychnroSequence( QByteArray sequence );
//    void slotNewDataPacketLength( int newLength );
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

private:
    CKernel();
    CKernel(const CKernel &);
    CKernel& operator=(const CKernel &);

private:
    static CKernel* m_pInstance;
    unsigned short m_usDestinationAddress;
    unsigned short m_usGateway;
    CPipe *m_pPipeCmd;
    CTransceiver *m_Transmitter;
    CTransceiver *m_Receiver;

private:
    typedef enum
    {
        eDisconnected,
        eConnected
    }T_ProgrammState;
    T_ProgrammState m_ProgrammState;

    void setProgrammState(T_ProgrammState newProgrammState);
    int m_DataToSendLength;
    CTransceiver::T_CrcType m_crcType;
    int m_PacketLength;
    int m_packets_to_send;
    int m_packets_received;
    int m_packets_received_ok;
    int m_bytes_received;
    bool m_isLastPacketSent;
    bool m_isLastRawBufferProcessed;
    int m_iBitErrorsTotal;
    int m_iBitErrorsDetected;
};

#endif // CKERNEL_H
