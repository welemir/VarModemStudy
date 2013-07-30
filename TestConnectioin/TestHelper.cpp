#include "TestHelper.h"

#include <QDebug>
#include <QThread>
#include <QTest>


TestHelper *TestHelper::m_pThis = NULL;

TestHelper *TestHelper::getInstance()
{
    if (NULL == m_pThis)
        m_pThis = new TestHelper;

    return m_pThis;
}

TestHelper::~TestHelper()
{
}

TestHelper::TestHelper(QObject *parent)
    : QObject(parent)
    , m_bTransmitterConnected(false)
    , m_bReceiverConnected(false)
{

    qRegisterMetaType<TReceivedPacketDescription>("TReceivedPacketDescription");
    m_ModulationTypeList.append(CTransceiver::eOOK);
    m_ModulationTypeList.append(CTransceiver::eFSK);

    m_ConnectionSpeedList.append(1200);
    m_ConnectionSpeedList.append(2400);
    m_ConnectionSpeedList.append(4800);
    m_ConnectionSpeedList.append(9600);
    m_ConnectionSpeedList.append(19200);
    m_ConnectionSpeedList.append(32000);

    m_TxPowerList.append(-8);
    m_TxPowerList.append(-2);
    m_TxPowerList.append(4);
    m_TxPowerList.append(10);
}

void TestHelper::slotTransmitterConnected()
{
    m_Transmitter = new CTransceiver(this);
    attachTransmitter(m_Transmitter);
    m_bTransmitterConnected = true;
}

void TestHelper::slotReceiverConnected()
{
    m_Receiver = new CTransceiver(this);
    attachReceiver(m_Receiver);
    m_bReceiverConnected = true;
}

void TestHelper::initDevices()
{
    CConnectionControl *pConnectionControl = CConnectionControl::GetInstance();
    connect(pConnectionControl, SIGNAL(signalTransmitterConnected()), this, SLOT(slotTransmitterConnected()));
    connect(pConnectionControl, SIGNAL(signalReceiverrConnected()), this, SLOT(slotReceiverConnected()));
}

void TestHelper::attachReceiver(CTransceiver *pReceiver)
{
    Q_ASSERT(pReceiver);

    if (!pReceiver) return;

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetRxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    m_RxPipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);
    connect(pReceiver, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            pReceiver, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(m_RxPipeRadioRaw, SIGNAL(ReadData(QByteArray,unsigned short)),
            pReceiver, SLOT(slotParceRadioData(QByteArray,unsigned short)) );


    // заполним карты шпионов. Адрес устройства - ключ, список шпионов - значение.
    QSignalSpy *spy = NULL;
    DviceSpyes spyesList;

    spy = new QSignalSpy(pReceiver, SIGNAL(signalNewModulationType(int)));
    spy->setObjectName("ModulationTypeSet");
    spyesList.insert(static_cast<int>(eModulationTypeSet), spy);

    spy = new QSignalSpy(pReceiver, SIGNAL(signalNewConnectionSpeed(int)));
    spy->setObjectName("ModulationSpeedSet");
    spyesList.insert(static_cast<int>(eModulationSpeedSet), spy);

    spy = new QSignalSpy(pReceiver, SIGNAL(signalNewOutputPower(int)));
    spy->setObjectName("TxPowerSet");
    spyesList.insert(static_cast<int>(eTxPowerSet), spy);

    m_Spyes.insert(pReceiver, spyesList);
}

