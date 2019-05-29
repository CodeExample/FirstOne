#ifndef CGROUPITEM_H
#define CGROUPITEM_H

#include <QGraphicsObject>

#include "citems.h"
#include "cgroup.h"

using namespace sm;

class CGroupItem : public QGraphicsObject
{
  Q_OBJECT

public:
  CGroupItem(const CGroup&);

  int type() const override
  {
    return GroupItem;
  }

  QRectF boundingRect() const override;

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)  override;

  CGroup groupObject() const
  {
    return _group;
  }

protected:
  QVariant itemChange(QGraphicsItem::GraphicsItemChange, const QVariant &) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  CGroup _group;

  TxSelectionRec _tx_rec;

  void _disable_group_events();
  void _enable_group_events();

private slots:
  void _selected_changed(bool);
  void _transform_changed();

};



#endif // CGROUPITEM_H
