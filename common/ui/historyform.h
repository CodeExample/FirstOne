#ifndef HISTORYFORM_H
#define HISTORYFORM_H

#include "ui_historyform.h"

class HistoryForm : public QWidget, private Ui::HistoryForm
{
  Q_OBJECT

public:
  explicit HistoryForm(QWidget *parent = nullptr);

};

#endif // HISTORYFORM_H
