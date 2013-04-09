/* USB_Communicator.h - ������������ ���� ��� ������� ���������� ������������� ��� USB �����
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
  enum{ // ����������� �������������� ��������� ��������� ������
    eDevID_Unknown  = 0x0000,
    eDevID_Ethernet = 0xffe0,
    eDevID_Usb      = 0xfff0,
    eDevID_Usb01    = 0xfff1,
    eDevID_Dummy    = 0xfffe, // ������ ID (������ �� �����������)
    eDevID_Broadcast= 0xffff
  };

public:
  static CUSB_Communicator* GetInstance(unsigned short usDeviceID_Usb = eDevID_Usb);

  QList<QString> availablePorts();

private:
  CUSB_Communicator(unsigned short usDeviceID_Usb);
  CUSB_Communicator(const CUSB_Communicator &); // hide copy constructor
  CUSB_Communicator& operator=(const CUSB_Communicator &); // hide assign op

  /* ������ �������������� � ��������������� �� ICommunicator */
public slots:
  void slotSetPortName(const QString &strPortName);  // ��������� ���� ��� ������, ��������� - ������� �� ���������� ����

signals:
  void signalPortListUpdated(const QList<QString> &portList, QString strSelected);
  void signalPortSelected(QString strPortName);
  void signalConnected();   // ����������� ���������� � ����������� ������
  void signalDisonnected(); // ��������� ���������� � ����������� ������

private slots:
  void stotUpdate();  // ���� ��� ������� ���������� ���������� � ������ � �������

protected:
  const unsigned short m_usDeviceID;

private:
  static CUSB_Communicator* st_pThis;

  QTimer m_timerUpdate;
  QList<QString> m_listPortsNames;  // ������ ��������� � ������� ������

  SerialPort  *m_pSerialPort;       // ������� ����������������� ���������� USB
};
//==============================================================================
#endif //_USB_COMMUNICATOR_H_

