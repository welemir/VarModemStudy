#ifndef CONNECTIONCONTROLCONTROLLER_H
#define CONNECTIONCONTROLCONTROLLER_H

#include <QObject>
#include <QSignalSpy>

#include "CommandCode_RadioModem.h"
#include "ctransceiver.h"

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

private slots:
    void slotTransmitterConnected();
    void slotReceiverConnected();

private:
    static TestHelper *m_pThis;
    CTransceiver *m_Transmitter;
    CTransceiver *m_Receiver;
    bool m_bTransmitterConnected;
    bool m_bReceiverConnected;

    QList<int> m_ModulationTypeList;
    QList<int> m_ConnectionSpeedList;
    QList<int> m_TxPowerList;
    QList<int> m_Answers;

    DevicesSpyDescriptor m_Spyes;

    // для тестирования корректности установки настроек будем ожидать ответ с нужной настройкой
    int m_awaitingTransmitterAnswer;
    int m_awaitingReceiverAnswer;
    int m_iReceiverAnswer;
    int m_iTransmitterAnswer;

    QTimer m_senderTimer;
};


#endif // CONNECTIONCONTROLCONTROLLER_H
