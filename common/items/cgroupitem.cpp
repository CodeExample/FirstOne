#include "cgroupitem.h"

#include "cprojecthandler.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

CGroupItem::CGroupItem(const CGroup &group) : _group(group)
{
  setAcceptHoverEvents(true);

  setFlags(ItemSendsGeometryChanges | ItemIsSelectable | ItemIsMovable);

  setSelected(group.isSelected());

  setTransform(group.transform());

  _enable_group_events();
}


void CGroupItem::_disable_group_events()
{
  disconnect(_group.notifier(), SIGNAL(selectedChanged(bool)), this, SLOT(_selected_changed(bool)));
  disconnect(_group.notifier(), SIGNAL(transformChanged()), this, SLOT(_transform_changed()));
}


void CGroupItem::_enable_group_events()
{
  connect(_group.notifier(), SIGNAL(selectedChanged(bool)), this, SLOT(_selected_changed(bool)));
  connect(_group.notifier(), SIGNAL(transformChanged()), this, SLOT(_transform_changed()));
}


void CGroupItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mousePressEvent(event);

  QSet<QGraphicsItem*> items_set = scene()->selectedItems().toSet();
  items_set |= this;
  _tx_rec = hprj->beginDragSelection(event->scenePos(), items_set.toList());
}


void CGroupItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  hprj->dragSelection(event->scenePos(), _tx_rec);
}


void CGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mouseReleaseEvent(event);
  hprj->endDragSelection(_tx_rec);
}


QVariant CGroupItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  switch(change)
  {
  case ItemPositionHasChanged:
  case ItemTransformHasChanged:
  {
    _disable_group_events();
    {
      QTransform tx;
      tx.translate(pos().x(), pos().y());
      _group.setTransform(transform()*tx);
    }
    _enable_group_events();
    break;
  }
  case ItemSelectedChange:
  {
    _group.setSelected(value.toBool());
    break;
  }
  default:;
  }

  return value;
}




QRectF CGroupItem::boundingRect() const
{
  QRectF brect;
  for(const QGraphicsItem * item : childItems())
  {
    QTransform item_tx;
    item_tx.translate(item->pos().x(), item->pos().y());
    item_tx = item->transform()*item_tx;
    brect |= item_tx.mapRect(item->boundingRect());
  }
  return brect;
}



void CGroupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  QPen pen(Qt::red, isSelected() ? 2 : 1);
  pen.setCosmetic(true);
  painter->setPen(pen);
  painter->setBrush(Qt::NoBrush);
  painter->drawPolygon(boundingRect());
}


void CGroupItem::_selected_changed(bool s)
{
  if (isSelected() != s)
  {
    setSelected(s);
    update();
  }
}


void CGroupItem::_transform_changed()
{
  if (transform() != _group.transform())
  {
    setTransform(_group.transform());

    update();
  }

}


