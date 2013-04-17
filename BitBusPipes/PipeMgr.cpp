/* PipeMgr.cpp - Реализация класса организации каналов обмена данными
                 в протоколе BitBusExt
*/

#include <string.h>
#include <QMutex>

#include "PipeMgr.h"

CPipeMgr* CPipeMgr::st_pThis = 0;

//==============================================================================
CPipeMgr::CPipeMgr(unsigned short usDeviceId):
  m_usDeviceID(usDeviceId)
{
  for(int i=0;i<MAX_PIPES;i++)
  {
    m_arrPipes[i] = 0;
  };
}

//==============================================================================
CPipe* CPipeMgr::CreatePipe(TPipeType PipeAddress, TPipeMode mode/*=eTwoDirections*/)
{ // Проверяем адрес пайпа
  if(MAX_PIPES <= PipeAddress)
    return 0;

  // Проверяем режим пайпа
  if(eLastMode < mode)
    return 0;

  // Проверяем наличие пайпа с данным адресом
  if(!m_arrPipes[PipeAddress])
  { // Создаем пайп ThreadSafe способом
    static QMutex mutex;
    mutex.lock();

    if(!m_arrPipes[PipeAddress])
      m_arrPipes[PipeAddress] = new CPipe(PipeAddress, mode, this);

    mutex.unlock();
  }

  return m_arrPipes[PipeAddress];
}

//==============================================================================
void CPipeMgr::ReceiveNewData(TPacket packet)
{
  emit ReceivePacket(packet);
}

//==============================================================================
int CPipeMgr::SendPacket(TPacket packet)
{
  // Распределение данных пакета в пайп
  int iRet = CPipe::eInvalidPointer;
  if(MAX_PIPES > packet.ucPipeAddress)
  {
    if(0 != m_arrPipes[packet.ucPipeAddress])
    {
      QByteArray byteArray((const char*)packet.ucData, packet.ucLength - packet.ServiceHeaderSize());
      m_arrPipes[packet.ucPipeAddress]->SendNewData(byteArray, packet.usID_Sender);
      iRet = CPipe::eOk;
    }
  }
  return iRet;
}

/*******************************************************************************
  Описание класса CPipe
*******************************************************************************/

//==============================================================================
CPipe::CPipe(CPipeMgr::TPipeType PipeIndex, CPipeMgr::TPipeMode mode, CPipeMgr* pipeManager):
  m_PipeIndex(PipeIndex),
  m_pipeManager(pipeManager)
{

  if(CPipeMgr::eReadOnly != mode)
  { // Очередь пакетов на отправку
//    m_qInputPackets = xQueueCreate(iMaxInPackets, uiPipePacketSize);
  }

  if(CPipeMgr::eWriteOnly != mode)
  { // Очередь пришедших от роутера пакетов
//    m_qOutputPackets = xQueueCreate(iMaxOutPackets, uiPipePacketSize);
  }
}

//==============================================================================
CPipe::~CPipe()
{
}

//==============================================================================
void CPipe::ReleasePipe()
{ // Удаляем пайп
//  CPipeMgr::m_arrPipes[m_PipeIndex] = NULL;
//  this->~CPipe();
}

//==============================================================================
void CPipe::SendNewData(QByteArray baData, unsigned short usSenderID)
{
  emit ReadData(baData, usSenderID);
}

//==============================================================================
void CPipe::WriteData(QByteArray baData, unsigned short usRecipientID)
{

  /* Заполням служебные поля пакета */
  TPacket packetWork;
  packetWork.ucLength = baData.length() + TPacket::ServiceHeaderSize();
  packetWork.usID_Recipient = usRecipientID;
  packetWork.usID_Sender = m_pipeManager->m_usDeviceID;
  packetWork.ucPipeAddress = m_PipeIndex;
  memcpy(packetWork.ucData, baData.constData(), baData.length());

  // Отправляем пакет в очередь
  m_pipeManager->ReceiveNewData(packetWork);
}

//==============================================================================
