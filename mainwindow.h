#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QtGui>

#include "ckernel.h"

#include "dialogaboutprogram.h"
//#include "dialogsettings.h"
#include "dialogdiagnostics.h"
#include "cstatuswidget.h"

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
    void slotSetTxState( bool isInProgress);


private slots:
    void showAboutWindow();
//    void showSettingsWindow();
    void showDiagWindow();

    void slotSetConnectionSpeed(QString newSpeed);
    void slotSetOutputPower(QString newPower);
    void slotSetModulationType(int newModIndex);
    void slotSetPreambleLength(QString newLength);
    void slotSetSyncPatternLength(QString newLength);
    void slotSetDataPacketLength(QString newLength);
    void slotSetTotalDataLength(QString newLength);

    void slotNewConnectionSpeed(int);
    void slotNewOutputPower(int);
    void slotNewModulationType(int);
    void slotNewTotalDataLength(int);
    void slotNewBER(qreal);
    void slotNewPER(qreal);
    void slotNewChannelUtilizationPayload(qreal);
    void slotNewChannelUtilizationSerivce(qreal);
    void slotNewRxSpeed(qreal);
    void slotUpdateStatistics();  // Запрос на обновление статистики обмена

    void on_actionSwitchPlaybackMode_toggled(bool arg1);

private:
    void connectKernelToUI();

private:
    Ui::MainWindow *ui;
    CKernel* m_pKernel;
    DialogAboutProgram *m_aboutProgramWindow;
//    DialogSettings *m_settingsWindow;
    DialogDiagnostics *m_diagnosticsWindow;
    CStatusWidget *m_statusWidget;
};

#endif // MAINWINDOW_H
