#ifndef CBITBUSPIPES_H
#define CBITBUSPIPES_H

#include <QObject>
#include <QMutex>
#include <QTimer>

#include "serialportinfo.h"

QT_USE_NAMESPACE_SERIALPORT

class CBitBusPipes : public QObject
{
    Q_OBJECT
public:

    typedef enum
    {
      ePipeOfCommand = 0,
      ePipeOfSound,
      ePipeOfDiagnostics,
      ePipeOfUpdating
    }TPipeType;

public:
    static CBitBusPipes* GetInstance();
public slots:
    void slotWriteDataToPipe(const QByteArray buff, unsigned short usDestAddr,
                             TPipeType PipeID);
//    void slotConnectSerialPort(SerialPort newSP);
signals:
    //void signalNewData();

private:
    CBitBusPipes() {};
    CBitBusPipes(const CBitBusPipes &);
    CBitBusPipes& operator=(const CBitBusPipes &);

    static CBitBusPipes* m_pInstance;
};

#endif // CBITBUSPIPES_H
