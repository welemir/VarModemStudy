/* PipeMgr.cpp - ���������� ������ ����������� ������� ������ �������
                 � ��������� BitBusExt
*/

#include <string.h>
#include <QMutex>

#include "PipeMgr.h"

CPipeMgr* CPipeMgr::st_pThis = 0;

//==============================================================================
CPipeMgr* CPipeMgr::GetInstance(unsigned short usDeviceId)
{
  if(st_pThis)
    return st_pThis;

  static QMutex mutex;

  mutex.lock();
  if (!st_pThis)
      st_pThis = new CPipeMgr(usDeviceId);

  mutex.unlock();

  return st_pThis;
}

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
{ // ��������� ����� �����
  if(MAX_PIPES <= PipeAddress)
    return 0;

  // ��������� ����� �����
  if(eLastMode < mode)
    return 0;

  // ��������� ������� ����� � ������ �������
  if(!m_arrPipes[PipeAddress])
  { // ������� ���� ThreadSafe ��������
    static QMutex mutex;
    mutex.lock();

    if(!m_arrPipes[PipeAddress])
      m_arrPipes[PipeAddress] = new CPipe(PipeAddress, mode);

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
  // ������������� ������ ������ � ����
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
  �������� ������ CPipe
*******************************************************************************/

//==============================================================================
CPipe::CPipe(CPipeMgr::TPipeType PipeIndex, CPipeMgr::TPipeMode mode):
  m_PipeIndex(PipeIndex)
{

  if(CPipeMgr::eReadOnly != mode)
  { // ������� ������� �� ��������
//    m_qInputPackets = xQueueCreate(iMaxInPackets, uiPipePacketSize);
  }

  if(CPipeMgr::eWriteOnly != mode)
  { // ������� ��������� �� ������� �������
//    m_qOutputPackets = xQueueCreate(iMaxOutPackets, uiPipePacketSize);
  }
}

//==============================================================================
CPipe::~CPipe()
{
}

//==============================================================================
void CPipe::ReleasePipe()
{ // ������� ����
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
  CPipeMgr *pipeManager = CPipeMgr::GetInstance();

  /* �������� ��������� ���� ������ */
  TPacket packetWork;
  packetWork.ucLength = baData.length() + TPacket::ServiceHeaderSize();
  packetWork.usID_Recipient = usRecipientID;
  packetWork.usID_Sender = pipeManager->m_usDeviceID;
  packetWork.ucPipeAddress = m_PipeIndex;
  memcpy(packetWork.ucData, baData.constData(), baData.length());

  // ���������� ����� � �������
  pipeManager->ReceiveNewData(packetWork);
}

//==============================================================================
