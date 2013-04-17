/* USB_Communicator.cpp - Реализация классов обмена через USB интерфейс
*/
#include <string.h>

#include "USB_Communicator.h"
#include "serialportinfo.h"

#include <QMutex>
#include <QDebug>

#define BITBUS_MARK 0x7e

/*******************************************************************************/
// Оператор сравнения для поиска порта в списке имеющихся
bool operator==(const SerialPortInfo &first, const SerialPortInfo &second)
{
  return first.portName()==second.portName();
}

/*******************************************************************************
Класс CUSB_Communicator - поддержка работы с USB через интерфейс ICommunicator
                          с использованием внешнегомаршрутизатора пакетов
*******************************************************************************/
CUSB_Communicator* CUSB_Communicator::st_pThis;

//==============================================================================
CUSB_Communicator::CUSB_Communicator(unsigned short usDeviceID_Usb):
  m_usDeviceID(usDeviceID_Usb),
  m_pSerialPort(0)
{
  // Последовательный порт для работы USB
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
    // Отключение порта, если есть соединение и изменено имя
  if(m_pSerialPort->isOpen() && m_pSerialPort->portName() != strPortName)
  {
    m_pSerialPort->close();
    emit signalDisonnected();

    qDebug() << QObject::tr("Disconnected from ") << m_pSerialPort->portName();
  }

  // Установка нового имени порта
  m_pSerialPort->setPort(strPortName);

  m_pSerialPort->open(QIODevice::ReadWrite);

}

QString CUSB_Communicator::getPortName()
{
    return m_pSerialPort->portName();
}

//==============================================================================
