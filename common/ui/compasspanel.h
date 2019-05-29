#ifndef COMPASSPANEL_H
#define COMPASSPANEL_H

#include "ui_compasspanel.h"

class CompassPanel : public QWidget, private Ui::CompassPanel
{
  Q_OBJECT

public:
  explicit CompassPanel(QWidget *parent = 0);
  void setAngle(double);
private slots:
  void on_resetButton_clicked();

  void on_angleEdit_valueChanged(int arg1);

signals:
  void reset();
  void angleChanged(double);

};

#endif // COMPASSPANEL_H
