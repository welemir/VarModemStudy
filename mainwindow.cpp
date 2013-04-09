#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "BitBusPipes/USB_Communicator.h"
#include "programsettings.h"
#include "connectioncontrol.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pKernel = CKernel::GetInstance();
    QObject::connect(this, SIGNAL(signalRunCommandFromUI(const CUICommand )), m_pKernel, SLOT(slotRunCommandFromUI(const CUICommand )));
    QObject::connect(m_pKernel, SIGNAL(signalNewMessageToUI(const CUICommand)), this, SLOT(slotNewMessageToUI(const CUICommand)));

    CConnectionControl::GetInstance(this)->attachUI(ui->widget_ConnectionControlUI);

    // загрузка таблицы маршрутизации при нажатии на кнопку
    QObject::connect(ui->pushButtonRouteUpload, SIGNAL(clicked()), m_pKernel, SLOT(slotUploadRouteTable()));

    // Загрузка настроек маршрутизации
    ProgramSettings settings;
    ui->lineEdiGateway->setText(settings.value("Route/Gate").toString());

    TRouteTable table = settings.RouteTable();
    QMapIterator<ushort, uchar> i(table);
    while (i.hasNext())
    {
        i.next();

        int rowIndex = ui->tableWidgetRoutes->rowCount();
        ui->tableWidgetRoutes->insertRow(rowIndex);

        // Отображение ID устройства (ключевое поле)
        QTableWidgetItem *pItem = new QTableWidgetItem(QString("%1").arg(i.key()));
        pItem->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidgetRoutes->setItem(rowIndex, 0, pItem);

        // Отображение CAN-адреса (поле значения)
        pItem = new QTableWidgetItem(QString("%1").arg(i.value()));
        pItem->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidgetRoutes->setItem(rowIndex, 1, pItem);
    }

    //  установка текущего шлюза
    CUICommand NewCommand(CUICommand::eCmdSetGateway);
    NewCommand.qsArguments = ui->lineEdiGateway->text();
    emit signalRunCommandFromUI(NewCommand);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotNewMessageToUI(const CUICommand UIcommand)
{

    switch(UIcommand.teUIcommand)
    {

    default:
    {

    }break;
    }// switch

}

void MainWindow::on_tableWidgetRoutes_cellActivated(int row, int column)
{
    CUICommand NewCommand(CUICommand::eCmdConnect);
    QString IDtext = ui->tableWidgetRoutes->item(row, 0)->text();
    NewCommand.qsArguments = IDtext;

    emit signalRunCommandFromUI(NewCommand);
}

void MainWindow::on_lineEdiGateway_textChanged(const QString &arg1)
{
    CUICommand NewCommand(CUICommand::eCmdSetGateway);
    NewCommand.qsArguments = arg1;

    emit signalRunCommandFromUI(NewCommand);
}

void MainWindow::on_tableWidgetRoutes_clicked(const QModelIndex &index)
{
    CUICommand NewCommand(CUICommand::eCmdConnect);
    int row = ui->tableWidgetRoutes->currentRow();
    QString IDtext = ui->tableWidgetRoutes->item(row, 0)->text();
    NewCommand.qsArguments = IDtext;

    emit signalRunCommandFromUI(NewCommand);
}
