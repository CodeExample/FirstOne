#include "cscenetool.h"

#include "citems.h"
#include "cprojecthandler.h"
#include "cshapeitem.h"
#include "ccommands.h"

#include <QApplication>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>


CSceneTool::CSceneTool(QWidget * parent, bool post_event_tool) :
  CAbstractTool(parent),
  _rect_item(0), _pressed(false),
  _post_event_tool(post_event_tool), _post_event(false)
{
  connect(hprj, SIGNAL(selectionChanged()), this, SLOT(_fetch_selection()));
}




void CSceneTool::setGraphicsView(QGraphicsView * view)
{
  CAbstractTool::setGraphicsView(view);

  _scene->installEventFilter(this);
}


void CSceneTool::toolActivated()
{
  fetchSelection();
}


bool CSceneTool::eventFilter(QObject * object, QEvent *event)
{
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

    _drag = false;

    event_accepted = true;
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

    if (!_move)
    {
      _move = true;

      if (!sceneCanAcceptDrag(mouse_event))
        return false;

      sceneBeginDrag(mouse_event);
      _drag = true;
    }

    _move = true;

    event_accepted = true;

    if (_drag)
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

      if (!_drag)
        return false;

      event_accepted = true;
      sceneEndDrag(mouse_event);
    }

    _pressed = false;
    _move = false;
    _drag = false;
    break;
  }

  default:;
  }

  return event_accepted;
}


void CSceneTool::executeCommand(const QString &text)
{
  QUndoCommand * group = new QUndoCommand(text);

  for(const CStrokesTemplate &tmpl : _selection)
  {
    commEditParams * comm = new commEditParams(text, tmpl, _init_data.value(tmpl.id()), group);
    Q_UNUSED(comm)
  }

  hprj->execute(group);

  storeInitData();
}


void CSceneTool::storeInitData()
{
  _init_data.clear();
  for(const CStrokesTemplate &tmpl : _selection)
  {
    _init_data.insert(tmpl.id(), *tmpl.data());
  }
}


void CSceneTool::restoreInitParams()
{
  for(const CStrokesTemplate &tmpl : _selection)
  {
    if (_init_data.contains(tmpl.id()))
    {
      tmpl.data()->assignParams(_init_data.value(tmpl.id()));
    }
  }
}

void CSceneTool::_fetch_selection()
{
  fetchSelection();
}


bool CSceneTool::fetchSelection()
{
  _selection.clear();
  _tmpl = cnull;
  if (!hprj->getSelected(Template, _selection) && _fetch_hovered)
  {
    _tmpl = hprj->hoveredTmpl();
    if (_tmpl)
      _selection << _tmpl;
  }
  return _selection.count() > 0;
}



bool CSceneTool::clickAction(QGraphicsSceneMouseEvent * event)
{
  bool accepted = false;
  QGraphicsItem * item;
  if (hprj->getHitItem(event->scenePos(), item))
  {
    if (item->type() == ShapeItem)
    {
      CShapeItem * shape_item = static_cast<CShapeItem*>(item);
      accepted |= shape_item->mouseSceneClickEvent(event);
    }
  }

  if (!accepted)
    hprj->unselectAll();


  return accepted;
}


