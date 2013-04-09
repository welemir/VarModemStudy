/* USB_Communicator.cpp - ���������� ������� ������ ����� USB ���������
*/
#include <string.h>

#include "USB_Communicator.h"
#include "serialportinfo.h"

#include <QMutex>
#include <QDebug>

#define BITBUS_MARK 0x7e

/*******************************************************************************/
// �������� ��������� ��� ������ ����� � ������ ���������
bool operator==(const SerialPortInfo &first, const SerialPortInfo &second)
{
  return first.portName()==second.portName();
}

/*******************************************************************************
����� CUSB_Communicator - ��������� ������ � USB ����� ��������� ICommunicator
                          � �������������� ���������������������� �������
*******************************************************************************/
CUSB_Communicator* CUSB_Communicator::st_pThis;

//==============================================================================
CUSB_Communicator* CUSB_Communicator::GetInstance(unsigned short usDeviceID_Usb/* = eDevID_Usb*/)
{
    if(st_pThis)
      return st_pThis;

    static QMutex mutex;

    mutex.lock();
    if (!st_pThis)
        st_pThis = new CUSB_Communicator(usDeviceID_Usb);

    mutex.unlock();

    return st_pThis;
  }

//==============================================================================
CUSB_Communicator::CUSB_Communicator(unsigned short usDeviceID_Usb):
  m_usDeviceID(usDeviceID_Usb),
  m_pSerialPort(0)
{
  // ������ ����������� ������� ������ � �������
  connect(&m_timerUpdate, SIGNAL(timeout()), SLOT(stotUpdate()));
  m_timerUpdate.setInterval(1000);
  m_timerUpdate.start();

  // ���������������� ���� ��� ������ USB
  m_pSerialPort = new SerialPort(this);
  m_pIODevice = m_pSerialPort;

  connect(m_pSerialPort, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
}

//==============================================================================
QList<QString> CUSB_Communicator::availablePorts()
{
  return QList<QString>(m_listPortsNames);
}

//==============================================================================
void CUSB_Communicator::stotUpdate()
{
  // ��������� ������ ��������� � ������� ���������������� ������
  QList<SerialPortInfo> serialPortInfoList = SerialPortInfo::availablePorts();

    QList<QString> listPortsNamesNew;  // ������ ��������� � ������� ������

    // ����������� � ��������� ������ ����� ������ ��������� ��� ������
    foreach (const SerialPortInfo &serialPortInfo, serialPortInfoList) {
        if( !serialPortInfo.description().contains("AT91SAM"))
          continue;

    listPortsNamesNew.append(serialPortInfo.portName());  // ���������� �����
  }

  // �������� ������ �� ���������
  bool bUpdated = true;
  if(listPortsNamesNew.length() == m_listPortsNames.length())
    { // ������ ���������, ��������� ��������� �� ������
      bUpdated = false;
      foreach(const QString &name, listPortsNamesNew )
        {
          if (!m_listPortsNames.contains(name))
            { // ���� ������������ ��� - ������ ���������
              bUpdated = true;
              break;
            }
        }
    }

  if(bUpdated)
    {
      // ��������� ���������� ������
      m_listPortsNames = listPortsNamesNew;

      // ����� ������ ����� ������ ��� �������
      qDebug() << endl << "Ports list updated:";
      foreach(const QString &name, listPortsNamesNew )
        {
          qDebug() << QObject::tr("Port: ") << name;
        }

      // �������� ��������� �� ��������� ������ ������
      emit(signalPortListUpdated(m_listPortsNames, m_pSerialPort->portName()));

      // ���� ���� ��������� � ��� ������ ��� � ������ - ���������
      if((0 > m_listPortsNames.indexOf(m_pSerialPort->portName())) && m_pSerialPort->isOpen())
      {
        m_pSerialPort->close();
        emit signalDisonnected();

        qDebug() << QObject::tr("Disconnected from ") << m_pSerialPort->portName();
      }

       // ���� ���� ������������ � �� ��������� - ����������
      if((0 <= m_listPortsNames.indexOf(m_pSerialPort->portName())) && !m_pSerialPort->isOpen())
      {
        m_pSerialPort->open(QIODevice::ReadWrite);
        emit signalConnected();

        qDebug() << QObject::tr("Connected to ") << m_pSerialPort->portName() << m_pSerialPort->isOpen();
      }
    }
}

//==============================================================================
void CUSB_Communicator::slotSetPortName(const QString &strPortName)
{
  emit signalPortSelected(strPortName);
    // ���������� �����, ���� ���� ���������� � �������� ���
  if(m_pSerialPort->isOpen() && m_pSerialPort->portName() != strPortName)
  {
    m_pSerialPort->close();
    emit signalDisonnected();

    qDebug() << QObject::tr("Disconnected from ") << m_pSerialPort->portName();
  }

  // ��������� ������ ����� �����
  m_pSerialPort->setPort(strPortName);

  // ����������� ������ �����, ��� �����������
  if((0 <= m_listPortsNames.indexOf(m_pSerialPort->portName())) && !m_pSerialPort->isOpen())
  {
    m_pSerialPort->open(QIODevice::ReadWrite);
    emit signalConnected();

    qDebug() << QObject::tr("Connected to ") << m_pSerialPort->portName() << m_pSerialPort->isOpen();
  }
}

//==============================================================================
