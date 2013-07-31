#include "dialogstatistics.h"
#include "ui_dialogstatistics.h"

#include <QKeyEvent>

CDialogStatistics::CDialogStatistics(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogStatistics)
{
  ui->setupUi(this);
}

CDialogStatistics::~CDialogStatistics()
{
  delete ui;
}

void	CDialogStatistics::keyPressEvent ( QKeyEvent * event )
{
  const int iIndexMax = ui->stackedWidget->count() - 1;
  int iIndexCurent = ui->stackedWidget->currentIndex();
  switch(event->key()){
    case Qt::Key_Left:
      iIndexCurent--;
      if(0 > iIndexCurent)
        iIndexCurent = iIndexMax;
      ui->stackedWidget->setCurrentIndex(iIndexCurent);
      return;
    case Qt::Key_Right:
      iIndexCurent++;
      if(iIndexMax < iIndexCurent)
        iIndexCurent = 0;
      ui->stackedWidget->setCurrentIndex(iIndexCurent);
      return;
  }

  QDialog::keyPressEvent(event);
}
