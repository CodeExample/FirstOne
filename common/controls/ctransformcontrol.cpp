#include "ctransformcontrol.h"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>

#define  CONTROL_SIZE 10
#define  ROTATE_SPOT  30

QCursor CTransformControl::_cursor_move;
QCursor CTransformControl::_cursor_shear;
QCursor CTransformControl::_cursor_rotate;

QCursor CTransformControl::_cursors_scale[4];
QCursor CTransformControl::_cursors_rotate[8];
bool CTransformControl::_cursors_loaded=false;

CTransformControl::CTransformControl() :
  _hovered(false), _hovered_control(tfNoControl)
{
  setAcceptHoverEvents(true);

  _frame_pen = QPen(Qt::red, 1);
  _frame_pen.setCosmetic(true);

  _hovered_control = tfBody;

  if (!_cursors_loaded)
  {
    _cursors_loaded = true;

    _cursor_move = QCursor(QPixmap(":/cursors/resources/cursors/arrow_move.png"), 7, 7);
    _cursor_shear = QCursor(QPixmap(":/cursors/resources/cursors/shear.png"), 7, 7);

    _cursor_rotate = QCursor(QPixmap(":/cursors/resources/cursors/arrow_circle_045_left.png"), 7, 7);

    _cursors_scale[0] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_resize_090.png"), 7, 7);
    _cursors_scale[1] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_resize_045.png"), 7, 7);
    _cursors_scale[2] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_resize.png"), 7, 7);
    _cursors_scale[3] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_resize_135.png"), 7, 7);

    _cursors_rotate[0] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_right.png"), 7, 7);
    _cursors_rotate[1] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_top_right.png"), 7, 7);
    _cursors_rotate[2] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_top.png"), 7, 7);
    _cursors_rotate[3] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_top_left.png"), 7, 7);
    _cursors_rotate[4] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_left.png"), 7, 7);
    _cursors_rotate[5] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_bottom_left.png"), 7, 7);
    _cursors_rotate[6] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_bottom.png"), 7, 7);
    _cursors_rotate[7] = QCursor(QPixmap(":/cursors/resources/cursors/arrow_rotate_bottom_right.png"), 7, 7);

    _cursor_id = -1;
  }
}


QVariant CTransformControl::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == QGraphicsItem::ItemSceneHasChanged)
  {
    _prev_cursor = cursor();

    _selection_changed();

    connect(scene(), SIGNAL(selectionChanged()), this, SLOT(_selection_changed()));
  }

  return QGraphicsItem::itemChange(change, value);

}


void CTransformControl::finish()
{
  if (hasChanges())
  {
    _notify_changes();
  }
}


void CTransformControl::_selection_changed()
{
  if (hasChanges())
    _notify_changes();

  _selection = scene()->selectedItems();

qDebug() << "selection" << _selection.count();
for(QGraphicsItem * item : _selection)
{
  qDebug() << item->type() << item;
}
  prepareTransform();
}


void CTransformControl::_notify_changes()
{
  emit transformFinished(_tx_caption, _selection);
}



void CTransformControl::prepareTransform()
{
  setPos(0,0);
  setTransform(QTransform());

  _rotation_enabled = true;
  _projection_enabled = true;

  if (_selection.count() == 0)
  {
    _rotation_enabled = false;
    _projection_enabled = false;
    _selection_mode = NoSelected;
    return;
  }

  if (_selection.count() == 1)
  {
    QGraphicsItem * item = _selection.first();
    QTransform to_scene_tx = item->sceneTransform();
    QRectF item_r = item->boundingRect();

    _item_tx_map.insert(item, item->transform());
    _item_to_scene_tx_map.insert(item, item->sceneTransform());

    _ini_tf.tl = _tf.tl = to_scene_tx.map(item_r.topLeft());
    _ini_tf.tr = _tf.tr = to_scene_tx.map(item_r.topRight());
    _ini_tf.bl = _tf.bl = to_scene_tx.map(item_r.bottomLeft());
    _ini_tf.br = _tf.br = to_scene_tx.map(item_r.bottomRight());
    _ini_tf.cr = _tf.cr = to_scene_tx.map(item_r.center());

  } else {
    QRectF init_scene_r;

    for(QGraphicsItem * item : _selection)
    {
      init_scene_r |= item->sceneBoundingRect();
      _item_tx_map.insert(item, item->transform());
      _item_to_scene_tx_map.insert(item, item->sceneTransform());
    }
    _ini_tf.tl = _tf.tl = init_scene_r.topLeft();
    _ini_tf.tr = _tf.tr = init_scene_r.topRight();
    _ini_tf.bl = _tf.bl = init_scene_r.bottomLeft();
    _ini_tf.br = _tf.br = init_scene_r.bottomRight();
    _ini_tf.cr = _tf.cr = init_scene_r.center();
  }

  for(QGraphicsItem * item : _selection)
  {
    _ini_tx_map.insert(item, item->transform());
  }

  _initial_scene_plgn.clear();
  _initial_scene_plgn << _tf.polygon();

  _selection_mode = SelectedShapes;

  update();

  emit transformStarted(_selection);
}



