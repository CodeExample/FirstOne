#include "paramspanelwidget.h"

#include "cprojecthandler.h"

ParamsPanelWidget::ParamsPanelWidget(QWidget *parent) :
  QWidget(parent)
{
  connect(hprj, SIGNAL(selectionChanged()), this, SLOT(_selection_changed()));
  connect(hprj, SIGNAL(historyChanged()), this, SLOT(_history_changed()));

  QTimer::singleShot(10, this, SLOT(_selection_changed()));
}


void ParamsPanelWidget::_selection_changed()
{
  bool has_selected = _selection.count() > 0;
  if (!hprj->getSelected(Template, _selection))
  {
    if (has_selected)
    {
      _tmpl = cnull;
      unselected();
    }
  } else {
    _tmpl = _selection.first();
    selected();
    updateControls();
  }
}


void ParamsPanelWidget::_history_changed()
{
  if (_selection.count() > 0)
    updateControls();
}

bool ParamsPanelWidget::isEquivalentData() const
{
  for(const CStrokesTemplate &tmpl : _selection)
  {
    if (!tmpl.data()->isEquivalent(_init_data.value(tmpl.id())))
      return false;
  }
  return true;
}


void ParamsPanelWidget::storeInitData()
{
  _init_data.clear();
  for(const CStrokesTemplate &tmpl : _selection)
  {
    _init_data.insert(tmpl.id(), *tmpl.data());
  }
}


void ParamsPanelWidget::restoreInitData()
{
  for(const CStrokesTemplate &tmpl : _selection)
  {
    tmpl.data()->assignParams(_init_data.value(tmpl.id()));
  }
}
