#include "docparams.h"
#include "ui_docparams.h"

DocParams::DocParams(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::DocParams)
{
  ui->setupUi(this);
}

DocParams::~DocParams()
{
  delete ui;
}


QSizeF DocParams::docSize() const
{
  return QSizeF(ui->widthEdit->value(), ui->heightEdit->value());
}


int DocParams::docResolution() const
{
  return ui->resolutionEdit->value();
}
