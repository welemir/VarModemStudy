/* ������ ���������� � ������ � �������
*/
#ifndef _PIPE_MGR_H_
#define _PIPE_MGR_H_

#include <QObject>
#include <QByteArray>

#include "CommunicationStructs.h"

#define MAX_PIPES     4

/*************************************************************************************
 ����� ���������� ������� � ������ �������� � ���������������
*************************************************************************************/
class CPipe;
class CPipeMgr: public QObject
{
    Q_OBJECT

public:
    typedef enum {
      ePipeOfCommand = 0,
      ePipeOfSound,
      ePipeOfDiagnostic,
      ePipeOfUpdating
    }TPipeType;

    typedef enum {
      eTwoDirections = 0,
      eReadOnly,
      eWriteOnly,
      eLastMode = eWriteOnly,
    }TPipeMode;

  friend class CPipe;

public:
  static  CPipeMgr* GetInstance(){return st_pThis;};
  static  CPipeMgr* GetInstance(unsigned short usDeviceId); // ����� ��������� Id ������� ��� ���������� ���������������� ���� ������

  /* ������ ���������� ������� */
  CPipe* CreatePipe(TPipeType PipeAddress, TPipeMode mode=eTwoDirections);

  /* ������ �������������� � ��������������� */
signals:
  int ReceivePacket(TPacket packet);
public slots:
  virtual int SendPacket(TPacket packet);
  virtual void ReceiveNewData(TPacket packet);
private:
  CPipeMgr(unsigned short usDeviceId);
  CPipeMgr(const CPipeMgr &); // hide copy constructor
  CPipeMgr& operator=(const CPipeMgr &); // hide assign op

private:
  static CPipeMgr* st_pThis;

  const unsigned short m_usDeviceID;
  CPipe* m_arrPipes[MAX_PIPES];
};

/*************************************************************************************
 ����� ����� "�� ���������"
*************************************************************************************/
class CPipe: public QObject
{
  Q_OBJECT

  friend class CPipeMgr;
public:
  typedef enum{ // ������� ���������� ��������
    eInvalidPointer = -6,
    eInvalidDataLength = -5,
    eNoMemory    = -4,
    eDeviceBusy  = -3,
    eNoData      = -2,
    eError       = -1,
    eOk          = 0,
    eRepeatOp    = 0xff // ��� ������������� ������� ��������
  }TRetCode;

public:
  void ReleasePipe();

signals:
  void ReadData(QByteArray baData, unsigned short usSenderID);
public slots:
  virtual void SendNewData(QByteArray baData, unsigned short usSenderID);
  virtual void WriteData(QByteArray baData, unsigned short usRecipientID);

private:
  CPipe(CPipeMgr::TPipeType PipeIndex, CPipeMgr::TPipeMode mode);
  ~CPipe(void);

private:
  const CPipeMgr::TPipeType m_PipeIndex;
};

#endif //_PIPE_MGR_H_
