#include <QFileDialog>
#include <QMessageBox>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "programsettings.h"
#include "connectioncontrol.h"
#include "cstatuswidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    bool kernelInPrivateThreadEnabled = false;
    ui->setupUi(this);

    // создание окон
    m_aboutProgramWindow = new DialogAboutProgram(this);
//    m_settingsWindow     = new DialogSettings(this);
    m_diagnosticsWindow  = new DialogDiagnostics(this);

    m_statusWidget = new CStatusWidget();

    ui->statusBar->insertPermanentWidget(1 , m_statusWidget, 20);

    // подключение пунктов главного меню
    QObject::connect(ui->actionShowAboutWindow, SIGNAL(triggered()), this, SLOT(showAboutWindow()));
    //QObject::connect(ui->actionShowSettings,    SIGNAL(triggered()), this, SLOT(showSettingsWindow()));
    QObject::connect(ui->actionShowDiag,        SIGNAL(triggered()), this, SLOT(showDiagWindow()));

    m_pKernel = CKernel::GetInstance();

    QThread *kernelThread = NULL;
    if (kernelInPrivateThreadEnabled)
    {
        kernelThread = new QThread(this);

        connect(kernelThread, SIGNAL(finished()), m_pKernel, SLOT(deleteLater()));
        m_pKernel->moveToThread(kernelThread);
    }

    connectKernelToUI();

    QObject::connect(m_pKernel, SIGNAL(signalPrintDiagMeaasge(QString)), m_diagnosticsWindow, SLOT(slotPrintDiagMeaasge(QString)));

    QObject::connect(m_pKernel, SIGNAL(signalTxStateUpdated(bool)), m_statusWidget, SLOT(slotSetTxStatus(bool)) );
    QObject::connect(m_pKernel, SIGNAL(signalRxStateUpdated(bool)), m_statusWidget, SLOT(slotSetRxStatus(bool)) );

    connect(m_pKernel, SIGNAL(signalTxInProgress(bool)), this, SLOT(slotSetTxState(bool)));
    connect(m_pKernel, SIGNAL(signalTxProgress(int)), m_statusWidget, SLOT(slotSetTxProgress(int)));
    connect(m_pKernel, SIGNAL(signalRxProgress(int)), m_statusWidget, SLOT(slotSetRxProgress(int)));

    if (kernelInPrivateThreadEnabled)
    {
        kernelThread->start();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotSetTxState(bool isInProgress)
{
    bool isLocked = !isInProgress;
    ui->comboBoxTxSpeed->setEnabled( isLocked );
    ui->comboBoxTxModulation->setEnabled( isLocked );
    ui->comboBoxTxOutputPower->setEnabled( isLocked );
    ui->comboBoxTxBitSynch->setEnabled( isLocked );
    ui->comboBoxTxSynch->setEnabled( isLocked );
    ui->lineEditTxPacketDataLength->setEnabled( isLocked );
    ui->comboBoxTxCRC->setEnabled( isLocked );
    ui->lineEditTxTotalDataLength->setEnabled( isLocked );
}

void MainWindow::connectKernelToUI()
{
    connect(ui->comboBoxTxSpeed, SIGNAL(activated(QString)), this, SLOT(slotSetConnectionSpeed(QString)));
    connect(ui->comboBoxTxOutputPower, SIGNAL(activated(QString)), this, SLOT(slotSetOutputPower(QString)));
    connect(ui->comboBoxTxModulation, SIGNAL(activated(int)), this, SLOT(slotSetModulationType(int)));
    connect(ui->comboBoxTxBitSynch, SIGNAL(activated(QString)), this, SLOT(slotSetPreambleLength(QString)));
    connect(ui->comboBoxTxSynch, SIGNAL(activated(QString)), this, SLOT(slotSetSyncPatternLength(QString)));
    connect(ui->lineEditTxPacketDataLength, SIGNAL(textChanged(QString)), this, SLOT(slotSetDataPacketLength(QString)));
    connect(ui->lineEditTxTotalDataLength, SIGNAL(textChanged(QString)), this, SLOT(slotSetTotalDataLength(QString)));
    connect(ui->pushButtonTxStart, SIGNAL(clicked()), m_pKernel, SLOT(slotStartOperation()));
    connect(ui->pushButtonTxStop, SIGNAL(clicked()), m_pKernel, SLOT(slotStopOperation()));
    connect(ui->comboBoxTxCRC, SIGNAL(currentIndexChanged(int)), m_pKernel, SLOT(slotSetCrcType(int)));

    connect(m_pKernel, SIGNAL(signalNewConnectionSpeed(int)), this, SLOT(slotNewConnectionSpeed(int)));
    connect(m_pKernel, SIGNAL(signalNewOutputPower(int)), this, SLOT(slotNewOutputPower(int)));
    connect(m_pKernel, SIGNAL(signalNewModulationType(int)), this, SLOT(slotNewModulationType(int)));
    connect(m_pKernel, SIGNAL(signalNewTotalDataLength(int)), this, SLOT(slotNewTotalDataLength(int)));
    connect(m_pKernel, SIGNAL(signalShowBER(qreal)), this, SLOT(slotNewBER(qreal)));
    connect(m_pKernel, SIGNAL(signalShowPER(qreal)), this, SLOT(slotNewPER(qreal)));
    connect(m_pKernel, SIGNAL(signalShowChannelUtilizationPayload(qreal)), this, SLOT(slotNewChannelUtilizationPayload(qreal)));
    connect(m_pKernel, SIGNAL(signalShowChannelUtilizationSerivce(qreal)), this, SLOT(slotNewChannelUtilizationSerivce(qreal)));
    connect(m_pKernel, SIGNAL(signalShowRxSpeed(qreal)), this, SLOT(slotNewRxSpeed(qreal)));
    connect(m_pKernel, SIGNAL(signalUpdateStatistics()), this, SLOT(slotUpdateStatistics()));
}

void MainWindow::showAboutWindow()
{
    m_aboutProgramWindow->show();
}

//void MainWindow::showSettingsWindow()
//{
//    m_settingsWindow->show();
//}

void MainWindow::showDiagWindow()
{
    m_diagnosticsWindow->show();
}

void MainWindow::slotSetConnectionSpeed(QString newSpeed)
{
    m_pKernel->slotSetConnectionSpeed(newSpeed.toInt());
}

void MainWindow::slotSetOutputPower(QString newPower)
{
    int iOutputPowerToSet = newPower.toInt();
    iOutputPowerToSet -= 6;
    m_pKernel->slotSetOutputPower(iOutputPowerToSet);
}

void MainWindow::slotSetModulationType(int newModIndex)
{
    m_pKernel->slotSetModulationType(newModIndex);
}

void MainWindow::slotSetPreambleLength(QString newLength)
{
  m_pKernel->slotSetPreambleLength(newLength.toInt());
}

void MainWindow::slotSetSyncPatternLength(QString newLength)
{
  m_pKernel->slotSetSyncPatternLength(newLength.toInt());
}

void MainWindow::slotSetDataPacketLength(QString newLength)
{
    m_pKernel->slotSetDataPacketLength(newLength.toInt());
}

void MainWindow::slotSetTotalDataLength(QString newLength)
{
    m_pKernel->slotSetTotalDataLength(newLength.toInt());
}

void MainWindow::slotNewConnectionSpeed(int newValue)
{
    ui->comboBoxTxSpeed->setEditText(QString("%1").arg(newValue));
}

void MainWindow::slotNewOutputPower(int newValue)
{
   ui->comboBoxTxOutputPower->setEditText(QString("%1").arg(newValue));
}

void MainWindow::slotNewModulationType(int newValue)
{
    ui->comboBoxTxModulation->setCurrentIndex(newValue);
}

void MainWindow::slotNewTotalDataLength(int newValue)
{
    ui->lineEditTxTotalDataLength->setText(QString("%1").arg(newValue));
}

void MainWindow::slotNewBER(qreal newValue)
{
    QString errorRate;
    errorRate.setNum(newValue, 'g', 5);
    errorRate.append(" %");
    ui->lineEditRxBER->setText(errorRate);
}

void MainWindow::slotNewPER(qreal newValue)
{
    QString errorRate;
    errorRate.setNum(newValue, 'g', 5);
    errorRate.append(" %");
    ui->lineEditRxPER->setText(errorRate);
}

void MainWindow::slotNewChannelUtilizationPayload(qreal newValue)
{
  QString utilization;
  utilization.setNum(newValue, 'g', 4);
  utilization.append(" %");
  ui->lineEditRxSPayloadInfoSize->setText(utilization);
}

void MainWindow::slotNewChannelUtilizationSerivce(qreal newValue)
{
  QString utilization;
  utilization.setNum(newValue, 'g', 4);
  utilization.append(" %");
  ui->lineEditRxServiceInfoSize->setText(utilization);
}

void MainWindow::slotNewRxSpeed(qreal newValue)
{
  ui->lineEditRxSpeed->setText(QString("%1").arg(newValue));
}

void MainWindow::slotUpdateStatistics()
{
  int iBitErrorsDetected = m_pKernel->getBitErrorsDetected();
  ui->lineEditRxDetectedErrorsCount->setText(QString("%1").arg(iBitErrorsDetected));
  int iBitErrorsMissed = m_pKernel->getBitErrorsMissed();
  ui->lineEditRxMissedErrorsCount->setText(QString("%1").arg(iBitErrorsMissed));
}
