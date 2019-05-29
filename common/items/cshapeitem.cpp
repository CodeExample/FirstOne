#include "cshapeitem.h"

#include "crendermanager.h"
#include "ccommands.h"
#include "cprojecthandler.h"

#include <QPainter>

using namespace sm;

CShapeItem::CShapeItem(const CShape &shape) : _shape(shape)
{
  setAcceptHoverEvents(true);

  setFlags(ItemSendsGeometryChanges | ItemIsSelectable | ItemIsMovable);

  setSelected(_shape.isSelected());

  setTransform(shape.transform());

  for(const CGraphics &tmpl : _shape.templates())
  {
    connect(tmpl.notifier(), SIGNAL(selectedChanged(bool)),
            this, SLOT(_selected_changed(bool)));

    connect(tmpl.notifier(), SIGNAL(dataChanged(CGraphics)),
            this, SLOT(_data_changed(const CGraphics&)));
  }

  connect(CRenderManager::instance(), SIGNAL(strokesCreated(int, COGLPainterDataSharedPtr)),
          this, SLOT(_strokes_created(int, COGLPainterDataSharedPtr)));

  connect(CRenderManager::instance(), SIGNAL(painterReady(int, PainterThreadTask)),
          this, SLOT(_painter_ready(int, PainterThreadTask)));

  _enable_shape_events();

  _update_timer.setInterval(100);
  _update_timer.setSingleShot(true);

  connect(&_update_timer, SIGNAL(timeout()), this, SLOT(_update()));

}


void CShapeItem::_disable_shape_events()
{
  disconnect(_shape.notifier(), SIGNAL(selectedChanged(bool)), this, SLOT(_selected_changed(bool)));
  disconnect(_shape.notifier(), SIGNAL(transformChanged()), this, SLOT(_transform_changed()));
  if (_shape.mask())
    disconnect(_shape.mask().notifier(), SIGNAL(maskChanged()), this, SLOT(_mask_changed()));
}


void CShapeItem::_enable_shape_events()
{
  connect(_shape.notifier(), SIGNAL(selectedChanged(bool)), this, SLOT(_selected_changed(bool)));
  connect(_shape.notifier(), SIGNAL(transformChanged()), this, SLOT(_transform_changed()));
  if (_shape.mask())
    connect(_shape.mask().notifier(), SIGNAL(maskChanged()), this, SLOT(_mask_changed()));
}


void CShapeItem::_update()
{
  _update_timer.stop();
  update();
}


bool CShapeItem::contains(const QPointF &item_p) const
{
  QWidget * w = hprj->workspaceWidget();
  for(const CStrokesTemplate &tmpl : _shape.templates())
  {
    CacheKey key(tmpl.id(), w);
    if (_hit_to_tmpl(item_p, key))
      return true;
  }

  if (!_shape.mask())
    return false;

  return _shape.mask().path().contains(item_p);
}




void CShapeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mousePressEvent(event);

  QSet<QGraphicsItem*> items_set = scene()->selectedItems().toSet();
  items_set |= this;
  _tx_rec = hprj->beginDragSelection(event->scenePos(), items_set.toList());
}


void CShapeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  hprj->dragSelection(event->scenePos(), _tx_rec);
}


void CShapeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mouseReleaseEvent(event);
  hprj->endDragSelection(_tx_rec);
}




bool CShapeItem::_hit_to_tmpl(const QPointF &, const CacheKey &) const
{
  /*const CacheRec &cr = _cache_map.value(key);
  if (!cr.pixmap_rect_on_item.contains(item_p))
    return false;

  QPolygonF one = cr.pixmap_rect_on_item; one.pop_back();
  QPolygonF two = QRectF(cr.pixmap.rect()); two.pop_back();
  QTransform item_to_pixmap_tx;
  if (!QTransform::quadToQuad(one, two, item_to_pixmap_tx))
    return false;

  QPoint pixmap_p = item_to_pixmap_tx.map(item_p).toPoint();
  if (!cr.pixmap.rect().contains(pixmap_p))
    return false;

  for(int y=pixmap_p.y() - 5; y < pixmap_p.y() + 5; ++y)
  {
    for(int x=pixmap_p.x() - 5; x < pixmap_p.x() + 5; ++x)
    {
      uint px = cr.pixmap.pixel(x, y);

      if (px != 0)
        return true;
    }
  }*/

  return false;
}


