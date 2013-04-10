#include "dialogaboutprogram.h"
#include "ui_dialogaboutprogram.h"

DialogAboutProgram::DialogAboutProgram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAboutProgram)
{
    ui->setupUi(this);
    ui->labelVersion->setText(VER);

}

DialogAboutProgram::~DialogAboutProgram()
{
    delete ui;
}
