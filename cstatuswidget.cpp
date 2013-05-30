#include "cstatuswidget.h"
#include "ui_cstatuswidget.h"

CStatusWidget::CStatusWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CStatusWidget)
{
    ui->setupUi(this);
}

CStatusWidget::~CStatusWidget()
{
    delete ui;
}

void CStatusWidget::slotSetTxStatus(bool isOn)
{
    if (isOn)
    {
        ui->label_Tx_Status->setText("Tx:On");
        ui->label_Tx_Status->setEnabled(true);
    }
    else
    {
        ui->label_Tx_Status->setText("Tx:Off");
        ui->label_Tx_Status->setEnabled(false);
    }
}

void CStatusWidget::slotSetRxStatus(bool isOn)
{
    if (isOn)
    {
        ui->label_Rx_Status->setText("Rx:On");
        ui->label_Rx_Status->setEnabled(true);
    }
    else
    {
        ui->label_Rx_Status->setText("Rx:Off");
        ui->label_Rx_Status->setEnabled(false);
    }
}

void CStatusWidget::slotSetTxProgress(int progress)
{
    ui->progressBar_Tx_Status->setValue(progress);
}

void CStatusWidget::slotSetRxProgress(int progress)
{
    ui->progressBar_Rx_Status->setValue(progress);
}
