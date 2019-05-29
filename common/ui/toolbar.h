#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "ui_toolbar.h"
#include "ctools.h"

using namespace sm;

class ToolBar : public QWidget, private Ui::ToolBar
{
  Q_OBJECT

public:


  explicit ToolBar(QWidget *parent = nullptr);



private slots:
  void _tool_changed(CAbstractTool*);

  void on_buttonShowImage_clicked(bool checked);
};

#endif // TOOLBAR_H