void TestHelper::attachTransmitter(CTransceiver *pTransmitter)
{
    Q_ASSERT(pTransmitter);

    if (!pTransmitter) return;

    CPipeMgr *pipeMgr = CConnectionControl::GetInstance()->GetTxDescriptor()->m_pipeMgr;
    CPipe    *pipeCmd = pipeMgr->CreatePipe(CPipeMgr::ePipeOfCommand);
    CPipe    *pipeRadioRaw = pipeMgr->CreatePipe(CPipeMgr::ePipeOfDataRaw);

    connect(pTransmitter, SIGNAL(signalNewCommand(QByteArray,unsigned short)),
            pipeCmd, SLOT(WriteData(QByteArray,unsigned short)));
    connect(pipeCmd, SIGNAL(ReadData(QByteArray,unsigned short)),
            pTransmitter, SLOT(slotParceCommand(QByteArray,unsigned short)));
    connect(pTransmitter, SIGNAL(signalNewRawPacket(QByteArray,unsigned short)),
            pipeRadioRaw, SLOT(WriteData(QByteArray,unsigned short)));

    // заполним карты шпионов. Адрес устройства - ключ, список шпионов - значение.
    QSignalSpy *spy = NULL;
    DviceSpyes spyesList;

    spy = new QSignalSpy(pTransmitter, SIGNAL(signalNewModulationType(int)));
    spy->setObjectName("ModulationTypeSet");
    spyesList.insert(static_cast<int>(eModulationTypeSet), spy);

    spy = new QSignalSpy(pTransmitter, SIGNAL(signalNewConnectionSpeed(int)));
    spy->setObjectName("ModulationSpeedSet");
    spyesList.insert(static_cast<int>(eModulationSpeedSet), spy);

    spy = new QSignalSpy(pTransmitter, SIGNAL(signalNewOutputPower(int)));
    spy->setObjectName("TxPowerSet");
    spyesList.insert(static_cast<int>(eTxPowerSet), spy);

    m_Spyes.insert(pTransmitter, spyesList);
}

void TestHelper::askTransceiver(CTransceiver *pDevice, const TCommand_RadioModem cmd, int value)
{   
    switch (cmd)
    {
        case eModulationTypeSet:
        {
            pDevice->slotSetModulationType(static_cast<CTransceiver::T_ModulationType>(value));
        }break;

        case eModulationSpeedSet:
        {
            pDevice->slotSetConnectionSpeed(value);
        }break;

        case eTxPowerSet:
        {
            pDevice->slotSetOutputPower(value);
        } break;

    default:
        break;
    }

}

void TestHelper::askTransceiver(CTransceiver *pDevice, const QList<int> &commands, const QList<int> &values, const int &sendDelay)
{
    Q_ASSERT(commands.count());
    Q_ASSERT(values.count());
    Q_ASSERT(commands.count() == values.count());

    //pDevice->setSendPeriod(sendDelay);

    for (int i = 0; i < commands.count(); i++)
    {
        askTransceiver(pDevice, static_cast<TCommand_RadioModem>(commands.at(i)), values.at(i));
        QTest::qWait(sendDelay);
    }
}

QSignalSpy *TestHelper::spyForCommand(CTransceiver *pDevice, const int command)
{
    return m_Spyes.value(pDevice).value(command);
}

void TestHelper::prepare()
{
//    qDebug() << "cleaning" << m_Spyes.count() << "Devices";
    foreach (DviceSpyes SpyesList, m_Spyes.values())
    {
//        qDebug() << "cleaning" << SpyesList.count() << "spyes";
        foreach(QSignalSpy *spy, SpyesList.values())
        {
//            qDebug() << "cleaning spy" << spy->objectName() << "for" << spy->count() << "results";
            spy->clear();
        }
    }
}

void TestHelper::startRxMeasure(QList<int> *resultList)
{
    m_iRxRawDataSize = 0;
    m_pRawDataRxIntervals = resultList;
    connect(m_RxPipeRadioRaw, SIGNAL(ReadData(QByteArray,unsigned short)), this, SLOT(slotRxRawDataCame(QByteArray, unsigned short)));
    //m_Receiver->setSendPeriod(20);
    m_Receiver->slotRxStart();
    m_RawDataRxMeasureTimer.restart();
}

void TestHelper::stopRxMeasure()
{
    m_Receiver->slotRxStop();
    disconnect(m_RxPipeRadioRaw, SIGNAL(ReadData(QByteArray, unsigned short)), this, SLOT(slotRxRawDataCame(QByteArray, unsigned short)));
}

void TestHelper::startTxMeasure(QList<int> *resultList)
{
    m_pRawDataTxIntervals = resultList;
    //m_Transmitter->setSendPeriod(20);
    m_Transmitter->slotTxStart();
    m_RawDataTxMeasureTimer.restart();
}

void TestHelper::slotRxRawDataCame(QByteArray baData, unsigned short usSenderID)
{
    Q_UNUSED(usSenderID);

    m_iRxRawDataSize += baData.count();
    m_pRawDataRxIntervals->append(m_RawDataRxMeasureTimer.elapsed());
    m_RawDataRxMeasureTimer.restart();
}

void TestHelper::slotTxRawDataCame()
{
    m_pRawDataTxIntervals->append(m_RawDataRxMeasureTimer.elapsed());
    m_RawDataTxMeasureTimer.restart();
}
