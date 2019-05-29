#include "classotool.h"

#include "citems.h"
#include "cprojecthandler.h"
#include "ccommands.h"

#include <QGraphicsSceneMouseEvent>

using namespace  sm;


CLassoTool::CLassoTool(QWidget * parent) : CSceneTool(parent, false)
{
  _fetch_hovered = false;
}



bool CLassoTool::clickAction(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem * item;
  if (hprj->getHitItem(event->scenePos(), item))
  {
    hprj->unselectAll();
    item->setSelected(!item->isSelected());
  } else {
    hprj->unselectAll();
  }
  hprj->updateItems();
  return true;
}



bool CLassoTool::sceneCanAcceptDrag(QGraphicsSceneMouseEvent*)
{
  return true;
}


void CLassoTool::toolActivated()
{
  CSceneTool::toolActivated();

  _view->setCursor(Qt::CrossCursor);

  hprj->setProperty(PROP_DSBL_TEST_HIT_ITEMS, true);
}


void CLassoTool::toolDeactivated()
{
  hprj->setProperty(PROP_DSBL_TEST_HIT_ITEMS, false);
  if (_path_item)
    delete _path_item;
}


void CLassoTool::sceneBeginDrag(QGraphicsSceneMouseEvent * event)
{
  QPen pen(Qt::red, 1);
  pen.setCosmetic(true);

  _begin_pos = event->scenePos();

  _path = QPainterPath();
  _path.moveTo(_begin_pos);

  _path_item = hprj->scene()->addPath(_path, pen);
  _path_item->setZValue(Z_VALUE_SELECTION_FRAME);

  hprj->scene()->update();
}


void CLassoTool::sceneDrag(QGraphicsSceneMouseEvent * event)
{
  QPointF pos = event->scenePos();
  _path.lineTo(pos);
  _path_item->setPath(_path);
}


void CLassoTool::sceneEndDrag(QGraphicsSceneMouseEvent * event)
{
  if (_shape)
    _edit_tmpl(event);
  else
    _create_tmpl(event);

  delete _path_item;

  _path_item = nullptr;

  hprj->updateItems();

  hprj->startRenderProcess(_shape);
}


bool CLassoTool::fetchSelection()
{
  QList<CGraphics> selection;
  _shape = cnull;
  if (hprj->getSelected(Shape, selection) && selection.count() == 1)
  {
    _shape = selection.first();
  }
  return _shape;
}


void CLassoTool::_create_tmpl(QGraphicsSceneMouseEvent*)
{
  CMask mask = CMask::createInstance();

  QPainterPath mask_path = _path;
  mask_path.closeSubpath();

  mask.setPath(mask_path);

  CStrokesTemplate tmpl = CStrokesTemplate::createInstance();

  tmpl.data()->name = "New";
  tmpl.data()->bounding_rect = tmpl.data()->create_rect = _path.boundingRect();
  tmpl.data()->is_knife = false;

  _shape = CShape(mask);
  _shape.add(tmpl);

  commAddToLayer * comm = new commAddToLayer("New strokes", _shape, true);

  hprj->execute(comm);
}


void CLassoTool::_edit_tmpl(QGraphicsSceneMouseEvent * event)
{
  QTransform scene_to_tmpl_tx = _shape.transform().inverted();

  QPainterPath tool_path = scene_to_tmpl_tx.map(_path);
  tool_path.closeSubpath();

  QPainterPath mask_path;
  if (_shape.mask())
  {
    mask_path = _shape.mask().path();

    if (event->modifiers() & Qt::ShiftModifier)
      mask_path |= tool_path;
    else
      if (event->modifiers() & Qt::AltModifier)
        mask_path -= tool_path;
      else
        mask_path = tool_path;
  } else {
    _shape.setMask(CMask::createInstance());
    mask_path = tool_path;
  }

  commEditMask * comm = new commEditMask("Lasso", _shape.mask(), mask_path);

  hprj->execute(comm);
}
