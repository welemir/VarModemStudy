/* USB_Communicator.cpp - ���������� ������� ������ ����� USB ���������
*/
#include <string.h>

#include "USB_Communicator.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QMutex>
#include <QDebug>

#define BITBUS_MARK 0x7e

/*******************************************************************************/
// �������� ��������� ��� ������ ����� � ������ ���������
bool operator==(const QSerialPortInfo &first, const QSerialPortInfo &second)
{
  return first.portName()==second.portName();
}

/*******************************************************************************
����� CUSB_Communicator - ��������� ������ � USB ����� ��������� ICommunicator
                          � �������������� ���������������������� �������
*******************************************************************************/
CUSB_Communicator* CUSB_Communicator::st_pThis;

//==============================================================================
CUSB_Communicator::CUSB_Communicator(unsigned short usDeviceID_Usb):
  m_usDeviceID(usDeviceID_Usb),
  m_pSerialPort(0)
{
  // ���������������� ���� ��� ������ USB
  m_pSerialPort = new QSerialPort(this);
  m_pIODevice = m_pSerialPort;

  connect(m_pSerialPort, SIGNAL(readyRead()), this, SLOT(slotReadyRead()), Qt::QueuedConnection);
}

CUSB_Communicator::~CUSB_Communicator()
{
    m_pSerialPort->close();
    qDebug() << "    m_pSerialPort->close();";

}

//==============================================================================
void CUSB_Communicator::slotSetPortName(const QString &strPortName)
{
  if(m_pSerialPort->portName() == strPortName)
    return;

  emit signalPortSelected(strPortName);
    // ���������� �����, ���� ���� ���������� � �������� ���
  if(m_pSerialPort->isOpen())
  {
    m_pSerialPort->close();
    emit signalDisonnected();

    qDebug() << QObject::tr("Disconnected from ") << m_pSerialPort->portName();
  }

  // ��������� ������ ����� �����
  m_pSerialPort->setPortName(strPortName);

  m_pSerialPort->open(QIODevice::ReadWrite);
  m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl);
  m_pSerialPort->setBreakEnabled(false);
  m_pSerialPort->setTextModeEnabled(false);
}

QString CUSB_Communicator::getPortName()
{
    return m_pSerialPort->portName();
}

//==============================================================================
