#include "cframetool.h"

#include "citems.h"
#include "cprojecthandler.h"

#include <QApplication>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>


CFrameTool::CFrameTool(QWidget * parent, bool post_event_tool) :
  CAbstractTool(parent),
  _rect_item(0), _pressed(false),
  _post_event_tool(post_event_tool), _post_event(false)
{
}




void CFrameTool::setGraphicsView(QGraphicsView * view)
{
  CAbstractTool::setGraphicsView(view);

  _scene->installEventFilter(this);

  _frame_pen = QPen(QColor(200,200,255), 1);
  _frame_pen.setCosmetic(true);

  _frame_brush = QBrush(QColor(200,200,255, 40));
}



bool CFrameTool::eventFilter(QObject * object, QEvent *event)
{return false;
  QEvent::Type event_type = event->type();

  if (event_type != QEvent::KeyPress &&
      event_type != QEvent::KeyRelease &&
      event_type != QEvent::GraphicsSceneMousePress &&
      event_type != QEvent::GraphicsSceneMouseMove &&
      event_type != QEvent::GraphicsSceneMouseRelease)
    return false;

  if (_post_event_tool)
  {
    if (_post_event)
      return false;

    _post_event = true;

    qApp->sendEvent(object, event);

    _post_event = false;

    if (event->isAccepted())
      return true;
  }

  if (_attached_tool)
  {
    if (_attached_tool->eventFilter(object, event) || event->isAccepted())
      return true;
  }

  bool event_accepted = false;

  switch(event_type)
  {

  case QEvent::KeyPress:
  {
    QKeyEvent * key_event = static_cast<QKeyEvent*>(event);
    event_accepted = keyPressed(key_event);
    break;
  }

  case QEvent::KeyRelease:
  {
    QKeyEvent * key_event = static_cast<QKeyEvent*>(event);
    event_accepted = keyReleased(key_event);
    break;
  }

  case QEvent::GraphicsSceneMousePress:
  {
    QGraphicsSceneMouseEvent * mouse_event = static_cast<QGraphicsSceneMouseEvent*>(event);

    if (mouse_event->button() != Qt::LeftButton)
      break;

    _begin_pos = mouse_event->scenePos();

    _pressed = true;

    _move = false;

    _rect_item = createFrameItem(QRectF(_begin_pos, QSizeF(0,0)));
    _rect_item->setZValue(Z_VALUE_SELECTION_FRAME);

    _scene->addItem(_rect_item);

    event_accepted = true;

    sceneBeginDrag(mouse_event);

    break;
  }


  case QEvent::GraphicsSceneMouseMove:
  {
    QGraphicsSceneMouseEvent * mouse_event = static_cast<QGraphicsSceneMouseEvent*>(event);

    if (!_pressed)
    {
      sceneMouseMove(mouse_event);
      break;
    }

    _move = true;

    QRectF selection_rect(_begin_pos, mouse_event->scenePos());

    updateRectItem(selection_rect);

    _rect_item->setVisible(true);
    _rect_item->update();

    event_accepted = true;

    sceneDrag(mouse_event);

    break;
  }

  case QEvent::GraphicsSceneMouseRelease:
  {
    QGraphicsSceneMouseEvent * mouse_event = static_cast<QGraphicsSceneMouseEvent*>(event);
    if (!_move)
    {
      event_accepted = clickAction(mouse_event);
    } else {

      if (_rect_item && _scene->items().contains(_rect_item))
      {
        event_accepted = frameAction(_rect_item->mapToScene(_rect_item->boundingRect()).boundingRect(), mouse_event);
        deleteFrameItem();
      }

      _move = false;

      sceneEndDrag(mouse_event);
    }
    _pressed = false;

    break;
  }

  default:;
  }

  return event_accepted;
}


void CFrameTool::updateRectItem(const QRectF &r)
{
  QGraphicsRectItem * rect_item = static_cast<QGraphicsRectItem*>(_rect_item);
  rect_item->setRect(r);
  rect_item->update();
}


QGraphicsItem * CFrameTool::createFrameItem(const QRectF &frame_r)
{
  QGraphicsRectItem * rect_item = new QGraphicsRectItem(frame_r);
  rect_item->setPen(_frame_pen);
  rect_item->setBrush(_frame_brush);
  return rect_item;
}


void CFrameTool::deleteFrameItem()
{
  delete _rect_item;
  _rect_item = nullptr;
}


void CFrameTool::toolActivated()
{
  hprj->setProperty(PROP_DSBL_TEST_HIT_ITEMS, true);
}


void CFrameTool::toolDeactivated()
{
  hprj->setProperty(PROP_DSBL_TEST_HIT_ITEMS, false);

  if (_rect_item)
  {
    delete _rect_item;
    _rect_item = nullptr;
  }
}



