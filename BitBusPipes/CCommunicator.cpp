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
      return -1;   // Интерфейс передачи не проинициализарован

  QByteArray baDataOut;
  baDataOut.append(BITBUS_MARK);  // Маркер начала пакета
  // Копирование тела пакета (заголовок и поле длины)
  baDataOut.append((char*)&packet, 1 + packet.ucLength);
  baDataOut.append(packet.ucCRC);
  baDataOut.append(BITBUS_MARK);  // Маркер завершения пакета

  // Отправка отладочного сообщения о принятом пакете
  QString strData;
  for( int i=0;i<=(packet.ucLength - 6); i++)
   {
     strData.append(QString("%1").arg(packet.ucData[i], 2, 16,QLatin1Char( '0' )));
   }
  qDebug() << endl << "Sended packet: --> " << strData;

  // Отправка данных (Длина указанная в пакете + поля CRC и длины + маркеры)
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
    if((BITBUS_MARK != ucByteNew)&&         // Для ускорения начальной синхронизации
       ((sizeof(TPacket)-2) >= ucByteNew))  // Ограничение маклимальной длины пакета
    { // Инициация приёма новогопакета
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
    // Поле контрольной суммы пока при обмене по USB не заполняется и не анализируется,
    // просто присоединяется к принятым данным
    *pDataStore++ = ucByteNew;
    parceState = eStateEndMark;
    break;

  case eStateEndMark:
    if(BITBUS_MARK == ucByteNew)
    {
      { // Отправка отладочного сообщения о принятом пакете
        QString strData;
        for( int i=0;i<=(tmsgInput.ucLength - 6); i++)
          {
            strData.append(QString("%1").arg(tmsgInput.ucData[i], 2, 16,QLatin1Char( '0' )));
          }
        qDebug() << endl << "Received packet: <-- " << strData;
      }

      // Отправка сигнала о принятом пакете всем подписчикам
      emit ReceivePacket(tmsgInput);
    }
    else
        qDebug() << endl << "Invalid packet";

    parceState = eStateStartMark; // Переход к ожиданию следующего пакета
    break;

  default:
    parceState = eStateStartMark;
    break;
  }
}