bool CTransformControl::hasChanges() const
{
  for(QGraphicsItem * item : _ini_tx_map.keys())
  {
    QTransform ini_tx = _ini_tx_map.value(item);
    if (ini_tx != item->transform())
      return true;
  }
  return false;
}


bool CTransformControl::cancelTransform()
{
  bool has_changes=false;
  for(QGraphicsItem * item : _ini_tx_map.keys())
  {
    QTransform ini_tx = _ini_tx_map.value(item);
    if (ini_tx != item->transform())
    {
      item->setTransform(_ini_tx_map.value(item));
      has_changes = true;
    }
  }
  return has_changes;
}


QRectF CTransformControl::_get_selection_rect() const
{
  QRectF brect;

  if (_selection.count() == 1)
  {
    brect = _selection.first()->boundingRect();
  } else {
    for(QGraphicsItem * item : _selection)
    {
      brect |= item->sceneBoundingRect();
    }
  }
  return brect;
}


QRectF CTransformControl::boundingRect() const
{
  double s = ROTATE_SPOT/_view_scale;
  QRectF brect = _tf.boundingRect().adjusted(-s,-s,s,s);
  return brect;
}



void CTransformControl::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  if (_selection.count() == 0)
    return;

  painter->save();

  painter->setPen(_frame_pen);
  painter->setBrush(Qt::NoBrush);

  _view_scale = painter->transform().m11();

  painter->setOpacity(0.5);

  painter->drawLine(_tf.tl, _tf.tr);
  painter->drawLine(_tf.bl, _tf.br);
  painter->drawLine(_tf.tl, _tf.bl);
  painter->drawLine(_tf.tr, _tf.br);

  _draw_size_controls(painter);

  painter->restore();
}


void CTransformControl::_draw_size_controls(QPainter * painter)
{
  _draw_size_control(tfTopLeft, painter);
  _draw_size_control(tfTopHCenter, painter);
  _draw_size_control(tfTopRight, painter);

  _draw_size_control(tfBottomLeft, painter);
  _draw_size_control(tfBottomHCenter, painter);
  _draw_size_control(tfBottomRight, painter);

  _draw_size_control(tfVCenterLeft, painter);
  _draw_size_control(tfVCenterRight, painter);

  _draw_size_control(tfCenter, painter);
}



QPointF CTransformControl::_get_control_pos(uint control)
{
  switch(control)
  {
  case tfTopLeft:
    return _tf.tl;

  case tfTopRight:
    return _tf.tr;

  case tfBottomLeft:
    return _tf.bl;

  case tfBottomRight:
    return _tf.br;

  case tfTopHCenter:
    return (_tf.tl + _tf.tr)/2;

  case tfBottomHCenter:
    return (_tf.bl + _tf.br)/2;

  case tfVCenterLeft:
    return (_tf.tl + _tf.bl)/2;

  case tfVCenterRight:
    return (_tf.tr + _tf.br)/2;

  case tfCenter:
    return _tf.cr;

  case tfTop:
    return _tf.tr*_vector_t + _tf.tl*(1 - _vector_t);

  case tfBottom:
    return _tf.br*_vector_t + _tf.bl*(1 - _vector_t);

  case tfLeft:
    return _tf.bl*_vector_t + _tf.tl*(1 - _vector_t);

  case tfRight:
    return _tf.br*_vector_t + _tf.tr*(1 - _vector_t);

  default:;
  }

  return (_tf.tl + _tf.br)/2;
}



