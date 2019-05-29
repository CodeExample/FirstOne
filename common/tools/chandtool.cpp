#include "chandtool.h"

#include <QEvent>
#include <QGraphicsSceneMouseEvent>

using namespace sm;

CHandTool::CHandTool(QWidget * parent) : CAbstractTool(parent),
  _pressed(false), _action_in_progress(false)
{
}


CHandTool::~CHandTool()
{
  if (_scene)
    _scene->removeEventFilter(this);
}


void CHandTool::setGraphicsView(QGraphicsView * view)
{
  CAbstractTool::setGraphicsView(view);
  _scene->installEventFilter(this);
}


void CHandTool::toolActivated()
{
  _view->setCursor(Qt::OpenHandCursor);
}



bool CHandTool::eventFilter(QObject *, QEvent * event)
{
  if (_action_in_progress)
    return false;

  _action_in_progress = true;

  bool action_accepted = false;

  switch(event->type())
  {

  case QEvent::GraphicsSceneMousePress:
  {
    QGraphicsSceneMouseEvent * mouse_event = static_cast<QGraphicsSceneMouseEvent*>(event);
    _last_pos = mouse_event->screenPos();
    _pressed = true;
    action_accepted = true;
    break;
  }

  case QEvent::GraphicsSceneMouseRelease:
  {
    _pressed = false;
    action_accepted = true;
    break;
  }

  case QEvent::GraphicsSceneMouseMove:
  {
    QGraphicsSceneMouseEvent * mouse_event = static_cast<QGraphicsSceneMouseEvent*>(event);
    if (_pressed)
    {
      _view->setCursor(Qt::ClosedHandCursor);
      _scroll_area(mouse_event);
      action_accepted = true;
    } else {
      _view->setCursor(Qt::OpenHandCursor);
      _last_pos = mouse_event->screenPos();
    }
    break;

  }

  default:;
  }

  _action_in_progress = false;

  return action_accepted;
}


void CHandTool::_scroll_area(QGraphicsSceneMouseEvent * mouse_event)
{
	double width = _view->viewport()->width();
	double height = _view->viewport()->height();
	QPointF center_scene = _view->mapToScene(QPoint(width/2, height/2));

  QPoint pos = mouse_event->screenPos();
  QPointF dlt = _view->mapToScene(pos) - _view->mapToScene(_last_pos);
  QPointF view_center = center_scene - dlt;
  _view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
  _view->centerOn(view_center);
  _last_pos = pos;
}
