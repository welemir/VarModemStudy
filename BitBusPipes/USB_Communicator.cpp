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
CUSB_Communicator::CUSB_Communicator(unsigned short usDeviceID_Usb):
  m_usDeviceID(usDeviceID_Usb),
  m_pSerialPort(0)
{
  // ���������������� ���� ��� ������ USB
  m_pSerialPort = new SerialPort(this);
  m_pIODevice = m_pSerialPort;

  connect(m_pSerialPort, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
}

CUSB_Communicator::~CUSB_Communicator()
{
    m_pSerialPort->close();
    qDebug() << "    m_pSerialPort->close();";

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

  m_pSerialPort->open(QIODevice::ReadWrite);

}

QString CUSB_Communicator::getPortName()
{
    return m_pSerialPort->portName();
}

//==============================================================================