void CTransformControl::_draw_size_control(uint control, QPainter * painter)
{
  painter->save();

  QPointF ctrl_p = _get_control_pos(control);

  QTransform tx;
  tx.translate(ctrl_p.x(), ctrl_p.y());

  QTransform painter_tx = tx*painter->transform();
  painter->resetTransform();

  bool hovered = _hovered && (_hovered_control == control);

  switch (control)
  {
  case tfTopLeft:
  case tfTopRight:
  case tfBottomLeft:
  case tfBottomRight:
    if (hovered)
      painter->drawEllipse(painter_tx.map(QPointF(0,0)), 5,5);
    else
      painter->drawEllipse(painter_tx.map(QPointF(0,0)), 4,4);
    break;

  case tfTopHCenter:
  case tfBottomHCenter:
  case tfVCenterLeft:
  case tfVCenterRight:
    if (hovered)
      painter->drawEllipse(painter_tx.map(QPointF(0,0)), 4,4);
    else
      painter->drawEllipse(painter_tx.map(QPointF(0,0)), 3,3);
    break;

  case tfCenter:
    if (hovered)
      painter->drawEllipse(painter_tx.map(QPointF(0,0)), 4,4);
    else
      painter->drawEllipse(painter_tx.map(QPointF(0,0)), 3,3);
    break;
  }

  painter->restore();
}


bool CTransformControl::_check_controls_hover(const QPointF& canvas_pos)
{
  _hovered_control = tfNoControl;

  bool h =
      _check_hover_control(tfTopLeft, canvas_pos) ||
      _check_hover_control(tfTopHCenter, canvas_pos) ||
      _check_hover_control(tfTopRight, canvas_pos) ||

      _check_hover_control(tfBottomLeft, canvas_pos) ||
      _check_hover_control(tfBottomHCenter, canvas_pos) ||
      _check_hover_control(tfBottomRight, canvas_pos) ||

      _check_hover_control(tfVCenterLeft, canvas_pos) ||
      _check_hover_control(tfVCenterRight, canvas_pos) ||

      _check_hover_control(tfLeft, canvas_pos) ||
      _check_hover_control(tfRight, canvas_pos) ||
      _check_hover_control(tfTop, canvas_pos) ||
      _check_hover_control(tfBottom, canvas_pos) ||

      _check_hover_control(tfCenter, canvas_pos) ||

      _check_hover_control(tfBody, canvas_pos) ||

      _check_hover_control(tfRotate, canvas_pos);
  return h;
}


bool CTransformControl::_check_hover_control(TransformFrameControl control, const QPointF& scene_pos)
{
  if (control == tfBody)
  {
    QPolygonF polygon;
    polygon << _tf.polygon();

    if (polygon.containsPoint(scene_pos, Qt::WindingFill))
    {
      _hovered_control = tfBody;
      return true;
    }
    return false;
  }

  if (control == tfRotate)
  {
    if (!_rotation_enabled)
      return false;

    double s = ROTATE_SPOT/_view_scale;
    if (distanceToLine(scene_pos, _tf.tl, _tf.bl) < s ||
        distanceToLine(scene_pos, _tf.tr, _tf.br) < s ||
        distanceToLine(scene_pos, _tf.tl, _tf.tr) < s ||
        distanceToLine(scene_pos, _tf.bl, _tf.br) < s)
    {
      _hovered_control = tfRotate;

      return true;
    }
    return false;
  }


  QPointF control_p = _get_control_pos(control);


  double s = CONTROL_SIZE/_view_scale;

  if (control == tfLeft)
  {
    if (crossPointToLine(_tf.tl, _tf.bl, scene_pos, s/2))
    {
      _vector_t = getTNormal(_tf.tl, _tf.bl, scene_pos);
      _hovered_control = control;
      return true;
    }

    return false;
  }

  if (control == tfRight)
  {
    if (crossPointToLine(_tf.tr, _tf.br, scene_pos, s/2))
    {
      _vector_t = getTNormal(_tf.tr, _tf.br, scene_pos);
      _hovered_control = control;
      return true;
    }

    return false;
  }

  if (control == tfTop)
  {
    if (crossPointToLine(_tf.tl, _tf.tr, scene_pos, s/2))
    {
      _vector_t = getTNormal(_tf.tl, _tf.tr, scene_pos);
      _hovered_control = control;
      return true;
    }

    return false;
  }

  if (control == tfBottom)
  {
    if (crossPointToLine(_tf.bl, _tf.br, scene_pos, s/2))
    {
      _vector_t = getTNormal(_tf.bl, _tf.br, scene_pos);
      _hovered_control = control;
      return true;
    }

    return false;
  }

  QRectF r(control_p - QPointF(s/2, s/2), QSizeF(s,s));

  if (!r.contains(scene_pos))
    return false;

  _hovered_control = control;

  return true;
}




