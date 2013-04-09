#include "connectioncontrol.h"
#include "BitBusPipes/USB_Communicator.h"
#include "BitBusPipes/ctcp_client_communicator.h"

#include <QMutex>

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


CConnectionControl::CConnectionControl(QObject *parent) :
    QObject(parent)
{
    selectCommunicatorRS();
}

void CConnectionControl::attachUI(QObject *pUIObject)
{
    m_pUIObject = pUIObject;

    CUSB_Communicator *pCommunicatorUSB = CUSB_Communicator::GetInstance(CUSB_Communicator::eDevID_Unknown);
    // Присоединение сигнала о выборе пользователем порта для связи
    connect(m_pUIObject, SIGNAL(signalSetRSPort(QString)), pCommunicatorUSB, SLOT(slotSetPortName(QString)));
    connect(pCommunicatorUSB, SIGNAL(signalPortSelected(QString)), m_pUIObject, SLOT(slotSetRSPortActive(QString)));
    // Присоединение сигнала оповещения о изменении списка подключённых портов
    connect(pCommunicatorUSB, SIGNAL(signalPortListUpdated(QList<QString>,QString)), m_pUIObject, SLOT(slotSetRSPortList(QList<QString>, QString)));

    CCommunicator *pCommunicatorTCP = CTCP_Client_Communicator::GetInstance("", 2323);
    connect(m_pUIObject, SIGNAL(signalSetServerName(QString)), pCommunicatorTCP, SLOT(slotSetHost(QString)));
    connect(this, SIGNAL(setNetworkDevicesList(QList<QString>, QString)), pUIObject, SLOT(slotSetNetworkDevicesList(QList<QString>, QString)));

    connect(m_pUIObject, SIGNAL(signalSetConnectionTypeUSB()), this, SLOT(selectCommunicatorRS()));
    connect(m_pUIObject, SIGNAL(signalSetConnectionTypeTCP()), this, SLOT(selectCommunicatorTCP()));

    CPipeMgr *pPipeMgr = CPipeMgr::GetInstance();
    m_pPipeCmd = pPipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    connect(m_pPipeCmd, SIGNAL(ReadData(QByteArray, unsigned short)), this, SLOT(slotParceCommand(QByteArray, unsigned short)));

    connect(m_pUIObject, SIGNAL(signalSetNetworkDeviceID(QString)), this, SLOT(slotSetNetworkDeviceID(QString)));
}

void CConnectionControl::slotSetNetworkDeviceID(QString strDeviceID)
{
    emit resetConnection();

    QByteArray data;
    if(!m_strTCPDeviceIMEICurent.isEmpty())
    {
        // Отпарвке команды на отключение от теккщего устройства
        data.append(0x73/*eDisconnectFromDevice*/);
        data.append(m_strTCPDeviceIMEICurent);
        m_pPipeCmd->WriteData(data, m_usTCPServerID);
    }

    m_strTCPDeviceIMEICurent = strDeviceID;

    // Отпарвке команды на подключение к новому устройству
    data.clear();
    data.append(0x72/*eConnectToDevice*/);
    data.append(m_strTCPDeviceIMEICurent);
    m_pPipeCmd->WriteData(data, m_usTCPServerID);
}

void CConnectionControl::selectCommunicatorRS()
{
    CCommunicator *pCommunicatorOld = CTCP_Client_Communicator::GetInstance("", 2323);
    CCommunicator *pCommunicatorNew = CUSB_Communicator::GetInstance(CUSB_Communicator::eDevID_Unknown);
    switchCommunicator(pCommunicatorOld, pCommunicatorNew);
}

void CConnectionControl::selectCommunicatorTCP()
{
//    if(bIsTCP)
    {
        CUSB_Communicator *pCommunicatorOld = CUSB_Communicator::GetInstance(CUSB_Communicator::eDevID_Unknown);
        pCommunicatorOld->slotSetPortName("");
        CCommunicator *pCommunicatorNew = CTCP_Client_Communicator::GetInstance("", 2323);
        switchCommunicator(pCommunicatorOld, pCommunicatorNew);
    }
}

void CConnectionControl::slotDisonnected()
{
    emit resetConnection();
}

void CConnectionControl::slotParceCommand(QByteArray baData, unsigned short usSenderID)
{
    int iSeek = 0;

    while ( iSeek < baData.length() )
    {
        unsigned char ucCommand = baData[iSeek++];
        switch(ucCommand)
        {
            case 0x20:
            {
                QByteArray baAnswer;
                baAnswer.append(0xa0);
                baAnswer.append(0x81);
                m_pPipeCmd->WriteData(baAnswer, usSenderID);
                m_usTCPServerID = usSenderID;
            }break;
        case /*eRegistered*/0x70:
            {
                QByteArray baAnswer;
                baAnswer.append(0x71/*eGetDevicesAvailable*/);
                m_pPipeCmd->WriteData(baAnswer, usSenderID);

                m_listDevicesID.clear();
            }break;
        case /**/0xf1:
            {
                QByteArray baDevID(baData.constData()+iSeek, baData.length() - iSeek);

                m_listDevicesID.append(QString(baDevID));
                emit setNetworkDevicesList(m_listDevicesID, m_listDevicesID[0]);
            }break;

            case 81: // пакеты FWupdater-a, нам не интересны
            case 82:
            {
                return;
            }break;


        }// switch
    }
}

void CConnectionControl::switchCommunicator(CCommunicator *pCommunicatorOld, CCommunicator *pCommunicatorNew)
{
    CPipeMgr *pPipeMgr = CPipeMgr::GetInstance();

    if(0 != pCommunicatorOld)
    {
        QObject::disconnect(pCommunicatorOld, SIGNAL(ReceivePacket(TPacket)), pPipeMgr, SLOT(SendPacket(TPacket)));
        QObject::disconnect(pPipeMgr, SIGNAL(ReceivePacket(TPacket)), pCommunicatorOld, SLOT(SendPacket(TPacket)));
        QObject::disconnect(pCommunicatorOld, SIGNAL(signalDisonnected()), this, SLOT(slotDisonnected()));
    }

    QObject::connect(pCommunicatorNew, SIGNAL(ReceivePacket(TPacket)), pPipeMgr, SLOT(SendPacket(TPacket)));
    QObject::connect(pPipeMgr, SIGNAL(ReceivePacket(TPacket)), pCommunicatorNew, SLOT(SendPacket(TPacket)));
    QObject::connect(pCommunicatorNew, SIGNAL(signalDisonnected()), this, SLOT(slotDisonnected()));
}
