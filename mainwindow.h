#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QtGui>

#include "ckernel.h"

#include "dialogaboutprogram.h"
//#include "dialogsettings.h"
#include "dialogdiagnostics.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void slotNewMessageToUI(const CUICommand UIcommand);
    void slotSetTxStatus( bool isOn);
    void slotSetRxStatus( bool isOn);
    void slotSetTxState( bool isInProgress);

    void slotConnectKernelToUI( QObject* kernel );

signals:
    void signalRunCommandFromUI(const CUICommand UIcommand);

private slots:
    void showAboutWindow();
//    void showSettingsWindow();
    void showDiagWindow();

private:
    Ui::MainWindow *ui;
    CKernel* m_pKernel;
    DialogAboutProgram *m_aboutProgramWindow;
//    DialogSettings *m_settingsWindow;
    DialogDiagnostics *m_diagnosticsWindow;
    QLabel *m_StatusBar_TxLabel;
    QLabel *m_StatusBar_RxLabel;
    QProgressBar *m_StatusBar_TxPogressBar;
};

#endif // MAINWINDOW_H
