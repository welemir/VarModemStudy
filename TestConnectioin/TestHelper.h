#ifndef CONNECTIONCONTROLCONTROLLER_H
#define CONNECTIONCONTROLCONTROLLER_H

#include <QObject>
#include <QSignalSpy>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>

#include "CommandCode_RadioModem.h"
#include "connectioncontrol.h"
#include "ctransceiver.h"

#define log qCritical

#define PACKET_TIMEOUT 100

typedef QMap<int, QSignalSpy *> DviceSpyes; // карта шпионов - команда к шпиону
typedef QMap<CTransceiver *, DviceSpyes> DevicesSpyDescriptor; // карта уникальных устройств и их карт шпионов



// Класс-помощник для тестирования системы.
// Работает с устройствами, хранит промежуточные результаты работы, исследуемые внешним тестом.
class TestHelper : public QObject
{
    Q_OBJECT

public:
    static TestHelper *getInstance();

    ~TestHelper();

private:
    explicit TestHelper(QObject *parent = 0);

public:
    void initDevices();
    bool isRxConnected() {return m_bReceiverConnected;}
    bool isTxConnected() {return m_bTransmitterConnected;}
    void attachReceiver(CTransceiver *pReceiver);
    void attachTransmitter(CTransceiver *pTransmitter);
    CTransceiver *receiver() {return m_Receiver;}
    CTransceiver *transmitter() {return m_Transmitter;}
    QList<int> modulationTypeList() {return m_ModulationTypeList;}
    QList<int> connectionSpeedList() {return m_ConnectionSpeedList;}
    QList<int> txPowerList() {return m_TxPowerList;}
    void askTransceiver(CTransceiver *pDevice, const TCommand_RadioModem cmd, int value);
    void askTransceiver(CTransceiver *pDevice, const QList<int> &commands, const QList<int> &values, const int &sendDelay);
    QSignalSpy *spyForCommand(CTransceiver *pDevice, const int command);
    void prepare();

    void startRxMeasure(QList<int> *resultList);
    void stopRxMeasure();
    void startTxMeasure(QList<int> *resultList);
    int rxRawDataSize(){return m_iRxRawDataSize;}

private slots:
    void slotTransmitterConnected();
    void slotReceiverConnected();

    void slotRxRawDataCame(QByteArray baData, unsigned short usSenderID);
    void slotTxRawDataCame();

private:
    static TestHelper *m_pThis;
    CTransceiver *m_Transmitter;
    CTransceiver *m_Receiver;
    CPipe *m_RxPipeRadioRaw;
    bool m_bTransmitterConnected;
    bool m_bReceiverConnected;

    QList<int> m_ModulationTypeList;
    QList<int> m_ConnectionSpeedList;
    QList<int> m_TxPowerList;
    QList<int> m_Answers;

    DevicesSpyDescriptor m_Spyes;


    QList<int> *m_pRawDataRxIntervals;
    QElapsedTimer m_RawDataRxMeasureTimer;

    QList<int> *m_pRawDataTxIntervals;
    int m_iRxRawDataSize;
    QTime m_RawDataTxMeasureTimer;

    QTime m_TxMeasureTimer;
};


#endif // CONNECTIONCONTROLCONTROLLER_H
