/* USB_Communicator.h - ������������ ���� ��� ������� ���������� ������������� ��� USB �����
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
  enum{ // ����������� �������������� ��������� ��������� ������
    eDevID_Unknown  = 0x0000,
    eDevID_Ethernet = 0xffe0,
    eDevID_Usb      = 0xfff0,
    eDevID_Usb01    = 0xfff1,
    eDevID_Dummy    = 0xfffe, // ������ ID (������ �� �����������)
    eDevID_Broadcast= 0xffff
  };

public:
  CUSB_Communicator(unsigned short usDeviceID_Usb);
  ~CUSB_Communicator();
private:
  /* ������ �������������� � ��������������� �� ICommunicator */
public slots:
  void slotSetPortName(const QString &strPortName);  // ��������� ���� ��� ������, ��������� - ������� �� ���������� ����
public:
  QString getPortName();
signals:
  void signalPortListUpdated(const QList<QString> &portList, QString strSelected);
  void signalPortSelected(QString strPortName);
  void signalConnected();   // ����������� ���������� � ����������� ������
  void signalDisonnected(); // ��������� ���������� � ����������� ������

protected:
  const unsigned short m_usDeviceID;

private:
  static CUSB_Communicator* st_pThis;
  SerialPort  *m_pSerialPort;       // ������� ����������������� ���������� USB

};
//==============================================================================
#endif //_USB_COMMUNICATOR_H_

