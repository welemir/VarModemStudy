#ifndef DIALOGDIAGNOSTICS_H
#define DIALOGDIAGNOSTICS_H

#include <QDialog>

namespace Ui {
class DialogDiagnostics;
}

class DialogDiagnostics : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogDiagnostics(QWidget *parent = 0);
    ~DialogDiagnostics();
public slots:
    void slotPrintDiagMeaasge(QString message);
    
private:
    Ui::DialogDiagnostics *ui;
};

#endif // DIALOGDIAGNOSTICS_H
