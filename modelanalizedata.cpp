#include "modelanalizedata.h"

CModelAnalizeData::CModelAnalizeData(QObject *parent) :
  QAbstractItemModel(parent)
  ,m_iMaxColumnCount(0)
{
}

QVariant CModelAnalizeData::data(const QModelIndex & index, int role/* = Qt::DisplayRole*/) const{
  switch(role){
    case Qt::DisplayRole:
      if(m_StatData.contains(index.row())
         &&m_StatData.value(index.row()).contains(index.column()))
        return m_StatData.value(index.row()).value(index.column());
      else
        return QVariant(-1);
      break;
    default:
      return QVariant();
  }
}

bool CModelAnalizeData::setData(const QModelIndex &index, const QVariant &value, int /*role = Qt::EditRole*/){
  m_StatData[index.row()][index.column()] = value.toInt();
  int iColumnCountNew = index.column();//m_StatData[index.row()].size();
  if(m_iMaxColumnCount < iColumnCountNew)
    m_iMaxColumnCount = iColumnCountNew;

  emit dataChanged(index, index);
  return true;
}

QModelIndex CModelAnalizeData::index(int row, int column, const QModelIndex & /*parent = QModelIndex()*/) const{
  return createIndex(row, column);
}

QModelIndex CModelAnalizeData::parent(const QModelIndex & index) const{
  return QModelIndex(); // Пустой индекс - родительских элементов нет
}

int CModelAnalizeData::rowCount(const QModelIndex &parent/* = QModelIndex()*/) const{
  return m_StatData.size();
}

int CModelAnalizeData::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const{
  return m_iMaxColumnCount;
}

void CModelAnalizeData::clear(){
  m_StatData.clear();
  m_iMaxColumnCount = 0;
  emit dataChanged(createIndex(0, 0), createIndex(0,0));
}
