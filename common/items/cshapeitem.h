#ifndef CSHAPEITEM_H
#define CSHAPEITEM_H


#include <QGraphicsItem>

#include "citems.h"
#include "cshape.h"
#include "coglpainterthread.h"

using namespace sm;

class CShapeItem : public QGraphicsObject
{
  Q_OBJECT

public:
  CShapeItem(const CShape&);

  int type() const override
  {
    return ShapeItem;
  }

  CMask mask() const
  {
    return _shape.mask();
  }

  CShape shapeObject() const
  {
    return _shape;
  }

  void startStrokesRender();
  void startStrokesRender(const CStrokesTemplate&);

  CGraphics element() const;

  void hoverLeave();

//  bool mouseSceneClickEvent(QGraphicsSceneMouseEvent *event);

protected:
  QRectF boundingRect() const override;
  QPainterPath shape() const override;

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

  QVariant itemChange(GraphicsItemChange , const QVariant &) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

  bool contains(const QPointF &point) const override;

private:
  CShape _shape;

  QMap<ObjId, COGLPainterData*> _painter_data;

  struct CacheRec
  {
    QImage pixmap;
    QRectF pixmap_rect_on_item;
    int data_version;
  };

  typedef QPair<ObjId, QWidget*> CacheKey;
  QMap<CacheKey, CacheRec> _cache_map;

  CGraphics _hovered_tmpl;

  int _strip=0;

  QTimer _update_timer;

  QMap<ObjId, int> _data_version;

  TxSelectionRec _tx_rec;

  void _create_painting_task(QWidget*, const CStrokesTemplate&);

  QRectF _get_united_view_on_item_r(QGraphicsView * view, QWidget * viewport);

  QRectF _get_pixmap_r(const QRectF &united_view_on_item_r, const QTransform&);

  void _paint_tmpl(const CStrokesTemplate&, QPainter *, QWidget *, bool&);

  bool _hit_to_tmpl(const QPointF &item_p, const CacheKey &key) const;

  void _disable_shape_events();

  void _enable_shape_events();

private slots:
  void _transform_changed();

  void _mask_changed();

  void _knife_edges_created(int, StrokesTemplateData*);

  void _strokes_created(int, COGLPainterDataSharedPtr);

  void _painter_ready(int, const PainterThreadTask&);

  void _selected_changed(bool);

  void _update();

  void _data_changed(const CGraphics&);


signals:
  void transformStarted(const QList<CGraphics>&);
  void transformFinished(const QString&);

};

#endif // CSHAPEITEM_H
