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

    // подключение пунктов главного меню
    QObject::connect(ui->actionShowAboutWindow, SIGNAL(triggered()), this, SLOT(showAboutWindow()));
    QObject::connect(ui->actionShowSettings,    SIGNAL(triggered()), this, SLOT(showSettingsWindow()));
    QObject::connect(ui->actionShowDiag,        SIGNAL(triggered()), this, SLOT(showDiagWindow()));


    m_pKernel = CKernel::GetInstance();
    QObject::connect(this, SIGNAL(signalRunCommandFromUI(const CUICommand )), m_pKernel, SLOT(slotRunCommandFromUI(const CUICommand )));
    QObject::connect(m_pKernel, SIGNAL(signalNewMessageToUI(const CUICommand)), this, SLOT(slotNewMessageToUI(const CUICommand)));

//    CConnectionControl::GetInstance(this)->attachUI(ui->widget_ConnectionControlUI);
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

