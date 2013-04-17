/* USB_Communicator.h - Заголовочный файл для классов интерфейса коммуникатора для USB порта
*/

#ifndef _USB_COMMUNICATOR_H_
#define _USB_COMMUNICATOR_H_

#include <QObject>


#include "CCommunicator.h"
#include "serialport.h"

QT_USE_NAMESPACE_SERIALPORT

/******************************************************************************/
class CUSB_Communicator: public CCommunicator
{
    Q_OBJECT
    friend class CConnectionControl;

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
  CUSB_Communicator(unsigned short usDeviceID_Usb);
  ~CUSB_Communicator();
private:
  /* Методы взаимодействия с маршрутизатором из ICommunicator */
public slots:
  void slotSetPortName(const QString &strPortName);  // Назначить порт для работы, результат - удалось ли подключить порт
public:
  QString getPortName();
signals:
  void signalPortListUpdated(const QList<QString> &portList, QString strSelected);
  void signalPortSelected(QString strPortName);
  void signalConnected();   // Установлено соединение с назначенным портом
  void signalDisonnected(); // Разорвано соединение с назначенным портом

protected:
  const unsigned short m_usDeviceID;

private:
  static CUSB_Communicator* st_pThis;
  SerialPort  *m_pSerialPort;       // Драйвер последовательного устройства USB

};
//==============================================================================
#endif //_USB_COMMUNICATOR_H_

