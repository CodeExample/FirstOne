#ifndef OBJECTSTREEMODEL_H
#define OBJECTSTREEMODEL_H


#include "cdocument.h"
#include "clayer.h"
#include "cgroup.h"
#include "cshape.h"

#include <QModelIndexList>
#include <QItemSelection>
#include <QIcon>
#include <QFont>

using namespace sm;

enum IconRole
{
  ItemIcons = 0,
  ActiveLayerIcons,
  VisibleIcons,
  LockIcons
};


enum TreeModelIcons
{
  IconBmp = 1,
  IconTif,
  IconEps,
  IconHyp,
  IconTable,

  IconDsblBmp,
  IconDsblTif,
  IconDsblEps,
  IconDsblHyp,
  IconDsblTable,

  IconLayer,
  IconDsblLayer,

  IconRender,
  IconDsblRender,

  IconMask,
  IconDsblMask,

  IconProject,
  IconDocument,
  IconRoot,

  IconVisible,
  IconHide,

  IconLocked,
  IconUnlocked,

  IconActiveLayer

};


class ObjectsTreeItem
{
  friend class ObjectsTreeModel;

public:
  static ObjectsTreeItem * createItem(const CGraphics& object, ObjectsTreeItem *parent);
  static ObjectsTreeItem * itemAt(const CGraphics& object);

  static void clearReleased();

  ObjectsTreeItem(const CGraphics& object, ObjectsTreeItem *parent);

  ~ObjectsTreeItem();

  void appendChild(ObjectsTreeItem *item);

  void removeChild(ObjectsTreeItem *item);

  ObjectsTreeItem * child(int row);

  QList<ObjectsTreeItem*> childs() const;

  int childsCount() const;

  ObjectsTreeItem *parent();

  int row() const;

  CGraphics object() const;

  void clearChilds();



private:
  ObjectsTreeItem * _parent=0;
  CGraphics _object;
  QList<ObjectsTreeItem*> _childs;

  static QMap<int, ObjectsTreeItem*> _object_items;

};


enum TreeModelDataRole
{
  roleSelected = Qt::UserRole + 1,
  roleObjectId = Qt::UserRole + 2,
};


class ObjectsTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  ObjectsTreeModel(QObject *parent = 0);

  QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  static QFont getFont(const CGraphics&);
  static QIcon getIcon(const CGraphics&, IconRole);

  QItemSelection& selection() const;

  ObjectsTreeItem * rootItem() const
  {
    return _root_item;
  }

  ObjectsTreeItem * itemAt(const CGraphics &object) const
  {
    return ObjectsTreeItem::_object_items.value(object.id());
  }


  QModelIndexList getPersistentIndexList() const
  {
    return this->persistentIndexList();
  }

  void createModel(const CDocument &);

  void createObjectItem(const CGraphics&,  ObjectsTreeItem*);

  void createLayerItem(const CLayer&, ObjectsTreeItem*, bool);
  void createGroupItem(const CGroup&, ObjectsTreeItem*);
  void createElementItem(const CShape&, ObjectsTreeItem*);
  void createTemplateItem(const CStrokesTemplate&, ObjectsTreeItem*);
  void removeObjectItem(const CGraphics&, ObjectsTreeItem*);

  void addObjectTo(const CGraphics&, const CGraphics&);
  void removeObjectFrom(const CGraphics&, const CGraphics&);

private:
  static bool _initialized;
  static QMap<int, QIcon> _icons;

  static void initStatic();

  ObjectsTreeItem * _root_item;
  QItemSelection _selection;
  CLayer _active_layer;

  bool findModelIndex(const CGraphics&, int, int, ObjectsTreeItem*, QModelIndex&);

private slots:
  void _document_changed(CDocument);

  void _layer_activated(CLayer layer);

  void _selected_changed(bool);
};

#endif // OBJECTSTREEMODEL_H
