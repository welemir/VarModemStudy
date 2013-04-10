#ifndef DIALOGABOUTPROGRAM_H
#define DIALOGABOUTPROGRAM_H

#include <QDialog>

namespace Ui {
class DialogAboutProgram;
}

class DialogAboutProgram : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogAboutProgram(QWidget *parent = 0);
    ~DialogAboutProgram();
    
private:
    Ui::DialogAboutProgram *ui;
};

#endif // DIALOGABOUTPROGRAM_H
