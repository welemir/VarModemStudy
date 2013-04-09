#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "ckernel.h"

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
    void on_tableWidgetRoutes_cellActivated(int row, int column);   // —мена активного устройства пользователем
    void on_lineEdiGateway_textChanged(const QString &arg1);

    void on_tableWidgetRoutes_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    CKernel* m_pKernel;
};

#endif // MAINWINDOW_H
