#ifndef CLASSOTOOL_H
#define CLASSOTOOL_H

#include "cscenetool.h"
#include "cstrokestemplate.h"
#include "cshape.h"
#include <QGraphicsLineItem>

using namespace sm;

class CLassoTool : public CSceneTool
{
  Q_OBJECT

public:
  CLassoTool(QWidget* =0);

  int toolType() const override
  {
    return ToolLasso;
  }

  //QWidget * paramsPanel() override;


protected:
  CShape _shape;
  QPointF _begin_pos;
  QGraphicsPathItem * _path_item=nullptr;
  QPainterPath _path;


  bool clickAction(QGraphicsSceneMouseEvent*) override;
  bool sceneCanAcceptDrag(QGraphicsSceneMouseEvent*) override;
  void sceneBeginDrag(QGraphicsSceneMouseEvent*) override;
  void sceneDrag(QGraphicsSceneMouseEvent*) override;
  void sceneEndDrag(QGraphicsSceneMouseEvent*) override;

  void toolActivated() override;

  void toolDeactivated() override;

  bool fetchSelection() override;

private:
  void _create_tmpl(QGraphicsSceneMouseEvent*);
  void _edit_tmpl(QGraphicsSceneMouseEvent*);

};

#endif // CLASSOTOOL_H
