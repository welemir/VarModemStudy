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

void DialogDiagnostics::slotPrintDiagMeaasge(QString message)
{
    ui->textBrowser->append(message);
}
