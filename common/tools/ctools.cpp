#include "ctools.h"

#include <QMouseEvent>

using namespace sm;

CAbstractTool::CAbstractTool(QWidget * parent) :
  QObject(parent), _attached_tool(0),
  _mouse_press(false), _mouse_move(false),
  _mouse_drag(false), _key_pressed(false),
  _enabled_tool(true), _view(0), _scene(0)
{
}


CAbstractTool::~CAbstractTool()
{
  if (_attached_tool)
    _attached_tool->deleteLater();

  if (_hovered_tool == this)
    _hovered_tool = 0;

}


QWidget * CAbstractTool::paramsPanel()
{
  return nullptr;
}


void CAbstractTool::toolActivated()
{
}


void CAbstractTool::toolDeactivated()
{
  if (_view)
    _view->removeEventFilter(this);
}



void CAbstractTool::setGraphicsView(QGraphicsView *view)
{
  _view = view;
  _view->installEventFilter(this);

  _scene = view->scene();
}


void CAbstractTool::attachTool(CAbstractTool*tool)
{
  if (_attached_tool)
  {
    _attached_tool->attachTool(tool);
    return;
  }
  _attached_tool = tool;

  tool->setGraphicsView(_view);
}


CAbstractTool * CAbstractTool::_hovered_tool = 0;

bool CAbstractTool::eventFilter(QObject * object, QEvent * event)
{
  lockTool();

  bool accepted = false;

  switch(event->type())
  {

  case QEvent::MouseButtonDblClick:
  case QEvent::MouseButtonPress:
  {
    QMouseEvent * me = static_cast<QMouseEvent*>(event);

    if (me->button() == Qt::RightButton)
      return false;

    _press_ev_pos = me->pos();
    _press_ev_globalPos = me->globalPos();
    _press_ev_button = me->button();
    _press_ev_buttons = me->buttons();
    _press_ev_modifiers = me->modifiers();

    _mouse_press = true;
    _mouse_move = false;
    _mouse_drag = false;

    _can_accept_drag = _enabled_tool && canAcceptDrag(me);
    _can_accept_click = _enabled_tool && canAcceptClick(me);

    accepted = _can_accept_drag || _can_accept_click;
    break;
  }

  case QEvent::MouseButtonRelease:
  {
    QMouseEvent * me = static_cast<QMouseEvent*>(event);

    if (_mouse_drag)
    {
      mouseEndDrag(me);
      accepted = true;
    } else
    if (!_mouse_move && _can_accept_click)
    {
      mouseClick(me);
      accepted = true;
    }

    _mouse_drag = false;
    _mouse_press = false;
    break;
  }

  case QEvent::MouseMove:
  {
    QMouseEvent * me = static_cast<QMouseEvent*>(event);

    if (_mouse_press && !_mouse_move &&
        _can_accept_drag && !_mouse_drag)
    {
      QMouseEvent press_ev(
            QEvent::MouseButtonPress,
            _press_ev_pos,
            _press_ev_globalPos,
            _press_ev_button,
            _press_ev_buttons,
            _press_ev_modifiers);

      mouseBeginDrag(&press_ev);

      mouseDrag(me);

      _mouse_drag = true;
      accepted = true;

    }

    if (_mouse_drag)
    {
      mouseDrag(me);
      accepted = true;
      break;
    }

    _mouse_move = true;

    _check_mouse_hover(me);

    mouseMove(me);

    break;
  }

  //case QEvent::MouseButtonDblClick:
  //{
    //QMouseEvent * me = static_cast<QMouseEvent*>(event);
    //_can_accept_click = false;
    //accepted = mouseDblClick(me);
  //  break;
 // }

  case QEvent::KeyPress:
  {
    QKeyEvent * ke = static_cast<QKeyEvent*>(event);

    if (!_key_pressed)
      _key_pressed = keyDown(ke);

    accepted = _key_pressed;
    break;
  }

  case QEvent::KeyRelease:
  {
    if (_key_pressed)
    {
      QKeyEvent * ke = static_cast<QKeyEvent*>(event);
      keyDown(ke);
      _key_pressed = false;
      accepted = true;
    }
    break;
  }
  default:;
  }

  if (!accepted && _attached_tool)
    accepted = _attached_tool->eventFilter(object, event);

  unlockTool();

  return accepted;
}


void CAbstractTool::_check_mouse_hover(QMouseEvent*me)
{
  if (!_mouse_press)
  {
    if (canAcceptHover(me))
    {
      if (_hovered_tool)
      {
        if (_hovered_tool != this)
        {
          _hovered_tool->mouseHoverLeave(me);
          mouseHoverEnter(me);
          _hovered_tool = this;
        }
      } else {
        mouseHoverEnter(me);
        _hovered_tool = this;
      }
    } else {
      if (_hovered_tool == this)
      {
        mouseHoverLeave(me);
        _hovered_tool = 0;
      }
    }
  }
}
