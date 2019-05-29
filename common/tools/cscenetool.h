#ifndef CSCENETOOL_H
#define CSCENETOOL_H

#include "ctools.h"
#include "cstrokestemplate.h"


using namespace sm;

class CSceneTool : public CAbstractTool
{
  Q_OBJECT

public:
  CSceneTool(QWidget * =0, bool post_event_tool=true);

  void setGraphicsView(QGraphicsView*) override;

protected:
  QGraphicsItem * _rect_item;
  QPointF _begin_pos;

  bool _pressed=false;

  bool _move=false;
  bool _drag=false;

  bool eventFilter(QObject *, QEvent *) override;

  virtual bool clickAction(QGraphicsSceneMouseEvent*);


  virtual bool sceneCanAcceptDrag(QGraphicsSceneMouseEvent*) {return false;}
  virtual void sceneBeginDrag(QGraphicsSceneMouseEvent*) {}
  virtual void sceneDrag(QGraphicsSceneMouseEvent*) {}
  virtual void sceneEndDrag(QGraphicsSceneMouseEvent*) {}
  virtual void sceneMouseMove(QGraphicsSceneMouseEvent*) {}
  virtual bool keyPressed(QKeyEvent*) {return false;}
  virtual bool keyReleased(QKeyEvent*) {return false;}

  void toolActivated() override;
  virtual bool fetchSelection();

protected:
  CStrokesTemplate _tmpl;
  QList<CGraphics> _selection;
  QMap<ObjId, StrokesTemplateData> _init_data;
  bool _fetch_hovered=true;

  void storeInitData();
  void restoreInitParams();
  void executeCommand(const QString&);

private:
  bool _post_event_tool;
  bool _post_event;

  void lockTool() override {}
  void unlockTool() override {}

private slots:
  void _fetch_selection();

};

#endif // CSCENETOOL_H
