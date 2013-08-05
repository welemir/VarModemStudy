#ifndef FORMPACKETSCOMPARE_H
#define FORMPACKETSCOMPARE_H

#include <QWidget>
//#include <QAbstractItemView>
#include <QAbstractItemModel>

namespace Ui {
class CFormPacketsCompare;
}

class CFormPacketsCompare : public QWidget/*QAbstractItemView*/
{
  Q_OBJECT
  
public:
  explicit CFormPacketsCompare(QWidget *parent = 0);
  ~CFormPacketsCompare();
  
//  virtual QModelIndex	indexAt(const QPoint & point);
//  virtual void	scrollTo(const QModelIndex & index, ScrollHint hint = EnsureVisible);
//  virtual QRect	visualRect(const QModelIndex & index) const;

  void setModel(QAbstractItemModel *model);

public slots:
  void	slotDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );

protected:
  virtual void paintEvent(QPaintEvent * event);

private:
  Ui::CFormPacketsCompare *ui;

  QAbstractItemModel *m_pDataModel;
};

#endif // FORMPACKETSCOMPARE_H
