#ifndef PARAMSPANELWIDGET_H
#define PARAMSPANELWIDGET_H

#include "cstrokestemplate.h"

#include <QWidget>

using namespace sm;

class ParamsPanelWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ParamsPanelWidget(QWidget *parent = 0);


protected:
  CStrokesTemplate _tmpl;
  QList<CGraphics> _selection;
  QMap<ObjId, StrokesTemplateData> _init_data;

  virtual void selected() {}
  virtual void unselected() {}
  virtual void updateControls() {}
  void storeInitData();
  void restoreInitData();
  bool isEquivalentData() const;

private slots:
  void _selection_changed();
  void _history_changed();

signals:
  void editingStarted();
  void editingFinished();
};

#endif // PARAMSPANELWIDGET_H
