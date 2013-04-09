#include "ctcp_client_communicator.h"

#include <QMutex>
#include <QDebug>

/*******************************************************************************
Класс CTCP_Client_Communicator - поддержка работы с TCP socket-ами через интерфейс
                    ICommunicator с использованием внешнего маршрутизатора пакетов
*******************************************************************************/
CTCP_Client_Communicator* CTCP_Client_Communicator::st_pThis;

//==============================================================================
CTCP_Client_Communicator* CTCP_Client_Communicator::GetInstance(void)
{
    return st_pThis;
}

//==============================================================================
CTCP_Client_Communicator* CTCP_Client_Communicator::GetInstance(const QString &strHost, int nPort)
{
    if(st_pThis)
    {
      st_pThis->slotSetHost(strHost, nPort);
      return st_pThis;
    }

    static QMutex mutex;

    mutex.lock();
    if (!st_pThis)
        st_pThis = new CTCP_Client_Communicator(strHost, nPort);

    mutex.unlock();

    return st_pThis;
}

CTCP_Client_Communicator::CTCP_Client_Communicator(const QString &strHost, int nPort, QObject *parent/*=0*/) :
    CCommunicator(parent),
    m_pTcpSocket(0)
{
    // Запуск мониторинга наличия связи с сервером
    connect(&m_timerUpdate, SIGNAL(timeout()), SLOT(stotUpdate()));
    m_timerUpdate.setInterval(1000);
    m_timerUpdate.start();

    m_pTcpSocket = new QTcpSocket(this);
    m_pIODevice = m_pTcpSocket;

    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketAccessError)));
}

void CTCP_Client_Communicator::slotSetHost(const QString &strHostName)
{
    slotSetHost(strHostName, 2323);
}

void CTCP_Client_Communicator::slotSetHost(const QString &strHostName, const int iPort)
{
    m_pTcpSocket->abort();
    emit signalDisonnected();

    m_pTcpSocket->connectToHost(strHostName/*hostAddressNew*/, iPort);
}


void CTCP_Client_Communicator::stotUpdate()
{
}

void CTCP_Client_Communicator::slotError(QAbstractSocket::SocketError err)
{
  QString strError = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                    "The host was not found." :
                                    err == QAbstractSocket::RemoteHostClosedError ?
                                      "The remote host is closed." :
                                      err == QAbstractSocket::ConnectionRefusedError ?
                                        "The connection was refused." :
                                        QString(m_pTcpSocket->errorString()));
  qDebug() << strError;
}

void CTCP_Client_Communicator::slotConnected()
{
  qDebug() << "Received the Connected() signal";
}

void CTCP_Client_Communicator::slotDisonnected()
{
  emit signalDisonnected();
  qDebug() << "Received the Disconnected() signal";
}
