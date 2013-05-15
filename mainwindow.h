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
    void slotSetTxStatus( bool isOn);
    void slotSetRxStatus( bool isOn);
    void slotSetTxState( bool isInProgress);

    void slotConnectKernelToUI();

private slots:
    void showAboutWindow();
//    void showSettingsWindow();
    void showDiagWindow();

    void slotSetConnectionSpeed(QString newSpeed);
    void slotSetOutputPower(QString newPower);
    void slotSetModulationType(QString newModIndex);
    void slotSetBitSynchLength(QString newLength);
    void slotSetSychnroSequenceLength(QString newLength);
    void slotSetDataPacketLength(QString newLength);
    void slotSetTotalDataLength(QString newLength);

    void slotNewConnectionSpeed(int);
    void slotNewOutputPower(int);
    void slotNewModulationType(int);
    void slotNewBitSynchLength(int);
    void slotNewSychnroSequenceLength(int);
    void slotNewDataPacketLength(int);
    void slotNewTotalDataLength(int);
    void slotNewBER(int);
    void slotNewPER(int);
    void slotNewChannelUtilizationPayload(int);
    void slotNewChannelUtilizationSerivce(int);
    void slotNewRxSpeed(int);

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
