#ifndef CITEMS_H
#define CITEMS_H

#include <QGraphicsItem>

#define Z_VALUE_SELECTION_FRAME  100001
#define Z_VALUE_DOCUMENT_ITEM         1
#define Z_VALUE_PIXMAP_ITEM           2
#define Z_VALUE_ENGRAVING_ITEM     1000
#define Z_VALUE_CONTROLS           5000

#define PROP_DSBL_TEST_HIT_ITEMS   "dsbl_test_hit_items"

#define NATIVE_PAINTING_

enum CItemsTypes
{
  EngravingItem  = QGraphicsItem::UserType + 1,
  DocumentItem,
  TransformItem,
  ShapeItem,
  GroupItem

};

typedef QMap<QGraphicsItem*, QPair<QTransform, QTransform>> TxSelectionMap;
struct TxSelectionRec
{
  TxSelectionMap selection_map;
  QPointF scene_begin_pos;
  bool has_offset=false;
};



#endif // CITEMS_H
