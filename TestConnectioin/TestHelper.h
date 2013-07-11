#ifndef CONNECTIONCONTROLCONTROLLER_H
#define CONNECTIONCONTROLCONTROLLER_H

#include <QObject>

class CTransceiver;
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
    
public slots:
    void slotTransmitterConnected();
    void slotReceiverConnected();

private:
    static TestHelper *m_pThis;
    CTransceiver *m_Transmitter;
    CTransceiver *m_Receiver;
    bool m_bTransmitterConnected;
    bool m_bReceiverConnected;
    
};

#endif // CONNECTIONCONTROLCONTROLLER_H
