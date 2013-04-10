#include <QMutex>
#include <QStringList>
#include <QTextCodec>

#include "ckernel.h"
#include "BitBusPipes/USB_Communicator.h"
#include "connectionControl.h"
#include "BitBusPipes/ctcp_client_communicator.h"
#include "programsettings.h"

CKernel* CKernel::m_pInstance = NULL;

CKernel* CKernel::GetInstance()
{
    static QMutex mutex;
    if (!m_pInstance)
    {
        mutex.lock();
        if (!m_pInstance)
            m_pInstance = new CKernel;
        mutex.unlock();
    }
    return m_pInstance;
}

CKernel::CKernel():
    m_usDestinationAddress(0),
    m_usGateway(0),
    m_pPipeCmd(0),
    m_ProgrammState(eDisconnected)
{
    CPipeMgr *pPipeMgr = CPipeMgr::GetInstance(CUSB_Communicator::eDevID_Usb);
    m_pPipeCmd = pPipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    QObject::connect(m_pPipeCmd, SIGNAL(ReadData(QByteArray, unsigned short)), this, SLOT(slotProcessDataFromPipeOfCommand(QByteArray, unsigned short)));

    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance(this);
    connect(pConnectionControl, SIGNAL(resetConnection()), this, SLOT(slotResetConnection()));
}

void CKernel::slotResetConnection()
{
    setProgrammState(eDisconnected);
}

void CKernel::slotRunCommandFromUI(const CUICommand UIcommand)
{

    qDebug() << (int) UIcommand.teUIcommand ;
    qDebug() <<  QString(UIcommand.qsArguments) ;

    switch(UIcommand.teUIcommand)
    {
        case CUICommand::eCmdConnect:
        {
            qDebug() <<  "Selected device: " << UIcommand.qsArguments;
            setProgrammState(eDisconnected);
            m_usDestinationAddress = UIcommand.qsArguments.toUShort();

            if( (m_pPipeCmd) &&( m_usDestinationAddress ))
              {
                QByteArray data;

                data.clear();
                data.append(0x20);
                data.append(0x21);
                data.append(0x22);
                m_pPipeCmd->WriteData(data, m_usDestinationAddress);

                data.clear();
                data.append(0x23);
                data.append((char)0x00);
                m_pPipeCmd->WriteData(data, m_usDestinationAddress);
            }

        }break;


        default:
        {
        }break;

    }
}

void CKernel::slotProcessDataFromPipeOfCommand(QByteArray baData, unsigned short usSenderID)
{
    int iSeek = 0;

    while ( iSeek < baData.length() )
    {
        unsigned char ucCommand = baData[iSeek++];
        switch(ucCommand)
        {

        }// switch
    }
}

void CKernel::setProgrammState(CKernel::T_ProgrammState newProgrammState)
{
    if (m_ProgrammState == newProgrammState)
        return;

    m_ProgrammState = newProgrammState;

    QString qsMessage;
    switch( m_ProgrammState )
    {
        case eConnected:
        {
            qsMessage = "Подключено";
        } break;

        default:
        {
        } break;
    }// switch

    CUICommand NewCommand(CUICommand::eCmdSetStatus);
    NewCommand.qsArguments = qsMessage;
    emit signalNewMessageToUI(NewCommand);

}

