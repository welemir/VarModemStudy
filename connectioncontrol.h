#ifndef CONNECTIONCONTROL_H
#define CONNECTIONCONTROL_H

#include "BitBusPipes/CCommunicator.h"
#include "BitBusPipes/PipeMgr.h"
#include "BitBusPipes/USB_Communicator.h"

#include <QObject>
#include <QTimer>

#define CONNECTION_TIMEOUT 1000
class CConnectionDescriptor : public QObject
{
    Q_OBJECT
public:
    typedef enum{ // состояния порта
      eStateConnectionTimeout, // устройство на порту не ответило
      eStateConnecting,        // ожидание ответа
      eStateConnected          // ответ получен, устройство идентифицировно
    }T_ConnectionState;

public:
    static int FindIndexByPortName( QList<CConnectionDescriptor*> descrList, QString portName );
    static CConnectionDescriptor* Create(QString portName);

signals:
    void signalNewDeviceFound( CConnectionDescriptor* );

private:
    CConnectionDescriptor();

public:
    ~CConnectionDescriptor();

private slots:
    void slotParceCommand(QByteArray baData, unsigned short usSenderID);
    void slotTimeout();

public:
    CUSB_Communicator   *m_serialCommunicator;
    CPipeMgr            *m_pipeMgr;
    T_ConnectionState    m_state;
    QByteArray           m_DeviceUID;

private:
    QTimer               m_timer;
};

class CConnectionControl : public QObject
{
    Q_OBJECT

public:
    static CConnectionControl* GetInstance(QObject *parent = 0);
    CConnectionDescriptor* GetTxDescriptor();
    CConnectionDescriptor* GetRxDescriptor();
signals:
    void signalTransmitterConnected();
    void signalTransmitterDisconnected();
    void signalReceiverrConnected();
    void signalReceiverDisconnected();
    void signalDiagMsg(QString);

private:
    explicit CConnectionControl(QObject *parent = 0);
    
signals:
    // внутренние сигналы для отработки подключения-отключения портов
    void serialPortConnected(QString);
    void serialPortDisconnected(QString);

private slots:
  void slotUpdate();  // Слот для таймера обновления информации о портах в системе
  void slotPortConnected(QString portName);
  void slotPortDisconnected(QString portName);
  void slotNewDeviceFound( CConnectionDescriptor* connDescr );

private:
    CConnectionDescriptor*     m_TxPort;
    CConnectionDescriptor*     m_RxPort;
    static CConnectionControl *st_pThis;
    QTimer m_timerUpdate;
    QList<QString> m_listPortsNames;  // Список доступных в системе портов
    QList<CConnectionDescriptor*> m_connectionsList;
};

#endif // CONNECTIONCONTROL_H
