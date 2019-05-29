#ifndef NEWPROJECTDLG_H
#define NEWPROJECTDLG_H

#include "cenvironment.h"

#include <QDialog>


namespace Ui {
class NewProjectDlg;
}

using namespace sm;

class NewProjectDlg : public QDialog
{
  Q_OBJECT

public:
  explicit NewProjectDlg(QWidget *parent = nullptr);
  ~NewProjectDlg();

  QSizeF docSize() const;
  int docResolution();
  vxMetrics docMetric();

private:
  Ui::NewProjectDlg *ui;
};

#endif // NEWPROJECTDLG_H
