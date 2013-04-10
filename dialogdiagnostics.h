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
    
private:
    Ui::DialogDiagnostics *ui;
};

#endif // DIALOGDIAGNOSTICS_H
