#include "newprojectdlg.h"
#include "ui_newprojectdlg.h"

NewProjectDlg::NewProjectDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::NewProjectDlg)
{
  ui->setupUi(this);
}

NewProjectDlg::~NewProjectDlg()
{
  delete ui;
}



QSizeF NewProjectDlg::docSize() const
{
  return ui->docParams->docSize();
}

int NewProjectDlg::docResolution()
{
  return ui->docParams->docResolution();
}

vxMetrics NewProjectDlg::docMetric()
{
  return vxmMillimeters;
}
