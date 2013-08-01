#ifndef FORMBITSERRORSINBYTES_H
#define FORMBITSERRORSINBYTES_H

#include <QWidget>
#include <QAbstractItemModel>

namespace Ui {
class CFormBitsErrorsInBytes;
}

class CFormBitsErrorsInBytes : public QWidget
{
  Q_OBJECT
  
public:
  explicit CFormBitsErrorsInBytes(QWidget *parent = 0);
  ~CFormBitsErrorsInBytes();
  void setModel(QAbstractItemModel *model);

public slots:
  void	slotDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
private:
  Ui::CFormBitsErrorsInBytes *ui;

  QAbstractItemModel *m_pDataModel;
};

#endif // FORMBITSERRORSINBYTES_H
