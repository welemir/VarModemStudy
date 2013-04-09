#ifndef ICOMMUNICATOR_H
#define ICOMMUNICATOR_H

#include "CommunicationStructs.h"

#include <QObject>
#include <QIODevice>

class CCommunicator: public QObject{
    Q_OBJECT
public:
    CCommunicator(QObject *parent=0);
    /* ������ �������������� � ��������������� */
signals:
    void ReceivePacket(TPacket packet);
public slots:
    virtual int SendPacket(TPacket packet);
    virtual void slotReadyRead();

protected:
    // ��������� ������ ������� ������ ��� ���������� �������
    virtual void Parcer(unsigned char ucByteNew);

protected:
    QIODevice *m_pIODevice;
};

#endif // ICOMMUNICATOR_H
