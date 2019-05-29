#include "compasspanel.h"

CompassPanel::CompassPanel(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
}

void CompassPanel::on_resetButton_clicked()
{
  setAngle(0);
  emit reset();
}

void CompassPanel::on_angleEdit_valueChanged(int v)
{
  emit angleChanged(v);
}

void CompassPanel::setAngle(double a)
{
  angleEdit->blockSignals(true);
  angleEdit->setValue(a);
  angleEdit->blockSignals(false);
}


