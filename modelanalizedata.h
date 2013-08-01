#ifndef MODELANALIZEDATA_H
#define MODELANALIZEDATA_H

#include <QAbstractItemModel>
#include <QMap>

class CModelAnalizeData : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit CModelAnalizeData(QObject *parent = 0);
  
  virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex & index) const;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:
  
public slots:

protected:
  QMap<int, QMap<int, int> > m_StatData;
  int m_iMaxColumnCount;
};

#endif // MODELANALIZEDATA_H
