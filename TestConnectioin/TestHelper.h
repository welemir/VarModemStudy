#ifndef CONNECTIONCONTROLCONTROLLER_H
#define CONNECTIONCONTROLCONTROLLER_H

#include <QObject>
#include <QSignalSpy>

#include "CommandCode_RadioModem.h"
#include "ctransceiver.h"

#define PACKET_TIMEOUT 300

//  ласс-помощник дл€ тестировани€ системы.
// –аботает с устройствами, хранит промежуточные результаты работы, исследуемые внешним тестом.
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
    QSignalSpy *askTransceiver(CTransceiver *pDevice, const TCommand_RadioModem cmd, int value);
    QList<QSignalSpy*> askTransceiver(CTransceiver *pDevice, QList<TCommand_RadioModem> commands, QList<int> values, int sendDelay);
    int transmitterAnswer() {return m_iTransmitterAnswer;}
    int receiverAnswer() {return m_iReceiverAnswer;}

private slots:
    void slotTransmitterConnected();
    void slotReceiverConnected();
    void transmitterModulationTypeChanged(CTransceiver::T_ModulationType value);
    void receiverModulationTypeChanged(CTransceiver::T_ModulationType value);
    void transmitterModulationSpeedChanged(int value);
    void receiverModulationSpeedChanged(int value);
    void transmitterTxPowerChanged(int value);
    void receiverTxPowerChanged(int value);

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

    // дл€ тестировани€ корректности установки настроек будем ожидать ответ с нужной настройкой
    int m_awaitingTransmitterAnswer;
    int m_awaitingReceiverAnswer;
    int m_iReceiverAnswer;
    int m_iTransmitterAnswer;
};


#endif // CONNECTIONCONTROLCONTROLLER_H
