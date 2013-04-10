#ifndef CONNECTIONCONTROL_H
#define CONNECTIONCONTROL_H

#include "BitBusPipes/CCommunicator.h"
#include "BitBusPipes/PipeMgr.h"

#include <QObject>

class CConnectionControl : public QObject
{
    Q_OBJECT

public:
    static CConnectionControl* GetInstance(QObject *parent = 0);
public:
    explicit CConnectionControl(QObject *parent = 0);

    // ����������� ���������� ������������ ��� ��������� ������������
    void attachUI(QObject *pUIObject);
    
signals:
 //   void setRSPortList(QList<QString> listRSPorts, QString strSelected);
    void setNetworkDevicesList(QList<QString> listDevicesID, QString strSelected);
//    void setRSPortActive(QString strPortName);
    void setNetworkDeviceActive(QString strDeviceID);
    void resetConnection();

public slots:
    void slotSetNetworkDeviceID(QString strDeviceID);
    void selectCommunicatorRS();
    void selectCommunicatorTCP();
    void slotDisonnected(); // ��������� ���������� � ����������� ������
    void slotParceCommand(QByteArray baData, unsigned short usSenderID);

private:
    void switchCommunicator(CCommunicator *pCommunicatorOld, CCommunicator *pCommunicatorNew);

private:
    static CConnectionControl *st_pThis;
    QObject *m_pUIObject;               // ������ �� ���������������� ��������� ��� ���������� ������������
    QList<QString> m_listDevicesID;     // ������ ��������� ��������� ��� �������������
    CPipe *m_pPipeCmd;
    unsigned short m_usTCPServerID;     // ������������� ������� ��� ������������ ��������� �������
    QString m_strTCPDeviceIMEICurent;   // ������������� ���������� ��� ���������� ����� �������� ������
};

#endif // CONNECTIONCONTROL_H
