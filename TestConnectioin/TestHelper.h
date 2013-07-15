#ifndef CONNECTIONCONTROLCONTROLLER_H
#define CONNECTIONCONTROLCONTROLLER_H

#include <QObject>

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
    void attachReceiver();
    void attachTransmitter();
    void askTransmitter(const TCommand_RadioModem cmd, int value);
    void askReceiver(const TCommand_RadioModem cmd, int value);
    int transmitterAnswer() {return m_iTransmitterAnswer;}
    int receiverAnswer() {return m_iReceiverAnswer;}

private slots:
    void slotTransmitterConnected();
    void slotReceiverConnected();
    void transmitterModulationTypeChanged(CTransceiver::T_ModulationType value);
    void receiverModulationTypeChanged(CTransceiver::T_ModulationType value);
    void transmitterModulationSpeedChanged(int value);
    void receiverModulationSpeedChanged(int value);

private:
    // обобщение askTransmitter и askReceiver
    void askTransceiver(CTransceiver *device, const TCommand_RadioModem cmd, int value, TInfoType_RadioModem &waitFor);
    void transceiverModulationTypeChanged();

private:
    static TestHelper *m_pThis;
    CTransceiver *m_Transmitter;
    CTransceiver *m_Receiver;
    bool m_bTransmitterConnected;
    bool m_bReceiverConnected;

    // дл€ тестировани€ корректности установки настроек будем ожидать ответ с нужной настройкой
    TInfoType_RadioModem m_awaitingTransmitterAnswer;
    TInfoType_RadioModem m_awaitingReceiverAnswer;
    int m_iReceiverAnswer;
    int m_iTransmitterAnswer;
};

#endif // CONNECTIONCONTROLCONTROLLER_H
