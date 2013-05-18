#include <QFileDialog>
#include <QMessageBox>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "programsettings.h"
#include "connectioncontrol.h"


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

    m_StatusBar_TxLabel = new QLabel(this);
    m_StatusBar_RxLabel = new QLabel(this);
    m_StatusBar_TxPogressBar = new QProgressBar(this);

    slotSetTxStatus(false);
    slotSetRxStatus(false);

    ui->statusBar->insertPermanentWidget(0 ,m_StatusBar_TxLabel, 1);
    ui->statusBar->insertPermanentWidget(2, m_StatusBar_RxLabel, 2);
    ui->statusBar->insertPermanentWidget(4, m_StatusBar_TxPogressBar, 20);

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
    QObject::connect(this, SIGNAL(signalRunCommandFromUI(const CUICommand )), m_pKernel, SLOT(slotRunCommandFromUI(const CUICommand )));

    QObject::connect(m_pKernel, SIGNAL(signalPrintDiagMeaasge(QString)), m_diagnosticsWindow, SLOT(slotPrintDiagMeaasge(QString)));

    QObject::connect(m_pKernel, SIGNAL(signalTxStateUpdated(bool)), this, SLOT(slotSetTxStatus(bool)) );
    QObject::connect(m_pKernel, SIGNAL(signalRxStateUpdated(bool)), this, SLOT(slotSetRxStatus(bool)) );

    connect(m_pKernel, SIGNAL(signalTxInProgress(bool)), this, SLOT(slotSetTxState(bool)));
    connect(m_pKernel, SIGNAL(signalTxProgress(int)), m_StatusBar_TxPogressBar, SLOT(setValue(int)));

    if (kernelInPrivateThreadEnabled)
    {
        kernelThread->start();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotSetTxStatus(bool isOn)
{
    QString newMessage;
    QString newStyleSheet;
    if (isOn)
    {
        newMessage = "Tx:On";
        newStyleSheet   = "QLabel { color : black; }";
    }
    else
    {
        newMessage = "Tx:Off";
        newStyleSheet   = "QLabel { color : gray; }";
    }

    m_StatusBar_TxLabel->setStyleSheet(newStyleSheet);
    m_StatusBar_TxLabel->setText(newMessage);
}

void MainWindow::slotSetRxStatus(bool isOn)
{
    QString newMessage;
    QString newStyleSheet;
    if (isOn)
    {
        newMessage = "Rx:On";
        newStyleSheet   = "QLabel { color : black; }";
    }
    else
    {
        newMessage = "Rx:Off";
        newStyleSheet   = "QLabel { color : gray; }";
    }

    m_StatusBar_RxLabel->setStyleSheet(newStyleSheet);
    m_StatusBar_RxLabel->setText(newMessage);
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
    connect(ui->comboBoxTxBitSynch, SIGNAL(activated(QString)), this, SLOT(slotSetBitSynchLength(QString)));
    connect(ui->comboBoxTxSynch, SIGNAL(activated(QString)), this, SLOT(slotSetSychnroSequenceLength(QString)));
    connect(ui->lineEditTxPacketDataLength, SIGNAL(textChanged(QString)), this, SLOT(slotSetDataPacketLength(QString)));
    connect(ui->lineEditTxTotalDataLength, SIGNAL(textChanged(QString)), this, SLOT(slotSetTotalDataLength(QString)));
    connect(ui->pushButtonTxStart, SIGNAL(clicked()), m_pKernel, SLOT(slotStartOperation()));
    connect(ui->pushButtonTxStop, SIGNAL(clicked()), m_pKernel, SLOT(slotStopOperation()));

    connect(m_pKernel, SIGNAL(signalNewConnectionSpeed(int)), this, SLOT(slotNewConnectionSpeed(int)));
    connect(m_pKernel, SIGNAL(signalNewOutputPower(int)), this, SLOT(slotNewOutputPower(int)));
    connect(m_pKernel, SIGNAL(signalNewModulationType(int)), this, SLOT(slotNewModulationType(int)));
    connect(m_pKernel, SIGNAL(signalNewBitSynchLength(int)), this, SLOT(slotNewBitSynchLength(int)));
    connect(m_pKernel, SIGNAL(signalNewSychnroSequenceLength(int)), this, SLOT(slotNewSychnroSequenceLength(int)));
    connect(m_pKernel, SIGNAL(signalNewDataPacketLength(int)), this, SLOT(slotNewDataPacketLength(int)));
    connect(m_pKernel, SIGNAL(signalNewTotalDataLength(int)), this, SLOT(slotNewTotalDataLength(int)));
    connect(m_pKernel, SIGNAL(signalShowBER(qreal)), this, SLOT(slotNewBER(qreal)));
    connect(m_pKernel, SIGNAL(signalShowPER(qreal)), this, SLOT(slotNewPER(qreal)));
    connect(m_pKernel, SIGNAL(signalShowChannelUtilizationPayload(int)), this, SLOT(slotNewChannelUtilizationPayload(int)));
    connect(m_pKernel, SIGNAL(signalShowChannelUtilizationSerivce(int)), this, SLOT(slotNewChannelUtilizationSerivce(int)));
    connect(m_pKernel, SIGNAL(signalShowRxSpeed(qreal)), this, SLOT(slotNewRxSpeed(qreal)));
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
    m_pKernel->slotSetOutputPower(newPower.toInt());
}

void MainWindow::slotSetModulationType(int newModIndex)
{
    m_pKernel->slotSetModulationType(newModIndex);
}

void MainWindow::slotSetBitSynchLength(QString newLength)
{
    m_pKernel->slotSetBitSynchLength(newLength.toInt());
}

void MainWindow::slotSetSychnroSequenceLength(QString newLength)
{
    m_pKernel->slotSetSychnroSequenceLength(newLength.toInt());
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

void MainWindow::slotNewBitSynchLength(int newValue)
{
    ui->comboBoxTxBitSynch->setEditText(QString("%1").arg(newValue));
}

void MainWindow::slotNewSychnroSequenceLength(int newValue)
{
    ui->comboBoxTxSynch->setEditText(QString("%1").arg(newValue));
}

void MainWindow::slotNewDataPacketLength(int newValue)
{
    ui->lineEditTxPacketDataLength->setText(QString("%1").arg(newValue));
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
    errorRate.setNum(newValue, 'g', 2);
    errorRate.append(" %");
    ui->lineEditRxPER->setText(errorRate);
}

void MainWindow::slotNewChannelUtilizationPayload(int newValue)
{
    ui->lineEditRxSPayloadInfoSize->setText(QString("%1").arg(newValue));
}

void MainWindow::slotNewChannelUtilizationSerivce(int newValue)
{
    ui->lineEditRxServiceInfoSize->setText(QString("%1").arg(newValue));
}

void MainWindow::slotNewRxSpeed(qreal newValue)
{
    ui->lineEditRxSpeed->setText(QString("%1").arg(newValue));
}
