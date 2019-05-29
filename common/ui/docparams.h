#ifndef DOCPARAMS_H
#define DOCPARAMS_H

#include <QWidget>

namespace Ui {
class DocParams;
}

class DocParams : public QWidget
{
  Q_OBJECT

public:
  explicit DocParams(QWidget *parent = 0);
  ~DocParams();

  QSizeF docSize() const;
  int docResolution() const;

private:
  Ui::DocParams *ui;
};

#endif // DOCPARAMS_H
