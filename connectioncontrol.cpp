#include "connectioncontrol.h"

#include "BitBusPipes/ctcp_client_communicator.h"
#include "serialportinfo.h"
#include "BitBusPipes/PipeMgr.h"
#include <QMutex>
#include <QList>
#include "programsettings.h"

//==============================================================================
CConnectionDescriptor::CConnectionDescriptor()
{
    m_serialCommunicator= new CUSB_Communicator(CUSB_Communicator::eDevID_Usb);
    m_pipeMgr           = new CPipeMgr(100);
    connect(m_serialCommunicator, SIGNAL(ReceivePacket(TPacket)), m_pipeMgr, SLOT(SendPacket(TPacket)));
    connect(m_pipeMgr, SIGNAL(ReceivePacket(TPacket)), m_serialCommunicator, SLOT(SendPacket(TPacket)));

    CPipe *pPipeCmd = m_pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    connect(pPipeCmd, SIGNAL(ReadData(QByteArray, unsigned short)), this, SLOT(slotParceCommand(QByteArray, unsigned short)));
    m_state = eStateConnecting;

    qDebug() << "CConnectionDescriptor() ";

}

//==============================================================================
CConnectionDescriptor::~CConnectionDescriptor()
{
    delete m_pipeMgr;
    delete m_serialCommunicator;

    qDebug() << "~CConnectionDescriptor() ";
}

//==============================================================================
CConnectionDescriptor *CConnectionDescriptor::Create(QString portName)
{
    CConnectionDescriptor *newDescriptor = new CConnectionDescriptor();
    newDescriptor->m_serialCommunicator->slotSetPortName(portName);

    QByteArray baRequest;
    baRequest.append(0x24);
//    newDescriptor->m_pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand)->WriteData(baRequest, MODEM_DEV_ID);
    newDescriptor->m_pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand)->WriteData(baRequest, 21);

    return newDescriptor;
}

//==============================================================================
int CConnectionDescriptor::FindIndexByPortName(QList<CConnectionDescriptor*> descrList, QString portName)
{
    for(int i = 0; i < descrList.length(); i++ )
    {
        QString currentPortName = descrList[i]->m_serialCommunicator->getPortName();
        if ( portName == currentPortName)
            return i;
    }
    return -1;
}

//==============================================================================
void CConnectionDescriptor::slotParceCommand(QByteArray baData, unsigned short usSenderID)
{
    qDebug() << "CConnectionDescriptor::slotParceCommand ";
    int iSeek = 0;

    while ( iSeek < baData.length() )
    {
        unsigned char ucCommand = baData[iSeek++];
        switch(ucCommand)
        {
            case 0xA4:  // разбор ответа и оповещение
            {
                if(16 <= (baData.length()- iSeek))
                {
                    m_DeviceUID =  baData.mid(iSeek, 16);
                    iSeek += 16;
                    m_state = eStateConnected;
                    emit signalNewDeviceFound(this);
                }
            }break;

            default:
            {
                return;
            }break;
        }// switch
    }

}

//==============================================================================
void CConnectionDescriptor::slotTimeout()
{
    m_state = eStateConnectionTimeout;
}

//==============================================================================
//==============================================================================
CConnectionControl *CConnectionControl::st_pThis=0;
CConnectionControl* CConnectionControl::GetInstance(QObject *parent/* = 0*/)
{
    if(0 != st_pThis)
        return st_pThis;

    static QMutex mutex;
    mutex.lock();
    if (0 == st_pThis)
        st_pThis = new CConnectionControl(parent);
    mutex.unlock();

    return st_pThis;
}

CConnectionDescriptor *CConnectionControl::GetTxDescriptor()
{
    return m_TxPort;
}

CConnectionDescriptor *CConnectionControl::GetRxDescriptor()
{
    return m_RxPort;
}

