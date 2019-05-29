#include "smoothpanel.h"

#include "cprojecthandler.h"
#include "cstrokestemplate.h"
#include "ccommands.h"

using namespace sm;

SmoothPanel::SmoothPanel(QWidget *parent) :
  ParamsPanelWidget(parent)
{
  setupUi(this);
}


void SmoothPanel::setSmooth(int a)
{
  smooth->blockSignals(true);

  smooth->setValue(a);

  smooth->blockSignals(false);
}


void SmoothPanel::updateControls()
{
  setSmooth(_tmpl.data()->smooth);
}


void SmoothPanel::selected()
{
  label->setEnabled(true);
  smooth->setEnabled(true);

  connect(smooth, SIGNAL(sliderPressed()), this, SLOT(_editing_started()));
  connect(smooth, SIGNAL(sliderReleased()), this, SLOT(_editing_finished()));
  connect(smooth, SIGNAL(valueChanged(int)), this, SLOT(_value_changed(int)));
}


void SmoothPanel::unselected()
{
  label->setEnabled(false);
  smooth->setEnabled(false);

  disconnect(smooth);
}


void SmoothPanel::_value_changed(int)
{
  _tmpl.data()->smooth = smooth->value();

  _tmpl.data()->is_changed = true;

  hprj->updateStrokesTemplate(_tmpl);
}


void SmoothPanel::_editing_started()
{
  _init_data.assignParams(_tmpl.data());
}


void SmoothPanel::_editing_finished()
{
  if (!_tmpl.data()->isEquivalent(_init_data))
  {
    commEditParams * comm = new commEditParams("Smooth", _tmpl, _init_data);
    hprj->execute(comm);
  }
}
