#include "CCommunicator.h"

#include <QDebug>

CCommunicator::CCommunicator(QObject *parent/*=0*/):
    QObject(parent),
    m_pIODevice(0)
{
}

void CCommunicator::slotReadyRead()
{
    if(0 == m_pIODevice)
        return;

    QByteArray data = m_pIODevice->readAll();
    for(int i=0;i<data.length();i++)
      Parcer(data[i]);
}

int CCommunicator::SendPacket(TPacket packet)
{
  if(0 == m_pIODevice)
      return -1;   // ��������� �������� �� ������������������

  QByteArray baDataOut;
  baDataOut.append(BITBUS_MARK);  // ������ ������ ������
  // ����������� ���� ������ (��������� � ���� �����)
  baDataOut.append((char*)&packet, 1 + packet.ucLength);
  baDataOut.append(packet.ucCRC);
  baDataOut.append(BITBUS_MARK);  // ������ ���������� ������

  // �������� ����������� ��������� � �������� ������
  QString strData;
  for( int i=0;i<=(packet.ucLength - 6); i++)
   {
     strData.append(QString("%1").arg(packet.ucData[i], 2, 16,QLatin1Char( '0' )));
   }
  qDebug() << endl << "Sended packet: --> " << strData;

  // �������� ������ (����� ��������� � ������ + ���� CRC � ����� + �������)
  return m_pIODevice->write(baDataOut);
}

void CCommunicator::Parcer(unsigned char ucByteNew)
{
  static enum
  {
    eStateStartMark,
    eStateLength,
    eStateData,
    eStateCRC,
    eStateEndMark
  } parceState = eStateStartMark;

  static TPacket tmsgInput;
  static int iDataRemain;
  static unsigned char *pDataStore;

  switch(parceState)
  {
  case eStateStartMark:
    if(BITBUS_MARK == ucByteNew)
      parceState = eStateLength;
    break;

  case eStateLength:
    if((BITBUS_MARK != ucByteNew)&&         // ��� ��������� ��������� �������������
       ((sizeof(TPacket)-2) >= ucByteNew))  // ����������� ������������ ����� ������
    { // ��������� ����� ������������
      iDataRemain = ucByteNew;
      tmsgInput.ucLength = ucByteNew;
      pDataStore = &tmsgInput.ucPipeAddress;
      parceState = eStateData;
    }
    break;

  case eStateData:
    *pDataStore++ = ucByteNew;
    if(0 >= --iDataRemain)
      parceState = eStateCRC;
    break;

  case eStateCRC:
    // ���� ����������� ����� ���� ��� ������ �� USB �� ����������� � �� �������������,
    // ������ �������������� � �������� ������
    *pDataStore++ = ucByteNew;
    parceState = eStateEndMark;
    break;

  case eStateEndMark:
    if(BITBUS_MARK == ucByteNew)
    {
      { // �������� ����������� ��������� � �������� ������
        QString strData;
        for( int i=0;i<=(tmsgInput.ucLength - 6); i++)
          {
            strData.append(QString("%1").arg(tmsgInput.ucData[i], 2, 16,QLatin1Char( '0' )));
          }
        qDebug() << endl << "Received packet: <-- " << strData;
      }

      // �������� ������� � �������� ������ ���� �����������
      emit ReceivePacket(tmsgInput);
    }
    else
        qDebug() << endl << "Invalid packet";

    parceState = eStateStartMark; // ������� � �������� ���������� ������
    break;

  default:
    parceState = eStateStartMark;
    break;
  }
}
