/* USB_Communicator.h - Заголовочный файл для классов интерфейса коммуникатора для USB порта
*/

#ifndef _USB_COMMUNICATOR_H_
#define _USB_COMMUNICATOR_H_

#include <QObject>
#include <QTimer>

#include "CCommunicator.h"
#include "serialport.h"

QT_USE_NAMESPACE_SERIALPORT

/******************************************************************************/
class CUSB_Communicator: public CCommunicator
{
    Q_OBJECT

public:
  enum{ // Стандартные идентификаторы устройств протокола обмена
    eDevID_Unknown  = 0x0000,
    eDevID_Ethernet = 0xffe0,
    eDevID_Usb      = 0xfff0,
    eDevID_Usb01    = 0xfff1,
    eDevID_Dummy    = 0xfffe, // Пустой ID (пакеты не принимаются)
    eDevID_Broadcast= 0xffff
  };

public:
  static CUSB_Communicator* GetInstance(unsigned short usDeviceID_Usb = eDevID_Usb);

  QList<QString> availablePorts();

private:
  CUSB_Communicator(unsigned short usDeviceID_Usb);
  CUSB_Communicator(const CUSB_Communicator &); // hide copy constructor
  CUSB_Communicator& operator=(const CUSB_Communicator &); // hide assign op

  /* Методы взаимодействия с маршрутизатором из ICommunicator */
public slots:
  void slotSetPortName(const QString &strPortName);  // Назначить порт для работы, результат - удалось ли подключить порт

signals:
  void signalPortListUpdated(const QList<QString> &portList, QString strSelected);
  void signalPortSelected(QString strPortName);
  void signalConnected();   // Установлено соединение с назначенным портом
  void signalDisonnected(); // Разорвано соединение с назначенным портом

private slots:
  void stotUpdate();  // Слот для таймера обновления информации о портах в системе

protected:
  const unsigned short m_usDeviceID;

private:
  static CUSB_Communicator* st_pThis;

  QTimer m_timerUpdate;
  QList<QString> m_listPortsNames;  // Список доступных в системе портов

  SerialPort  *m_pSerialPort;       // Драйвер последовательного устройства USB
};
//==============================================================================
#endif //_USB_COMMUNICATOR_H_

