#include "formbitserrorsinbytes.h"
#include "ui_formbitserrorsinbytes.h"
#include "ckernel.h"

#include <QVector>
#include <QDebug>
#include <QPlastiqueStyle>

CFormBitsErrorsInBytes::CFormBitsErrorsInBytes(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CFormBitsErrorsInBytes)
  ,m_pDataModel(0)
{
  ui->setupUi(this);
  QPlastiqueStyle *pStyle = new QPlastiqueStyle();
  setModel(CKernel::GetInstance()->getDataModel());
}

CFormBitsErrorsInBytes::~CFormBitsErrorsInBytes()
{
  delete ui;
}

void CFormBitsErrorsInBytes::setModel(QAbstractItemModel *model){
  m_pDataModel = model;
  connect(m_pDataModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(slotDataChanged(QModelIndex,QModelIndex)));
}

void	CFormBitsErrorsInBytes::slotDataChanged ( const QModelIndex & /*topLeft*/, const QModelIndex & /*bottomRight*/ ){
  QVector<int> vectData;
  vectData.resize(9);
  int iMaxValue = 0;
  for(int i=0;i<9;i++){
    int iValue = m_pDataModel->data(m_pDataModel->index(0,i)).toInt();
    iMaxValue = (iMaxValue < iValue)? iValue : iMaxValue;
    vectData[i] = iValue;
  }

  ui->progressBar_0->setMaximum(iMaxValue);
  ui->progressBar_1->setMaximum(iMaxValue);
  ui->progressBar_2->setMaximum(iMaxValue);
  ui->progressBar_3->setMaximum(iMaxValue);
  ui->progressBar_4->setMaximum(iMaxValue);
  ui->progressBar_5->setMaximum(iMaxValue);
  ui->progressBar_6->setMaximum(iMaxValue);
  ui->progressBar_7->setMaximum(iMaxValue);
  ui->progressBar_8->setMaximum(iMaxValue);

  ui->progressBar_0->setValue(vectData[0]);
  ui->progressBar_0->setToolTip(QString("%1").arg(vectData[0]));
  ui->progressBar_1->setValue(vectData[1]);
  ui->progressBar_1->setToolTip(QString("%1").arg(vectData[1]));
  ui->progressBar_2->setValue(vectData[2]);
  ui->progressBar_2->setToolTip(QString("%1").arg(vectData[2]));
  ui->progressBar_3->setValue(vectData[3]);
  ui->progressBar_3->setToolTip(QString("%1").arg(vectData[3]));
  ui->progressBar_4->setValue(vectData[4]);
  ui->progressBar_4->setToolTip(QString("%1").arg(vectData[4]));
  ui->progressBar_5->setValue(vectData[5]);
  ui->progressBar_5->setToolTip(QString("%1").arg(vectData[5]));
  ui->progressBar_6->setValue(vectData[6]);
  ui->progressBar_6->setToolTip(QString("%1").arg(vectData[6]));
  ui->progressBar_7->setValue(vectData[7]);
  ui->progressBar_7->setToolTip(QString("%1").arg(vectData[7]));
  ui->progressBar_8->setValue(vectData[8]);
  ui->progressBar_8->setToolTip(QString("%1").arg(vectData[8]));

}
