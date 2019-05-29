#ifndef CITEMSCONTROLLER_H
#define CITEMSCONTROLLER_H

#include <QGraphicsScene>
#include <QUndoCommand>

#include "cdocument.h"

#define itmc CItemsController::instance()

namespace sm {

#define Z_DOC      100

typedef QMap<ObjId, QGraphicsItem*> ItemsMap;

class CItemsController : public QObject
{
  Q_OBJECT
public:
  static CItemsController * instance()
  {
    if (!_instance)
      _instance = new CItemsController();

    return _instance;
  }

  void init(QGraphicsScene *, QGraphicsView *);

private:
  static CItemsController * _instance;
  CItemsController();
  QGraphicsScene * _scene;
  QGraphicsView * _graphics_view;
  ItemsMap _items_map;
  QUndoCommand *_current_command=0;

  void _check_document_item(const CDocument &);
  QGraphicsItem * _create_object_item(const CGraphics &);
  QGraphicsItem * _check_object_item(const CGraphics &);

private slots:
  void _document_changed(const CDocument &);
  void _object_added(CGraphics, CGraphics);
  void _object_removed(CGraphics, CGraphics);
  void _transform_finished(const QString &);
  void _transform_started(const QList<CGraphics> &);

};

}

#endif // CITEMSCONTROLLER_H
