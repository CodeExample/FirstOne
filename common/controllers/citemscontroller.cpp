#include "citemscontroller.h"

#include "cdocumentitem.h"
#include "cprojecthandler.h"
#include "cgroupitem.h"
#include "cshapeitem.h"
#include "ccommands.h"

using namespace sm;

CItemsController * CItemsController::_instance=nullptr;

CItemsController::CItemsController()
{
}


void CItemsController::init(QGraphicsScene * scene, QGraphicsView * view)
{
  _scene = scene;
  _graphics_view = view;

  connect(hprj, SIGNAL(documentChanged(CDocument)),
          this, SLOT(_document_changed(CDocument)));
}


void CItemsController::_document_changed(const CDocument &doc)
{
  _scene->clear();
  _check_document_item(doc);
}



void CItemsController::_object_removed(CGraphics object, CGraphics)
{
  for(const CGraphics &o : CGroup(object).objects())
  {
    _object_removed(o, object);
  }
  ObjId object_id = object.id();
  if (_items_map.contains(object_id))
  {
    QGraphicsItem * object_item = static_cast<QGraphicsItem*>(_items_map.value(object_id));
    _items_map.remove(object.id());
    delete object_item;
  }
}


void CItemsController::_object_added(CGraphics object, CGraphics dest)
{
  QGraphicsItem * object_item = _check_object_item(object);
  QGraphicsItem * dest_item = _check_object_item(dest);
  object_item->setParentItem(dest_item);
}




void CItemsController::_check_document_item(const CDocument &document)
{
  ObjId id = document.id();
  CDocumentItem * item = static_cast<CDocumentItem*>(_items_map.contains(id) ? _items_map.value(id) : 0);
  if (!item)
  {
    item = new CDocumentItem(document);
    item->setData(DATA_KEY_OBJ_ID, id);
    item->setZValue(Z_VALUE_DOCUMENT_ITEM);

    _items_map.insert(id, item);

    _scene->addItem(item);
    double w = document.boundingRect().width()/10;
    double h = document.boundingRect().height()/10;
    QRectF view_r = document.boundingRect().adjusted(-w,-h,w,h);
    _graphics_view->setSceneRect(view_r);
    _graphics_view->fitInView(view_r, Qt::KeepAspectRatio);
  }

  item->setZValue(Z_VALUE_DOCUMENT_ITEM);
}




QGraphicsItem * CItemsController::_create_object_item(const CGraphics &object)
{
  QGraphicsItem * item = nullptr;

  ObjId object_id = object.id();

  switch(object.type())
  {
  case Group:
    item = new CGroupItem(object);
    break;

  case Shape:
    item = new CShapeItem(object);
    connect(static_cast<CShapeItem*>(item), SIGNAL(transformStarted(QList<CGraphics>)),
            this, SLOT(_transform_started(QList<CGraphics>)));

    connect(static_cast<CShapeItem*>(item), SIGNAL(transformFinished(QString)),
            this, SLOT(_transform_finished(const QString&)));

    static_cast<CShapeItem*>(item)->startStrokesRender();
    break;

  default:;
  }

  if (item)
  {
    item->setData(DATA_KEY_OBJ_ID, object_id);

    _items_map.insert(object_id, item);

    _scene->addItem(item);
  }

  return item;
}



QGraphicsItem * CItemsController::_check_object_item(const CGraphics &object)
{
  ObjId elmt_id = object.id();
  QGraphicsItem * item = static_cast<QGraphicsItem*>(_items_map.contains(elmt_id) ? _items_map.value(elmt_id) : nullptr);

  if (!item)
  {
    item = _create_object_item(object);
  }

  for(const CGraphics &child : object.objects())
  {
    QGraphicsItem * child_item = _check_object_item(child);
    if (item)
      child_item->setParentItem(item);
  }

  return item;
}


void CItemsController::_transform_finished(const QString &caption)
{
  if (_current_command)
  {
    if (caption != "")
      _current_command->setText(caption);

    hprj->execute(_current_command);
    _current_command = nullptr;
  }
}


void CItemsController::_transform_started(const QList<CGraphics> &objects)
{
  if (objects.count() > 0)
  {
    _current_command = new commSaveState("Edit", objects);
  } else {
    _current_command = nullptr;
  }
}

