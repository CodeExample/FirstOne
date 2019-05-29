#ifndef VALUECONTROL_H
#define VALUECONTROL_H

#include "ui_valuecontrol.h"

class ValueControl : public QWidget, private Ui::ValueControl
{
  Q_OBJECT

public:
  explicit ValueControl(QWidget *parent = 0);
};

#endif // VALUECONTROL_H
