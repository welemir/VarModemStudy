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
    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance(this);
    connect(pConnectionControl, SIGNAL(signalTransmitterConnected()), this, SLOT(slotTransmitterConnected()));
    connect(pConnectionControl, SIGNAL(signalTransmitterDisconnected()), this, SLOT(slotTransmitterDisconnected()));

    connect(&m_Transmitter, SIGNAL(signalNewModulationType( T_ModulationType )), this, SLOT(slotNewModulationType(CTransceiver::T_ModulationType)));
    connect(&m_Transmitter, SIGNAL(signalNewConnectionSpeed( int )), this, SLOT(slotNewConnectionSpeed(int)));
    connect(&m_Transmitter, SIGNAL(signalNewOutputPower( int )), this, SLOT(slotNewOutputPower(int)));
//    connect(&m_Transmitter, SIGNAL(signalNewBitSynchLength( int )), this, SLOT();
//    connect(&m_Transmitter, SIGNAL(signalNewSychnroSequence( QByteArray )), this, SLOT());
//    connect(&m_Transmitter, SIGNAL(signalNewDataPacketLength( int )), this, SLOT());
//    connect(&m_Transmitter, SIGNAL(signalNewCrcType( T_CrcType )), this, SLOT()));


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

void CKernel::slotTransmitterConnected()
{
    QString msg = " CKernel::slotTransmitterConnected()";
    emit signalPrintDiagMeaasge(msg);
    emit signalTxStateUpdated(true);

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetTxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    connect(&m_Transmitter, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            &m_Transmitter, SLOT(slotParceCommand(QByteArray,unsigned short)));

  //  m_Transmitter.slotSetDeviceMode(CTransceiver::eTransmitter);

}

void CKernel::slotTransmitterDisconnected()
{
    QString msg = " CKernel::slotTransmitterDisconnected()";
    emit signalPrintDiagMeaasge(msg);
    emit signalTxStateUpdated(false);
}

void CKernel::slotSetConnectionSpeed(QString newSpeed)
{
    m_Transmitter.slotSetConnectionSpeed( newSpeed.toInt() );
}

void CKernel::slotSetOutputPower(QString newPower)
{
    m_Transmitter.slotSetOutputPower( newPower.toInt() );
}

void CKernel::slotSetModulationType(int newModIndex)
{
    m_Transmitter.slotSetModulationType( (CTransceiver::T_ModulationType) newModIndex );
}

void CKernel::slotSetBitSynchLength(QString newLength)
{
    m_Transmitter.slotSetBitSynchLength( newLength.toInt() );
}

void CKernel::slotSetSychnroSequenceLength(QString newLength)
{
    QByteArray baSequence;
    m_Transmitter.slotSetSychnroSequence( baSequence );
}

void CKernel::slotSetDataPacketLength(QString newLength)
{
    m_Transmitter.slotSetDataPacketLength( newLength.toInt() );
}

void CKernel::slotSetTotalDataLength(QString newLength)
{
}

void CKernel::slotSetCrcType(int newCrcIndex)
{
    m_Transmitter.slotSetCrcType( (CTransceiver::T_CrcType) newCrcIndex );
}

void CKernel::slotNewModulationType(CTransceiver::T_ModulationType newModulaton)
{
    int newModulatonInd = newModulaton;
    emit signalNewModulationType(newModulatonInd);
}

void CKernel::slotNewConnectionSpeed(int newSpeed)
{
    QString qsNewMessaage = QString("%1").arg(newSpeed);
    emit signalNewConnectionSpeed(qsNewMessaage);
}

void CKernel::slotNewOutputPower(int newPower)
{
    QString qsNewMessaage = QString("%1").arg(newPower);
    emit signalNewOutputPower(qsNewMessaage);
}

void CKernel::slotStartOperation()
{

}

void CKernel::slotStopOperation()
{

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

