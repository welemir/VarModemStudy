#include "formPacketsCompare.h"
#include "ui_formPacketsCompare.h"
#include "ckernel.h"

#include <QPainter>
#include <math.h>

CFormPacketsCompare::CFormPacketsCompare(QWidget *parent) :
  QWidget/*QAbstractItemView*/(parent),
  ui(new Ui::CFormPacketsCompare)
{
  ui->setupUi(this);

  setModel(CKernel::GetInstance()->getCompareModel());
}

CFormPacketsCompare::~CFormPacketsCompare()
{
  delete ui;
}

void CFormPacketsCompare::setModel(QAbstractItemModel *model){
  m_pDataModel = model;
  connect(m_pDataModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(slotDataChanged(QModelIndex,QModelIndex)));
}

//QModelIndex	CFormPacketsCompare::indexAt ( const QPoint & /*point*/){
//  return model()->index(0,0);
//}

//void	CFormPacketsCompare::scrollTo ( const QModelIndex & /*index*/, ScrollHint /*hint = EnsureVisible*/){

//}

//QRect	CFormPacketsCompare::visualRect ( const QModelIndex & /*index*/) const{
//  return QRect();
//}

void	CFormPacketsCompare::slotDataChanged ( const QModelIndex & /*topLeft*/, const QModelIndex & /*bottomRight*/ ){
  update();
}

void CFormPacketsCompare::paintEvent ( QPaintEvent * /*event*/)
{
  int iMaxRow = m_pDataModel->rowCount();
  int iMaxCol = m_pDataModel->columnCount();
  if(0 >= iMaxCol*iMaxRow)
    return;

  QPainter painter(this);

  int ct_iCellSize  = 5;
  // Настройка системы координат для отрисовки
  painter.setWindow(0,0,ct_iCellSize*iMaxRow,ct_iCellSize*iMaxCol);

  int m_iMaxValue = 300;
  for(int i=0; i < iMaxRow; i++){
    for(int j=0; j < iMaxCol; j++){

    QRect rect;
    rect.setLeft(i*ct_iCellSize);
    rect.setTop(j*ct_iCellSize);
    rect.setRight(rect.left()+ct_iCellSize);
    rect.setBottom(rect.top()+ct_iCellSize);

    QColor color(0,0,0);
    QModelIndex index = m_pDataModel->index(i, j);
    int iValue = m_pDataModel->data(index).toInt();
    if(0 > iValue){
//      color.setBlue(255*iValue/m_iMinValue);
    }else{
      int iRed = 255*iValue/m_iMaxValue;
      color.setRed(iRed);
      color.setGreen(255 - iRed);
    }

    painter.setPen(color);
    painter.setBrush(QBrush(color, Qt::SolidPattern));

    painter.drawRect(rect);
    }
  }
}
