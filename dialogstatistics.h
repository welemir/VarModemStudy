#ifndef DIALOGSTATISTICS_H
#define DIALOGSTATISTICS_H

#include <QDialog>

namespace Ui {
class DialogStatistics;
}

class CDialogStatistics : public QDialog
{
  Q_OBJECT
  
public:
  explicit CDialogStatistics(QWidget *parent = 0);
  ~CDialogStatistics();

protected:
  virtual void	keyPressEvent ( QKeyEvent * event );

private:
  Ui::DialogStatistics *ui;
};

#endif // DIALOGSTATISTICS_H
