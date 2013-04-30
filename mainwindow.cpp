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
    slotConnectKernelToUI(m_pKernel);
    QObject::connect(this, SIGNAL(signalRunCommandFromUI(const CUICommand )), m_pKernel, SLOT(slotRunCommandFromUI(const CUICommand )));
    QObject::connect(m_pKernel, SIGNAL(signalNewMessageToUI(const CUICommand)), this, SLOT(slotNewMessageToUI(const CUICommand)));
    QObject::connect(m_pKernel, SIGNAL(signalPrintDiagMeaasge(QString)), m_diagnosticsWindow, SLOT(slotPrintDiagMeaasge(QString)));

    QObject::connect(m_pKernel, SIGNAL(signalTxStateUpdated(bool)), this, SLOT(slotSetTxStatus(bool)) );
    QObject::connect(m_pKernel, SIGNAL(signalRxStateUpdated(bool)), this, SLOT(slotSetRxStatus(bool)) );

    connect(m_pKernel, SIGNAL(signalTxInProgress(bool)), this, SLOT(slotSetTxState(bool)));
    connect(m_pKernel, SIGNAL(signalTxProgress(int)), m_StatusBar_TxPogressBar, SLOT(setValue(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotNewMessageToUI(const CUICommand UIcommand)
{

    switch(UIcommand.teUIcommand)
    {

    default:
    {

    }break;
    }// switch

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

void MainWindow::slotConnectKernelToUI(QObject *kernel)
{
    connect(ui->comboBoxTxSpeed, SIGNAL(activated(QString)), kernel, SLOT(slotSetConnectionSpeed(QString)) );
    connect(ui->comboBoxTxOutputPower, SIGNAL(activated(QString)), kernel, SLOT(slotSetOutputPower(QString)) );
    connect(ui->comboBoxTxModulation, SIGNAL(activated(int)), kernel, SLOT(slotSetModulationType(int)) );
    connect(ui->comboBoxTxBitSynch, SIGNAL(activated(QString)), kernel, SLOT(slotSetBitSynchLength(QString)) );
    connect(ui->comboBoxTxSynch, SIGNAL(activated(QString)), kernel, SLOT(slotSetSychnroSequenceLength(QString)) );
    connect(ui->lineEditTxPacketDataLength, SIGNAL(textChanged(QString)), kernel, SLOT(slotSetDataPacketLength(QString)) );
    connect(ui->lineEditTxTotalDataLength, SIGNAL(textChanged(QString)), kernel, SLOT(slotSetTotalDataLength(QString)) );
    connect(ui->pushButtonTxStart, SIGNAL(clicked()), kernel, SLOT(slotStartOperation()) );
    connect(ui->pushButtonTxStop, SIGNAL(clicked()), kernel, SLOT(slotStopOperation()) );

    connect(kernel, SIGNAL(signalNewConnectionSpeed(QString)), ui->comboBoxTxSpeed, SLOT(setEditText(QString)));
    connect(kernel, SIGNAL(signalNewOutputPower( QString )), ui->comboBoxTxOutputPower, SLOT(setEditText(QString)) );
    connect(kernel, SIGNAL(signalNewModulationType( int )), ui->comboBoxTxModulation, SLOT(setCurrentIndex(int)) );
    connect(kernel, SIGNAL(signalNewBitSynchLength( QString )), ui->comboBoxTxBitSynch, SLOT(setEditText(QString)) );
    connect(kernel, SIGNAL(signalNewSychnroSequenceLength( QString )), ui->comboBoxTxSynch, SLOT(setEditText(QString)) );
    connect(kernel, SIGNAL(signalNewDataPacketLength( QString )), ui->lineEditTxPacketDataLength, SLOT(setText(QString)));
    connect(kernel, SIGNAL(signalNewTotalDataLength( QString )), ui->lineEditTxTotalDataLength, SLOT(setText(QString)) );
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

