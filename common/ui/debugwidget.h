#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include "ui_debugwidget.h"

#include "cstrokestemplate.h"

using namespace sm;

class DebugWidget : public QWidget, private Ui::DebugWidget
{
  Q_OBJECT

public:
  explicit DebugWidget(QWidget *parent = nullptr);

private:
  CStrokesTemplate _selected_tmpl;
  bool _ignore_changes=false;

  void _update_controls();
  void _update_tmpl_params(const CStrokesTemplate &);
  void _set_controls_enabled(bool);

private slots:
  void _object_selected(const CGraphics&);

  void on_valueChanged(int value);
  void on_roundCups_stateChanged(int arg1);
};

#endif // DEBUGWIDGET_H
