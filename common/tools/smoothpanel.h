#ifndef SMOOTHPANEL_H
#define SMOOTHPANEL_H

#include "ui_smoothpanel.h"
#include "paramspanelwidget.h"
#include "cgraphics.h"

using namespace sm;

class SmoothPanel : public ParamsPanelWidget, private Ui::SmoothPanel
{
  Q_OBJECT

public:
  explicit SmoothPanel(QWidget *parent = 0);

  void setSmooth(int);

protected:
  void selected() override;
  void unselected() override;
  void updateControls() override;

private:
  StrokesTemplateData _init_data;

private slots:
  void _editing_started();
  void _editing_finished();
  void _value_changed(int);

};

#endif // SMOOTHPANEL_H
