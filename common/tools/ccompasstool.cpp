#include "ccompasstool.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <math.h>
#include <QDebug>

using namespace sm;

CCompassTool::CCompassTool(QWidget * parent) : CAbstractTool(parent),
  _pressed(false), _action_in_progress(false)
{
  if (qApp->devicePixelRatio() == 2)
  {
    _cursor = QCursor(QPixmap(":/images/resources/images/cr_compass@2x.png"), 15,15);
  } else {
    _cursor = QCursor(QPixmap(":/images/resources/images/cr_compass.png"), 15,15);
  }
}


CCompassTool::~CCompassTool()
{
  if (_scene)
    _scene->removeEventFilter(this);

  if (_panel)
    delete _panel;
}


void CCompassTool::setGraphicsView(QGraphicsView * view)
{
  CAbstractTool::setGraphicsView(view);
  _scene->installEventFilter(this);
}


void CCompassTool::toolActivated()
{
  _view->setCursor(_cursor);

  _center = _view->viewport()->rect().center();
  QRectF view_r = QRectF(_center - QPointF(10,10), QSizeF(20,20));
  QRectF scene_r = _view->mapToScene(view_r.toRect()).boundingRect();
  _scene_center = scene_r.center();
}


QWidget * CCompassTool::paramsPanel()
{
  if (!_panel)
  {
    _panel = new CompassPanel();
    _panel->setAngle(_get_angle());
    connect(_panel, SIGNAL(reset()), this, SLOT(_reset()));
    connect(_panel, SIGNAL(angleChanged(double)), this, SLOT(_angle_changed(double)));
  }
  return _panel;
}


bool CCompassTool::eventFilter(QObject *, QEvent * event)
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
    _angle = QLineF(_center, _view->mapFromScene(mouse_event->scenePos())).angle();
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
      _rotate_area(mouse_event);
      action_accepted = true;
    } else {
      _last_pos = mouse_event->screenPos();
    }
    break;

  }

  default:;
  }

  _action_in_progress = false;

  return action_accepted;
}


double CCompassTool::_get_angle()
{
  QLineF l = _view->transform().map(QLineF(0,0,1,0));
  double a = round(l.angle());
  if (a > 180)
    a = a - 360;
  return a;
}


void CCompassTool::_rotate_area(QGraphicsSceneMouseEvent * mouse_event)
{
  double angle = QLineF(_center, _view->mapFromScene(mouse_event->scenePos())).angle();
  _angle_changed(angle);
  if (_panel)
  {
    _panel->setAngle(_get_angle());
  }
}


void CCompassTool::_reset()
{
  QLineF l = _view->transform().map(QLineF(1,0,0,0));
  _view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
  _view->rotate(180 + l.angle());
  _angle = 0;
}


void CCompassTool::_angle_changed(double angle)
{
  _view->setUpdatesEnabled(false);
  _view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
  _view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
  _view->rotate(_angle - angle);
  _view->centerOn(_scene_center);
  _view->setUpdatesEnabled(true);
  _angle = angle;
}