bool CTransformControl::_can_accept_hover(QGraphicsSceneHoverEvent*se)
{
  bool h = _hovered;
  TransformFrameControl h_c = _hovered_control;

  QPointF scene_pos = se->scenePos();
  _hovered = _check_controls_hover(scene_pos);

  QCursor cursor;

  bool shear = se->modifiers() & Qt::ControlModifier;

  int cursor_id;

  if (_hovered_control == tfBody)
  {
    cursor = _cursor_move;
  }
  else if (_hovered_control == tfRotate)
  {
    cursor = _cursor_rotate;

    QLineF line(_tf.cr, scene_pos);
    if (line.length() > 0)
    {
      double a = line.angle();
      cursor_id = (int) round(a / 45);
      cursor = cursor_id > 7 ? _cursors_rotate[0] : _cursors_rotate[cursor_id];
    }
  }
  else if (_hovered_control == tfCenter)
  {
    ;
  }
  else
  {
    QPointF vector;

    QPointF vtop = _tf.tr - _tf.tl;
    QPointF vbottom = _tf.bl - _tf.br;
    QPointF vright = _tf.br - _tf.tr;
    QPointF vleft = _tf.tl - _tf.bl;

    cursor_id = 0;

    switch(_hovered_control)
    {
    case tfTop:
    case tfTopHCenter:
      vector = vtop;
      break;

    case tfTopRight:
      vector = vtop + vright;
      cursor_id = 1;
      break;

    case tfRight:
    case tfVCenterRight:
      vector = vright;
      cursor_id = 2;
      break;

    case tfBottomRight:
      vector = vright + vbottom;
      cursor_id = 3;
      break;

    case tfBottom:
    case tfBottomHCenter:
      vector = vbottom;
      cursor_id = 0;
      break;

    case tfBottomLeft:
      vector = vbottom + vleft;
      cursor_id = 1;
      break;

    case tfLeft:
    case tfVCenterLeft:
      vector = vleft;
      cursor_id = 2;
      break;

    case tfTopLeft:
      vector = vleft + vtop;
      cursor_id = 3;
      break;

    default:
      return false;
    }

    QLineF line(QPointF(), vector);

    if (line.length() != 0)
    {
      QLineF d = line.normalVector().unitVector();
      double dx = d.dx();
      double dy = d.dy();

      if (fabs(dx) < 0.2)
      {
        cursor_id = 0;
      }
      else if (fabs(dy) < 0.2)
      {
        cursor_id = 2;
      }
      else if (dx * dy > 0)
      {
        cursor_id = 3;
      }
      else
      {
        cursor_id = 1;
      }
    }

    cursor = shear ? _cursor_shear : _cursors_scale[cursor_id];
  }

  if (_hovered)
  {
    setCursor(cursor);
    _cursor_id = cursor_id;
  }
  else
  {
    if (h && _cursor_id >= 0)
      unsetCursor();

    _cursor_id = -1;
  }

  if (h_c != _hovered_control)
    update();

  return _hovered;
}