//==============================================================================
CConnectionControl::CConnectionControl(QObject *parent) :
    QObject(parent)
{
    m_timerUpdate = new QTimer(this);
    m_TxPort = NULL;
    m_RxPort = NULL;
    connect(this, SIGNAL(serialPortConnected(QString)), this, SLOT(slotPortConnected(QString)));
    connect(this, SIGNAL(serialPortDisconnected(QString)), this, SLOT(slotPortDisconnected(QString)));
    // «апуск мониторинга наличи€ портов в системе
    connect(m_timerUpdate, SIGNAL(timeout()), SLOT(slotUpdate()));
    m_timerUpdate->setInterval(1000);
    m_timerUpdate->start();
}

//==============================================================================
void CConnectionControl::slotUpdate()
{
    // ѕолучение списка имеющихс€ в системе последовательных портов
    QList<SerialPortInfo> serialPortInfoList = SerialPortInfo::availablePorts();

    QList<QString> listPortsNamesNew;  // ќбновленный список доступных в системе портов

    //  опирование в локальный список толко портов пригодных дл€ работы
    foreach (const SerialPortInfo &serialPortInfo, serialPortInfoList) {
        if( !serialPortInfo.description().contains("AT91SAM"))
          continue;
        listPortsNamesNew.append(serialPortInfo.portName());  // ƒобавление порта
    }

    // ќбновленный список доступных в системе портов
    QList<QString> listPortsNamesCmpNew = listPortsNamesNew;
    // ѕредыдущий список доступных в системе портов
    QList<QString> listPortsNamesCmpPrev = m_listPortsNames;

    foreach(const QString &name, m_listPortsNames )
    {
        if (listPortsNamesCmpNew.contains(name))
        {
            listPortsNamesCmpNew.removeOne(name);
            listPortsNamesCmpPrev.removeOne(name);
        }
    }

    foreach(const QString &name, listPortsNamesCmpNew )
    {
        emit serialPortConnected(name);
        qDebug() << QObject::tr("+ Port: ") << name;
    }

    foreach(const QString &name, listPortsNamesCmpPrev )
    {
        emit serialPortDisconnected(name);
        qDebug() << QObject::tr("- Port: ") << name;
    }

    if ( ( 0 != listPortsNamesCmpNew.length() ) ||
         ( 0 != listPortsNamesCmpPrev.length() ))
    {
        m_listPortsNames = listPortsNamesNew;
    }
}

//==============================================================================
void CConnectionControl::slotPortConnected(QString portName)
{
    CConnectionDescriptor *connDescr;

    connDescr = CConnectionDescriptor::Create(portName);
    connect(connDescr, SIGNAL(signalNewDeviceFound(CConnectionDescriptor*)),
                 this, SLOT(slotNewDeviceFound(CConnectionDescriptor*)));
    m_connectionsList.append(connDescr);
}

//==============================================================================
void CConnectionControl::slotPortDisconnected(QString portName)
{
    int iInd = CConnectionDescriptor::FindIndexByPortName(m_connectionsList, portName);

    QByteArray txUID, rxUID;
    ProgramSettings ps;
    txUID = ps.value("TxUID").toByteArray();
    rxUID = ps.value("RxUID").toByteArray();

    if( txUID == m_connectionsList[iInd]->m_DeviceUID )
    {
        m_TxPort = NULL;
        emit signalTransmitterDisconnected();
    }
    if( rxUID == m_connectionsList[iInd]->m_DeviceUID )
    {
        m_RxPort = NULL;
        emit signalReceiverDisconnected();
    }
    delete m_connectionsList[iInd];
    m_connectionsList.removeAt(iInd);
}

//==============================================================================
void CConnectionControl::slotNewDeviceFound(CConnectionDescriptor *connDescr)
{
    QString strData(connDescr->m_DeviceUID.toHex());
    qDebug()<< "New device connected, UID =" << strData ;
    emit  signalDiagMsg("Device Connected " + strData);

    QByteArray txUID, rxUID;
    ProgramSettings ps;
    txUID = ps.value("TxUID").toByteArray();
    rxUID = ps.value("RxUID").toByteArray();

    if( txUID == connDescr->m_DeviceUID )
    {
        m_TxPort = connDescr;
        emit signalTransmitterConnected();
    }

    if( rxUID == connDescr->m_DeviceUID )
    {
        m_RxPort = connDescr;
        emit signalReceiverrConnected();
    }

}


