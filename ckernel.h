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
public slots:
    void slotRunCommandFromUI(const CUICommand UIcommand);
    void slotProcessDataFromPipeOfCommand(QByteArray baData, unsigned short usSenderID);
    void slotTransmitterConnected();
    void slotTransmitterDisconnected();

    void slotSetConnectionSpeed(QString newSpeed);
    void slotSetOutputPower(QString newPower);
    void slotSetModulationType(int newModIndex);
    void slotSetBitSynchLength( QString newLength );
    void slotSetSychnroSequenceLength( QString newLength );
    void slotSetDataPacketLength( QString newLength );
    void slotSetTotalDataLength( QString newLength );
    void slotSetCrcType( int newCrcIndex );

    void slotNewModulationType(CTransceiver::T_ModulationType newModulaton );
    void slotNewConnectionSpeed( int newSpeed );
    void slotNewOutputPower( int newPower );
//    void slotNewBitSynchLength( int newLength );
//    void slotNewSychnroSequence( QByteArray sequence );
//    void slotNewDataPacketLength( int newLength );
//    void slotNewCrcType( T_CrcType newCrc );

    void slotStartOperation();
    void slotStopOperation();

signals:
    void signalNewMessageToUI(const CUICommand UIcommand);
    void signalWriteDataToPipe(const QByteArray buff, unsigned short usDestAddr,
                               CBitBusPipes::TPipeType PipeID);
    void signalPrintDiagMeaasge(QString);
    void signalTxStateUpdated(bool);
    void signalRxStateUpdated(bool);

    void signalNewConnectionSpeed( QString );
    void signalNewOutputPower( QString );
    void signalNewModulationType( int );
    void signalNewBitSynchLength( QString  );
    void signalNewSychnroSequenceLength( QString  );
    void signalNewDataPacketLength( QString  );
    void signalNewTotalDataLength( QString  );
    void signalNewCrcType( int  );

private:
    CKernel();
    CKernel(const CKernel &);
    CKernel& operator=(const CKernel &);

private:
    static CKernel* m_pInstance;
    unsigned short m_usDestinationAddress;
    unsigned short m_usGateway;
    CPipe *m_pPipeCmd;
    CTransceiver m_Transmitter;
    CTransceiver m_Receiver;

private:
    typedef enum
    {
        eDisconnected,
        eConnected
    }T_ProgrammState;
    T_ProgrammState m_ProgrammState;

    void setProgrammState(T_ProgrammState newProgrammState);
};

#endif // CKERNEL_H