/*
void CShapeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  //if (!_hovered_tmpl)
  //{
  //  event->ignore();
  //  return;
  //}

  _mouse_pressed = true;
  _mouse_move = false;
  QGraphicsObject::mousePressEvent(event);
}



void CShapeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (_mouse_pressed && !_mouse_move)
  {
    _mouse_move = true;
    emit transformStarted(QList<CGraphics>() << _shape.mask());
  }
  QGraphicsObject::mouseMoveEvent(event);
}

bool CShapeItem::mouseSceneClickEvent(QGraphicsSceneMouseEvent *event)
{
  bool accepted = false;
  if (_hovered_tmpl)
  {
    if ((~event->modifiers() & Qt::ControlModifier) &&
        (~event->modifiers() & Qt::ShiftModifier))
    {
      for(const CStrokesTemplate &tmpl : _shape.templates())
      {
        tmpl.setSelected(false);
      }
    }

    _hovered_tmpl.setSelected(!_hovered_tmpl.isSelected());
    accepted = true;
  }
  return accepted;
}


void CShapeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  bool accepted=false;
  if (_mouse_pressed)
  {
    if(_mouse_move)
    {
      emit transformFinished("Move");
    } else {
      accepted |= mouseSceneClickEvent(event);
    }
  }

  _mouse_pressed = false;
  _mouse_move = false;

  if (accepted)
    emit hprj->selectionChanged();
  else
    QGraphicsObject::mouseReleaseEvent(event);
}
*/



QVariant CShapeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  switch(change)
  {
  case ItemPositionHasChanged:
  case ItemTransformHasChanged:
  {
    _disable_shape_events();
    {
      QTransform tx;
      tx.translate(pos().x(), pos().y());
      _shape.setTransform(transform()*tx);
    }
    _enable_shape_events();
    startStrokesRender();
    break;
  }
  case ItemSelectedChange:
  {
    _shape.setSelected(value.toBool());
    if (!value.toBool())
    {
      for(const CStrokesTemplate &tmpl : _shape.templates())
      {
        tmpl.setSelected(false);
      }
    }
    break;
  }
  default:;
  }

  return value;
}



QRectF CShapeItem::boundingRect() const
{
  return _shape.mask().boundingRect();
}


QPainterPath CShapeItem::shape() const
{
  return _shape.mask().path();
}




void CShapeItem::_transform_changed()
{
  if (transform() != _shape.transform())
  {
    setTransform(_shape.transform());

    _cache_map.clear();

    update();

    startStrokesRender();
  }
}




void CShapeItem::_selected_changed(bool)
{
  bool s=false;
  if (_shape.isSelected())
  {
    s = true;
  } else {
    for(const CStrokesTemplate &tmpl : _shape.templates())
    {
      s |= tmpl.isSelected();
      if (s)
        break;
    }
  }
  setSelected(s);
  update();
}



void CShapeItem::_mask_changed()
{
  _cache_map.clear();

  update();

  startStrokesRender();
}



void CShapeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *viewport)
{
  painter->save();

  bool need_update_caches=false;
  for(const CStrokesTemplate &tmpl : _shape.templates())
  {
    _paint_tmpl(tmpl, painter, viewport, need_update_caches);
  }

  if (need_update_caches)
  {
    for(const CStrokesTemplate &tmpl : _shape.templates())
    {
      _create_painting_task(viewport, tmpl);
    }
  }


  if (_shape.mask())
  {
    if (isSelected())
    {
      QPen pen(Qt::red, 1, Qt::DotLine);
      pen.setCosmetic(true);
      painter->setPen(pen);
      painter->setBrush(Qt::NoBrush);
      QPainterPath mask_path = _shape.mask().path();
      painter->drawPath(mask_path);

      for(int i=0; i < mask_path.elementCount(); ++i)
      {
        QPainterPath::Element e = mask_path.elementAt(i);
        painter->save();

        QTransform painter_tx = painter->transform();
        painter->resetTransform();

        QPointF scr_p = painter_tx.map(QPointF(e.x,e.y));

        QPen pen(Qt::red, 1);
        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        switch(e.type)
        {
        case QPainterPath::MoveToElement:
          painter->drawEllipse(scr_p, 3,3);
          break;

        case QPainterPath::LineToElement:
          painter->drawEllipse(scr_p, 3,3);
          break;

        case QPainterPath::CurveToElement:
          painter->drawEllipse(scr_p, 3,3);
          break;

        case QPainterPath::CurveToDataElement:
          painter->drawEllipse(scr_p, 1.5,1.5);
          break;
        }

        painter->restore();
      }

    } else {
      QPen pen(Qt::red, 1, Qt::DotLine);
      pen.setCosmetic(true);
      painter->setOpacity(0.6);
      painter->setPen(pen);
      painter->setBrush(Qt::NoBrush);
      painter->drawPath(_shape.mask().path());
    }
  }

  painter->restore();

}



