#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

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
};

#endif // MAINWINDOW_H
