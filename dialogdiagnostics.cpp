#include "dialogdiagnostics.h"
#include "ui_dialogdiagnostics.h"

DialogDiagnostics::DialogDiagnostics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDiagnostics)
{
    ui->setupUi(this);
}

DialogDiagnostics::~DialogDiagnostics()
{
    delete ui;
}
