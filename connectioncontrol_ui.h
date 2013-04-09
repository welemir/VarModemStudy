#ifndef CONNECTIONCONTROL_UI_H
#define CONNECTIONCONTROL_UI_H

#include <QWidget>

namespace Ui {
class ConnectionControl_UI;
}

class ConnectionControl_UI : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConnectionControl_UI(QWidget *parent = 0);
    ~ConnectionControl_UI();

signals:
    void signalSetRSPort(QString strPortName);
    void signalSetServerName(QString strServerName);
    void signalSetNetworkDeviceID(QString strDeviceID);
    void signalSetConnectionTypeUSB();
    void signalSetConnectionTypeTCP();

public slots:
    void slotSetRSPortList(QList<QString> listRSPorts, QString strSelected);
    void slotSetNetworkDevicesList(QList<QString> listDevicesID, QString strSelected);
    void slotSetRSPortActive(QString strPortName);
    void slotSetNetworkDeviceActive(QString strDeviceID);
    
private slots:
    void on_comboBox_RSPorts_activated(const QString &arg1);

    void on_radioButton_RS_toggled(bool checked);

    void on_lineEdit_TCPHostAddress_returnPressed();

    void on_comboBox_TCPDevices_currentIndexChanged(const QString &arg1);

private:
    Ui::ConnectionControl_UI *ui;
};

#endif // CONNECTIONCONTROL_UI_H