void CTransformControl::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
  _hovered = _can_accept_hover(event);
}

void CTransformControl::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
  _hovered = false;
}


//
// Drag
//

void CTransformControl::mousePressEvent(QGraphicsSceneMouseEvent * se)
{
  _start_pos = _scene_pos = se->scenePos();

  _tf.prepareTransformations(_hovered_control, _scene_pos, _view_scale);

  if (_hovered_control == tfRotate)
  {
    _rotate_center = _tf.cr;
    _started_angle = -M_PI*QLineF(_rotate_center, _scene_pos).angle()/180.;
    _angle = 0;
  }
}


void CTransformControl::mouseMoveEvent(QGraphicsSceneMouseEvent *se)
{
  QPointF canvas_pos = se->scenePos();

  _set_control_positions(canvas_pos,
                         (se->modifiers() & Qt::AltModifier),
                         (se->modifiers() & Qt::ShiftModifier),
                         (se->modifiers() & Qt::ControlModifier) && _projection_enabled,
                         (se->modifiers() & Qt::MetaModifier));
  update();
}



void CTransformControl::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
  _hovered_control = tfNoControl;

  update();
}




void CTransformControl::_set_control_positions(const QPointF& item_pos,
                        bool reflection, bool proportional, bool projection, bool fixed)
{
  switch (_hovered_control)
  {

  case tfCenter:
  {
    _tf.cr = item_pos;
    break;
  }

  case tfRotate:
  {
    _angle = -M_PI*QLineF(_rotate_center, item_pos).angle()/180. - _started_angle;

    if (fabs(_angle) < 0.03)
      _angle = 0;

    if (fixed)
      _angle = M_PI*15*round((180.0*_angle/M_PI)/15)/180.0;

    _tf.rotate(_angle);
    break;
  }

  case tfBody:
  {
    QPointF d = item_pos - _scene_pos;
    _scene_pos = item_pos;
    _tf.translate(d);
    break;
  }

  default:

    if (projection)
      _tf.setProjectionPos(item_pos, reflection, proportional, fixed);
    else
      _tf.setControlPos(item_pos, reflection, proportional, fixed);

    if (projection)
      setCursor(_cursor_shear);
    else
      if (_cursor_id >=0 && _cursor_id < 4)
        setCursor(_cursors_scale[_cursor_id]);
  }

  _update_transform_frame_data();
}




bool CTransformControl::_update_transform_frame_data()
{
  QTransform preview_tx;

  QPolygonF on_scene_plgn = _tf.polygon();

  if (!QTransform::quadToQuad(_initial_scene_plgn, on_scene_plgn, preview_tx))
    return false;

  switch(preview_tx.type())
  {
  case QTransform::TxTranslate:
    _tx_caption = "Move";
    break;
  case QTransform::TxScale:
    _tx_caption = "Resize";
    break;
  case QTransform::TxRotate:
    _tx_caption = "Rotate";
    break;
  case QTransform::TxShear:
    _tx_caption = "Shear";
    break;
  case QTransform::TxProject:
    _tx_caption = "Project";
    break;
  default:
    _tx_caption = "Transform";
  }

  QRectF check_r = preview_tx.map(_initial_scene_plgn).boundingRect();
  double shape_s = on_scene_plgn.boundingRect().width()*on_scene_plgn.boundingRect().height();

  double check_s = check_r.width() + check_r.height();

  if (check_s/shape_s >= 10)
    return false;

  prepareGeometryChange();

  for(QGraphicsItem * item : _selection)
  {
    QTransform item_tx = _item_tx_map.value(item);
    QTransform item_to_scene_tx = _item_to_scene_tx_map.value(item);
    QTransform scene_to_item_tx = item_to_scene_tx.inverted();

    QPolygonF item_plgn = scene_to_item_tx.map(on_scene_plgn);
    QPolygonF init_plgn = scene_to_item_tx.map(_initial_scene_plgn);

    QTransform preview_tx;
    if (QTransform::quadToQuad(init_plgn, item_plgn, preview_tx))
    {
      item->setTransform(preview_tx*item_tx);
    }
  }

  return true;
}


