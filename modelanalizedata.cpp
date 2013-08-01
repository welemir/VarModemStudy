#include "modelanalizedata.h"

CModelAnalizeData::CModelAnalizeData(QObject *parent) :
  QAbstractItemModel(parent)
  ,m_iMaxColumnCount(0)
{
}

QVariant CModelAnalizeData::data(const QModelIndex & index, int role/* = Qt::DisplayRole*/) const{
  switch(role){
    case Qt::DisplayRole:
      return m_StatData.value(index.row()).value(index.column());
      break;
    default:
      return QVariant();
  }
}

bool CModelAnalizeData::setData(const QModelIndex &index, const QVariant &value, int /*role = Qt::EditRole*/){
  m_StatData[index.row()][index.column()] = value.toInt();
  int iColumnCountNew = m_StatData[index.row()].size();
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