void CShapeItem::_paint_tmpl(const CStrokesTemplate &tmpl, QPainter *painter,
                             QWidget *viewport, bool &need_update_caches)
{
  QGraphicsView * view = static_cast<QGraphicsView*>(viewport->parent());

  ObjId tmpl_id = tmpl.id();
  CacheKey key(tmpl_id, viewport);

  if (_cache_map.contains(key))
  {
    QGLShaderProgram * program = 0;

    painter->save();

    CacheRec cache_rec = _cache_map.value(key);

    painter->beginNativePainting();

    if (tmpl == _hovered_tmpl || tmpl.isSelected())
    {

      double dx = (tmpl.isSelected() ? 2 : 1.2)/cache_rec.pixmap.width();
      double dy = (tmpl.isSelected() ? 2 : 1.2)/cache_rec.pixmap.height();

      program = new QGLShaderProgram();

      program->addShaderFromSourceCode(QGLShader::Vertex,
                                       "varying vec2 vTexCoord;"
                                       "void main(void)"
                                       "{"
                                       "  vTexCoord = gl_MultiTexCoord0.xy;"
                                       "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                                       "}");

      program->addShaderFromSourceCode(QGLShader::Fragment,
                               QString("uniform sampler2D myTexture;"
                                       "varying vec2 vTexCoord;"
                                       "void main(void)"
                                       "{"
                                       "  if((texture2D(myTexture, vTexCoord).a < 1.0) &&"
                                       "     (texture2D(myTexture, vTexCoord - vec2(%1, 0.0)).a == 1.0 ||"
                                       "      texture2D(myTexture, vTexCoord - vec2(0.0, %2)).a == 1.0 ||"
                                       "      texture2D(myTexture, vTexCoord + vec2(%1, 0.0)).a == 1.0 ||"
                                       "      texture2D(myTexture, vTexCoord + vec2(0.0, %2)).a == 1.0))"
                                       "  {"
                                       "    gl_FragColor = vec4(0.8, 0.0, 0.0, 1.0);"
                                       "  } else {"
                                       "    gl_FragColor = texture2D(myTexture, vTexCoord);"
                                       "  }"
                                       "}").arg(dx).arg(dy));
      program->link();
      program->bind();
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, cache_rec.pixmap.width(),  cache_rec.pixmap.height(), 0,
                 GL_BGRA, GL_UNSIGNED_BYTE,  cache_rec.pixmap.scanLine(0));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1,1,1, 1);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    QRectF r = cache_rec.pixmap_rect_on_item;

    glBegin(GL_QUADS);
    {
      glTexCoord2f(0, 1); glVertex2f(r.left(), r.bottom());
      glTexCoord2f(1, 1); glVertex2f(r.right(), r.bottom());
      glTexCoord2f(1, 0); glVertex2f(r.right(), r.top());
      glTexCoord2f(0, 0); glVertex2f(r.left(), r.top());
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texture_id);

    painter->endNativePainting();

    if (program)
    {
      program->release();
      delete program;
    }

    QTransform item_to_viewport_tx = deviceTransform(view->viewportTransform());

    QRectF painted_item_rect = cache_rec.pixmap_rect_on_item & boundingRect();

    QRectF pixmap_r = _get_pixmap_r(painted_item_rect, item_to_viewport_tx);

    QRectF united_view_on_item_r = _get_united_view_on_item_r(view, viewport);

    if (pixmap_r != cache_rec.pixmap.rect() ||
        united_view_on_item_r != painted_item_rect)
    {
      need_update_caches = true;
    }

    painter->restore();
  }


  if (!_cache_map.contains(key) ||
      _cache_map.value(key).data_version != _data_version[tmpl_id])
  {
    need_update_caches = true;
  }

}



void CShapeItem::startStrokesRender()
{
  for(const CStrokesTemplate &tmpl : _shape.templates())
  {
    startStrokesRender(tmpl);
  }
}


void CShapeItem::startStrokesRender(const CStrokesTemplate &tmpl)
{
  tmpl.clearChanges();

  COGLRenderThreadTask render_task;
  render_task.tmpl_data = tmpl.data();
  render_task.tmpl_data->mask_boundingRect = _shape.mask().boundingRect();
  render_task.tmpl_data->mask_transform = _shape.mask().transform();
  render_task.tmpl_data->mask_path = _shape.mask().path();


  if (!_painter_data.contains(tmpl.id()))
    _painter_data.insert(tmpl.id(), new COGLPainterData());

  render_task.painter_data = _painter_data[tmpl.id()];

  CRenderManager::instance()->append(render_task);
}




