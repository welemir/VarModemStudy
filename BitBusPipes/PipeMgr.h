/* Классы управления и работы с пайпами
*/
#ifndef _PIPE_MGR_H_
#define _PIPE_MGR_H_

#include <QObject>
#include <QByteArray>

#include "CommunicationStructs.h"

#define MAX_PIPES     6

/*************************************************************************************
 Класс управления пайпами и обмена пакетами с маршрутизатором
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
      ePipeOfUpdating,
      ePipeOfData,
      ePipeOfDataRaw
    }TPipeType;

    typedef enum {
      eTwoDirections = 0,
      eReadOnly,
      eWriteOnly,
      eLastMode = eWriteOnly,
    }TPipeMode;

  friend class CPipe;

public:
  CPipeMgr(unsigned short usDeviceId);
  ~CPipeMgr();

  /* Методы управления пайпами */
  CPipe* CreatePipe(TPipeType PipeAddress, TPipeMode mode=eTwoDirections);

  /* Методы взаимодействия с маршрутизатором */
signals:
  int ReceivePacket(TPacket packet);
public slots:
  virtual int SendPacket(TPacket packet);
  virtual void ReceiveNewData(TPacket packet);
private:
  CPipeMgr(const CPipeMgr &); // hide copy constructor
  CPipeMgr& operator=(const CPipeMgr &); // hide assign op

private:
  static CPipeMgr* st_pThis;

  const unsigned short m_usDeviceID;
  CPipe* m_arrPipes[MAX_PIPES];
};

/*************************************************************************************
 Класс пайпа "по умолчанию"
*************************************************************************************/
class CPipe: public QObject
{
  Q_OBJECT

  friend class CPipeMgr;
public:
  typedef enum{ // Статусы выполнения операций
    eInvalidPointer = -6,
    eInvalidDataLength = -5,
    eNoMemory    = -4,
    eDeviceBusy  = -3,
    eNoData      = -2,
    eError       = -1,
    eOk          = 0,
    eRepeatOp    = 0xff // Код необходимости повтора операции
  }TRetCode;

public:
 // void ReleasePipe();
  ~CPipe(void);
signals:
  void ReadData(QByteArray baData, unsigned short usSenderID);
public slots:
  virtual void SendNewData(QByteArray baData, unsigned short usSenderID);
  virtual void WriteData(QByteArray baData, unsigned short usRecipientID);

private:
  CPipe(CPipeMgr::TPipeType PipeIndex, CPipeMgr::TPipeMode mode, CPipeMgr* pipeManager);

private:
  const CPipeMgr::TPipeType m_PipeIndex;
  CPipeMgr* m_pipeManager;
};

#endif //_PIPE_MGR_H_
