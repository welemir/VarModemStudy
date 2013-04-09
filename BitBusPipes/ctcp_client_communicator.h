#ifndef CTCP_CLIENT_COMMUNICATOR_H
#define CTCP_CLIENT_COMMUNICATOR_H

#include "CCommunicator.h"

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class CTCP_Client_Communicator : public CCommunicator
{
    Q_OBJECT

public:
    static CTCP_Client_Communicator* GetInstance(const QString &strHost, int nPort);
    static CTCP_Client_Communicator* GetInstance(void);

  QList<QString> availablePorts();

private:
  explicit CTCP_Client_Communicator(const QString &strHost, int nPort, QObject *parent = 0);
  CTCP_Client_Communicator(const CTCP_Client_Communicator &); // hide copy constructor
  CTCP_Client_Communicator& operator=(const CTCP_Client_Communicator &); // hide assign op

  /* Методы взаимодействия с маршрутизатором из ICommunicator */
signals:
  void signalConnected();   // Установлено соединение с назначенным портом
  void signalDisonnected(); // Разорвано соединение с назначенным портом

public slots:
  void slotSetHost(const QString &strHostName, const int iPort);
  void slotSetHost(const QString &strHostName);
  void slotDisonnected(); // Разорвано соединение с назначенным портом

private slots:
  // Слоты для взаимодействия с TCP Socket-ами
  void slotError(QAbstractSocket::SocketError);
  void slotConnected();
  void stotUpdate();  // Слот для таймера обновления информации о портах в системе


private:
  static CTCP_Client_Communicator* st_pThis;

  QTimer m_timerUpdate; // Таймер обновления информации о соединении

  QTcpSocket* m_pTcpSocket;
};

#endif // CTCP_CLIENT_COMMUNICATOR_H
