#ifndef CKERNEL_H
#define CKERNEL_H

#include "BitBusPipes/cbitbuspipes.h"
//#include "BitBusPipes/CCommunicator.h"
#include "BitBusPipes/PipeMgr.h"

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
    void slotResetConnection();

signals:
    void signalNewMessageToUI(const CUICommand UIcommand);
    void signalWriteDataToPipe(const QByteArray buff, unsigned short usDestAddr,
                               CBitBusPipes::TPipeType PipeID);
private:
    CKernel();
    CKernel(const CKernel &);
    CKernel& operator=(const CKernel &);

private:
    static CKernel* m_pInstance;
    unsigned short m_usDestinationAddress;
    unsigned short m_usGateway;
    CPipe *m_pPipeCmd;

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
