#include "connectioncontrol_ui.h"
#include "ui_connectioncontrol_ui.h"

ConnectionControl_UI::ConnectionControl_UI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionControl_UI)
{
    ui->setupUi(this);

    // —окрытие элемнтов относ€щихс€ к TCP (при старте RS-режим)
    ui->label_TCPServer->hide();
    ui->lineEdit_TCPHostAddress->hide();
    ui->groupBox_TCPDeviceAvailable->hide();
    ui->comboBox_TCPDevices->hide();
}

ConnectionControl_UI::~ConnectionControl_UI()
{
    delete ui;
}

void ConnectionControl_UI::slotSetRSPortList(QList<QString> listRSPorts, QString strSelected)
{
    ui->comboBox_RSPorts->clear();
    ui->comboBox_RSPorts->addItem(tr("None"));
    for (int i=0; listRSPorts.length() > i; i++) {

      ui->comboBox_RSPorts->addItem(listRSPorts.at(i));
      }

    // ¬осстановление выбора в списке текущего активного порта
    int iIndex = ui->comboBox_RSPorts->findText(strSelected);
    ui->comboBox_RSPorts->setCurrentIndex(iIndex);
}

void ConnectionControl_UI::slotSetNetworkDevicesList(QList<QString> listDevicesID, QString strSelected)
{
    ui->comboBox_TCPDevices->clear();
    ui->comboBox_TCPDevices->addItem(tr("None"));
    for (int i=0; listDevicesID.length() > i; i++) {

      ui->comboBox_TCPDevices->addItem(listDevicesID.at(i));
      }

    // ¬осстановление выбора в списке текущего активного порта
    int iIndex = ui->comboBox_TCPDevices->findText(strSelected);
    ui->comboBox_TCPDevices->setCurrentIndex(iIndex);
}

void ConnectionControl_UI::slotSetRSPortActive(QString strPortName)
{
    int iIndex = -1;
    if("None" != strPortName)
        iIndex = ui->comboBox_RSPorts->findText(strPortName);

    ui->comboBox_RSPorts->setCurrentIndex(iIndex);
}

void ConnectionControl_UI::slotSetNetworkDeviceActive(QString strDeviceID)
{
    int iIndex = -1;
    if("None" != strDeviceID)
        iIndex = ui->comboBox_TCPDevices->findText(strDeviceID);

    ui->comboBox_TCPDevices->setCurrentIndex(iIndex);
}

void ConnectionControl_UI::on_comboBox_RSPorts_activated(const QString &arg1)
{
    emit signalSetRSPort(arg1);
}

void ConnectionControl_UI::on_radioButton_RS_toggled(bool checked)
{
    if(checked)
        emit signalSetConnectionTypeUSB();
    else
        emit signalSetConnectionTypeTCP();
}

void ConnectionControl_UI::on_lineEdit_TCPHostAddress_returnPressed()
{
    emit signalSetServerName(ui->lineEdit_TCPHostAddress->text());
}

void ConnectionControl_UI::on_comboBox_TCPDevices_currentIndexChanged(const QString &arg1)
{
    if("None" == arg1)
        ui->comboBox_TCPDevices->setCurrentIndex(-1);   // ќчистка пол€ выбранного устройства, если это "None"

    emit signalSetNetworkDeviceID(arg1);
}
