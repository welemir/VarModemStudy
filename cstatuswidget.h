#ifndef CSTATUSWIDGET_H
#define CSTATUSWIDGET_H

#include <QWidget>

namespace Ui {
class CStatusWidget;
}

class CStatusWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CStatusWidget(QWidget *parent = 0);
    ~CStatusWidget();
    
public slots:
    void slotSetTxStatus(bool isOn);
    void slotSetRxStatus(bool isOn);
    void slotSetTxProgress(int progress);
    void slotSetRxProgress(int progress);

private:
    Ui::CStatusWidget *ui;
};

#endif // CSTATUSWIDGET_H
