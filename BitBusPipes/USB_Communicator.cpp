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
  // Запуск мониторинга наличия портов в системе
  connect(&m_timerUpdate, SIGNAL(timeout()), SLOT(stotUpdate()));
  m_timerUpdate.setInterval(1000);
  m_timerUpdate.start();

  // Последовательный порт для работы USB
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
  // Получение списка имеющихся в системе последовательных портов
  QList<SerialPortInfo> serialPortInfoList = SerialPortInfo::availablePorts();

    QList<QString> listPortsNamesNew;  // Список доступных в системе портов

    // Копирование в локальный список толко портов пригодных для работы
    foreach (const SerialPortInfo &serialPortInfo, serialPortInfoList) {
        if( !serialPortInfo.description().contains("AT91SAM"))
          continue;

    listPortsNamesNew.append(serialPortInfo.portName());  // Добавление порта
  }

  // Проверка списка на изменение
  bool bUpdated = true;
  if(listPortsNamesNew.length() == m_listPortsNames.length())
    { // Размер совпадает, проверяем изменился ли состав
      bUpdated = false;
      foreach(const QString &name, listPortsNamesNew )
        {
          if (!m_listPortsNames.contains(name))
            { // Есть несовпадение имён - список изменился
              bUpdated = true;
              break;
            }
        }
    }

  if(bUpdated)
    {
      // сохраняем обновлённый список
      m_listPortsNames = listPortsNamesNew;

      // Вывод нового спска портов для отладки
      qDebug() << endl << "Ports list updated:";
      foreach(const QString &name, listPortsNamesNew )
        {
          qDebug() << QObject::tr("Port: ") << name;
        }

      // Отправка сообщения об изменении списка портов
      emit(signalPortListUpdated(m_listPortsNames, m_pSerialPort->portName()));

      // Если порт подключён и его больше нет в списке - отключаем
      if((0 > m_listPortsNames.indexOf(m_pSerialPort->portName())) && m_pSerialPort->isOpen())
      {
        m_pSerialPort->close();
        emit signalDisonnected();

        qDebug() << QObject::tr("Disconnected from ") << m_pSerialPort->portName();
      }

       // Если порт присутствует и не подключён - подключаем
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
    // Отключение порта, если есть соединение и изменено имя
  if(m_pSerialPort->isOpen() && m_pSerialPort->portName() != strPortName)
  {
    m_pSerialPort->close();
    emit signalDisonnected();

    qDebug() << QObject::tr("Disconnected from ") << m_pSerialPort->portName();
  }

  // Установка нового имени порта
  m_pSerialPort->setPort(strPortName);

  // Подключение нового порта, при возможности
  if((0 <= m_listPortsNames.indexOf(m_pSerialPort->portName())) && !m_pSerialPort->isOpen())
  {
    m_pSerialPort->open(QIODevice::ReadWrite);
    emit signalConnected();

    qDebug() << QObject::tr("Connected to ") << m_pSerialPort->portName() << m_pSerialPort->isOpen();
  }
}

//==============================================================================
