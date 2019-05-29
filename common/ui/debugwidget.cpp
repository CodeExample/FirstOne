#include "debugwidget.h"

#include "cprojecthandler.h"

using namespace  sm;


DebugWidget::DebugWidget(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  connect(thicknessSlider, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(thicknessBase, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(basreliefSlider, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(dotlineThreshold, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(smoothSlider, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(whiteSlider, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(blackSlider, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  //roundCups->setChecked(_selected_tmpl.data()->round_cups);
  connect(interval, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  connect(angle, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));
  //cells->setChecked(_selected_tmpl.data()->is_cell);
  connect(cellSize, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged(int)));

  _set_controls_enabled(false);
}


void DebugWidget::_object_selected(const CGraphics &obj)
{
  if (obj && obj.type() == Template)
  {
    _selected_tmpl = obj;
  } else {
    _selected_tmpl = cnull;
  }
  _update_controls();
}


void DebugWidget::_update_controls()
{
  _ignore_changes = true;

  if (_selected_tmpl)
  {
    thicknessSlider->setValue(_selected_tmpl.data()->thickness*100.);
    thicknessBase->setValue(_selected_tmpl.data()->thickness_base*100.);
    basreliefSlider->setValue(_selected_tmpl.data()->basrelief_rise*100.);
    thresholdSlider->setValue(100-_selected_tmpl.data()->threshold*100.);
    dotlineThreshold->setValue(100-_selected_tmpl.data()->dotline_threshold*100.);
    smoothSlider->setValue(_selected_tmpl.data()->smooth);
    whiteSlider->setValue(100-_selected_tmpl.data()->white*100.);
    blackSlider->setValue(100-_selected_tmpl.data()->black*100.);
    roundCups->setChecked(_selected_tmpl.data()->round_cups);
    interval->setValue(_selected_tmpl.data()->interval*25.);
    angle->setValue(_selected_tmpl.data()->angle);
    cells->setChecked(_selected_tmpl.data()->is_cell);
    cellSize->setValue(_selected_tmpl.data()->cell_size);

    _set_controls_enabled(true);
  } else {
    _set_controls_enabled(false);
  }
  _ignore_changes = false;
}

void DebugWidget::_set_controls_enabled(bool b)
{
  thicknessSlider->setEnabled(b);
  thicknessBase->setEnabled(b);
  basreliefSlider->setEnabled(b);
  thresholdSlider->setEnabled(b);
  dotlineThreshold->setEnabled(b);
  smoothSlider->setEnabled(b);
  whiteSlider->setEnabled(b);
  blackSlider->setEnabled(b);
  roundCups->setEnabled(b);
  interval->setEnabled(b);
  angle->setEnabled(b);
  cells->setEnabled(b);
  cellSize->setEnabled(b);
}


void DebugWidget::_update_tmpl_params(const CStrokesTemplate &tmpl)
{
  if (!tmpl)
    return;

  if (_ignore_changes)
    return;

  tmpl.data()->thickness = thicknessSlider->value()/100.;
  tmpl.data()->thickness_base = thicknessBase->value()/100.;
  tmpl.data()->basrelief_rise = basreliefSlider->value()/100.;
  tmpl.data()->threshold = 1 - thresholdSlider->value()/100.;
  tmpl.data()->dotline_threshold = 1-dotlineThreshold->value()/100.;
  tmpl.data()->smooth = smoothSlider->value();
  tmpl.data()->white = 1.-whiteSlider->value()/100.;
  tmpl.data()->black = 1.-blackSlider->value()/100.;
  tmpl.data()->round_cups = roundCups->isChecked();
  tmpl.data()->interval = interval->value()/25.;
  tmpl.data()->angle = angle->value();
  tmpl.data()->is_cell = cells->isChecked();
  tmpl.data()->cell_size = cellSize->value()/100.;

  tmpl.data()->is_changed = true;
  //qDebug() << tmpl.data()->white << tmpl.data()->black << tmpl.data()->threshold;

  hprj->updateItems();
}


void DebugWidget::on_valueChanged(int)
{
  _update_tmpl_params(_selected_tmpl);
}


void DebugWidget::on_roundCups_stateChanged(int )
{
  _update_tmpl_params(_selected_tmpl);
}
