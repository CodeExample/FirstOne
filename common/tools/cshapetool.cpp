#include "cshapetool.h"

#include "cshape.h"
#include "cprojecthandler.h"
#include "cmask.h"
#include "ccommands.h"

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>


CShapeTool::CShapeTool(ShapeType shape_type, QWidget * parent) : CFrameTool(parent), _shape_type(shape_type)
{
}


bool CShapeTool::frameAction(const QRectF &, QGraphicsSceneMouseEvent *)
{
  CEngravingItem * rect_item = static_cast<CEngravingItem*>(_rect_item);
  CStrokesTemplate tmpl = rect_item->tmpl();

  hprj->assignItemToObject(_rect_item, tmpl);

  _rect_item = nullptr;

  CShape strokes = CShape::createInstance();
  strokes.add(tmpl);

  commAddToLayer * comm = new commAddToLayer("Create strokes", strokes);

  hprj->execute(comm);

  hprj->select(tmpl);

  return true;
}


bool CShapeTool::clickAction(QGraphicsSceneMouseEvent *)
{
  return false;
}

void CShapeTool::toolActivated()
{
  CFrameTool::toolActivated();
  _view->setCursor(Qt::CrossCursor);
}


void CShapeTool::updateRectItem(const QRectF &)
{
  CEngravingItem * rect_item = static_cast<CEngravingItem*>(_rect_item);

  CStrokesTemplate tmpl = rect_item->tmpl();
  tmpl.data()->bounding_rect = tmpl.data()->create_rect = rect.normalized();
  tmpl.data()->is_changed = true;

  switch(_shape_type)
  {
  case Rectangle:
    break;

  case Ellipse:
    if (!tmpl.data()->mask)
      tmpl.data()->mask = CMask::createInstance();

    QPainterPath path;
    path.addEllipse(rect.normalized());
    tmpl.data()->mask.setPath(path);
    break;
  }
  rect_item->startStrokesRender();
}


void CShapeTool::deleteFrameItem()
{
  if (_rect_item)
  {
    CEngravingItem * engr_item = static_cast<CEngravingItem*>(_rect_item);
    engr_item->releaseItem();
    delete engr_item;
    _rect_item = nullptr;
  }
}


QGraphicsItem * CShapeTool::createFrameItem(const QRectF &)
{
  CStrokesTemplate tmpl = createTmpl(rect);
  CEngravingItem * engr_item = new CEngravingItem(tmpl);
  engr_item->setSelected(true);*/
  return engr_item;
}


CGraphics CShapeTool::createTmpl(const QRectF &rect)
{
  CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
  tmpl.data()->image = hprj->sourceImage();
  tmpl.data()->bounding_rect = tmpl.data()->create_rect = rect;
  tmpl.data()->angle = 45;
  tmpl.data()->round_cups = true;
  tmpl.data()->interval = 4;
  tmpl.data()->thickness = 2.1;
  tmpl.data()->basrelief_rise = -2;
  tmpl.data()->smooth = 3;
  tmpl.data()->steps_on_px = 3;
  tmpl.data()->black =  0.13;
  tmpl.data()->white = 0.29;
  tmpl.data()->dotline_threshold = 0.55;

  switch(_shape_type)
  {
  case Rectangle:
    break;

  case Ellipse:
    QPainterPath path;
    path.addEllipse(rect.normalized());
    CMask mask(path);
    hprj->addMask("__ellipse_mask", mask);
    tmpl.addMask("__ellipse_mask");
    break;
  }
  return tmpl;
}