void CShapeItem::_create_painting_task(QWidget * viewport, const CStrokesTemplate &tmpl)
{
  QGraphicsView * view = static_cast<QGraphicsView*>(viewport->parent());

  QTransform item_to_viewport_tx = deviceTransform(view->viewportTransform());

  QRectF united_view_on_item_r = _get_united_view_on_item_r(view, viewport);

  QRectF pixmap_r = _get_pixmap_r(united_view_on_item_r, item_to_viewport_tx);

  if (pixmap_r.width() < 2 || pixmap_r.height() < 2)
    return;

  QTransform item_to_pixmap_tx;
  QPolygonF one = united_view_on_item_r; one.pop_back();
  QPolygonF two = pixmap_r; two.pop_back();
  QTransform::quadToQuad(one, two, item_to_pixmap_tx);

  ObjId tmpl_id = tmpl.id();

  if (!_painter_data.contains(tmpl_id))
    _painter_data.insert(tmpl_id, new COGLPainterData());

  PainterThreadTask pt;
  pt.viewport = viewport;
  pt.painter_data = _painter_data[tmpl.id()];
  pt.painter_data->tmpl_id = tmpl.id();
  pt.item_to_pixmap_tx = sceneTransform().inverted()*item_to_pixmap_tx;
  pt.item_view_r = pixmap_r.toRect();
  pt.pixmap_rect_on_item = united_view_on_item_r;

  CRenderManager::instance()->append(tmpl.id(), pt);
}


void CShapeItem::_strokes_created(int tmpl_id, COGLPainterDataSharedPtr)
{
  for(const CStrokesTemplate &tmpl : _shape.templates())
  {
    if (tmpl_id == tmpl.id())
    {
      prepareGeometryChange();
      ++_data_version[tmpl_id];
      update();
    }
  }
}


void CShapeItem::_knife_edges_created(int tmpl_id, StrokesTemplateData * tmpl_data )
{
  for(const CStrokesTemplate &tmpl : _shape.templates())
  {
    if (tmpl_id == tmpl.id())
    {
      tmpl.data()->edges_img = tmpl_data->edges_img;
      tmpl.data()->edges_indexes = tmpl_data->edges_indexes;
      tmpl.data()->edges_track = tmpl_data->edges_track;
    }
  }
}


void CShapeItem::_painter_ready(int tmpl_id, const PainterThreadTask &task)
{
  for(const CStrokesTemplate &tmpl : _shape.templates())
  {
    if (tmpl_id == tmpl.id())
    {
      CacheRec cr;
      cr.pixmap = task.pixmap;
      cr.pixmap_rect_on_item = task.pixmap_rect_on_item;

      cr.data_version = _data_version[tmpl_id];

      _cache_map.insert(CacheKey(tmpl_id, task.viewport), cr);
    }
  }
}


QRectF CShapeItem::_get_united_view_on_item_r(QGraphicsView *view, QWidget *viewport)
{
  QTransform item_to_viewport_tx = deviceTransform(view->viewportTransform());
  QRectF item_r_on_view = item_to_viewport_tx.mapRect(boundingRect());
  QRectF item_united_view_r = item_r_on_view & viewport->rect();
  QTransform viewport_to_item_tx = item_to_viewport_tx.inverted();
  QRectF united_view_on_item_r = viewport_to_item_tx.mapRect(item_united_view_r);
  united_view_on_item_r = united_view_on_item_r & boundingRect();
  return united_view_on_item_r;
}


QRectF CShapeItem::_get_pixmap_r(const QRectF &united_view_on_item_r, const QTransform &item_to_viewport_tx)
{
  QLineF height_l = item_to_viewport_tx.map(QLineF(united_view_on_item_r.topLeft(), united_view_on_item_r.bottomLeft()));
  QLineF width_l = item_to_viewport_tx.map(QLineF(united_view_on_item_r.topLeft(), united_view_on_item_r.topRight()));

  int h = round(height_l.length()*qApp->devicePixelRatio());
  int w = round(width_l.length()*qApp->devicePixelRatio());

  return QRectF(0,0,w,h);
}




void CShapeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
  _update_timer.stop();
  _update_timer.start();
}


void CShapeItem::hoverMoveEvent(QGraphicsSceneHoverEvent * se)
{
  CGraphics h_tmpl;
  const QPointF &item_p = se->pos();

  QWidget * w = se->widget();
  for(const CStrokesTemplate &tmpl : _shape.templates())
  {
    CacheKey key(tmpl.id(), w);
    if (_hit_to_tmpl(item_p, key))
    {
      h_tmpl = tmpl;
    }
  }

  if (h_tmpl != _hovered_tmpl)
  {
    _update_timer.stop();
    _hovered_tmpl = h_tmpl;
    _update_timer.start();
    hprj->setHoveredTmpl(h_tmpl);
  }
}


void CShapeItem::hoverLeave()
{
  if (_hovered_tmpl)
  {
    hoverLeaveEvent(0);
  }
}


void CShapeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
  _update_timer.stop();
  if (_hovered_tmpl)
  {
    _hovered_tmpl = cnull;
  }
  _update_timer.start();
}



void CShapeItem::_data_changed(const CGraphics & tmpl)
{
  startStrokesRender(tmpl);
}
