#include "objectstreemodel.h"

#include "cprojecthandler.h"
#include "cmodel.h"

#define TREE_COLUMNS_COUNT  1


using namespace sm;

/**
  *
  *
  * TreeItem
  *
  *
  */
QMap<int, ObjectsTreeItem*> ObjectsTreeItem::_object_items;


ObjectsTreeItem * ObjectsTreeItem::createItem(const CGraphics& object, ObjectsTreeItem *parent)
{
  uint object_id = object.id();

  ObjectsTreeItem * tree_item=0;

  if (_object_items.contains(object_id))
  {
    tree_item = _object_items.value(object_id);
    tree_item->clearChilds();
    if (tree_item->parent() != parent)
    {
      tree_item = 0;
    }
  }

  if (!tree_item)
  {
    tree_item = new ObjectsTreeItem(object, parent);
    _object_items.insert(object_id, tree_item);
  }

  return tree_item;
}

ObjectsTreeItem * ObjectsTreeItem::itemAt(const CGraphics& object)
{
  return _object_items.contains(object.id()) ? _object_items.value(object.id()) : nullptr;
}


ObjectsTreeItem::ObjectsTreeItem(const CGraphics& object, ObjectsTreeItem *parent) :
  _parent(parent), _object(object)
{
  _object_items.insert(object.id(), this);
}


ObjectsTreeItem::~ObjectsTreeItem()
{
  _object_items.remove(_object.id());
  qDeleteAll(_childs);
}


void ObjectsTreeItem::appendChild(ObjectsTreeItem *item)
{
  _childs.append(item);
}

void ObjectsTreeItem::removeChild(ObjectsTreeItem *item)
{
  _childs.removeOne(item);
}


ObjectsTreeItem *ObjectsTreeItem::child(int row)
{
  return _childs.value(row);
}


int ObjectsTreeItem::childsCount() const
{
  return _childs.count();
}


ObjectsTreeItem *ObjectsTreeItem::parent()
{
  return _parent;
}


int ObjectsTreeItem::row() const
{
  if (_parent)
    return _parent->_childs.indexOf(const_cast<ObjectsTreeItem*>(this));

  return 0;
}

void ObjectsTreeItem::clearChilds()
{
  _childs.clear();
}


QList<ObjectsTreeItem*> ObjectsTreeItem::childs() const
{
  return _childs;
}


CGraphics ObjectsTreeItem::object() const
{
  return _object;
}


/**
  *
  *
  * DocumentTreeModel
  *
  *
  */

ObjectsTreeModel::ObjectsTreeModel(QObject * parent)
  : QAbstractItemModel(parent), _root_item(0)
{
  initStatic();

//  connect(vxEnvironment::instance(), SIGNAL(projectChanged()), this, SLOT(_project_changed()));

  connect(hprj, SIGNAL(documentChanged(CDocument)), this, SLOT(_document_changed(CDocument)));
  connect(hprj, SIGNAL(layerActivated(CLayer)), this, SLOT(_layer_activated(CLayer)));
}


/*
void ObjectsTreeModel::_project_changed()
{
  _update_model();

  emit layoutAboutToBeChanged();
  emit layoutChanged();
}
*/


int ObjectsTreeModel::columnCount(const QModelIndex &) const
{
  return 4;
}


QFont ObjectsTreeModel::getFont(const CGraphics& object)
{
  switch(object.type())
  {
  case Document:
    return QFont("Arial", 12, 75);

  case Layer:
    return QFont("Arial", 12);

  case Group:
    return QFont("Arial", 11);

  case Shape:
    return QFont("Arial", 10);

  case Template:
    return QFont("Arial", 10, -1, true);

  default:;
  }

  return QFont();
}


bool ObjectsTreeModel::_initialized = false;

QMap<int, QIcon> ObjectsTreeModel::_icons;

void ObjectsTreeModel::initStatic()
{
  if (_initialized)
    return;

  _initialized = true;

/*  _icons.insert(IconBmp, QIcon(":/images/bmp.png"));
  _icons.insert(IconTif, QIcon(":/images/tiff.png"));
  _icons.insert(IconEps, QIcon(":/images/eps.png"));
  _icons.insert(IconHyp, QIcon(":/images/hyp.png"));
  _icons.insert(IconTable, QIcon(":/images/table.png"));

  _icons.insert(IconDsblBmp, QIcon(":/images/bmp_dsbl.png"));
  _icons.insert(IconDsblTif, QIcon(":/images/tiff_dsbl.png"));
  _icons.insert(IconDsblEps, QIcon(":/images/eps_dsbl.png"));
  _icons.insert(IconDsblHyp, QIcon(":/images/hyp_dsbl.png"));
  _icons.insert(IconDsblTable, QIcon(":/images/table_dsbl.png"));

  _icons.insert(IconMask, QIcon(":/images/mask.png"));

  _icons.insert(IconProject, QIcon(":/images/project_star.png"));
  _icons.insert(IconDocument, QIcon(":/images/document.png"));
  _icons.insert(IconRoot, QIcon(":/images/database.png"));

  _icons.insert(IconRender, QIcon(":/images/hammer.png"));
  _icons.insert(IconDsblRender, QIcon(":/images/hammer_dsbl.png"));

  _icons.insert(IconMask, QIcon(":/images/layer_shade.png"));
  _icons.insert(IconDsblMask, QIcon(":/images/mask_dsbl.png"));

  _icons.insert(IconVisible, QIcon(":/images/eye.png"));
  _icons.insert(IconHide, QIcon(":/images/eye_off.png"));

  _icons.insert(IconLocked, QIcon(":/images/lock.png"));
  _icons.insert(IconUnlocked, QIcon(":/images/lock_open.png"));
*/

  _icons.insert(IconLayer, QIcon(":/images/resources/images/tree_layer.png"));
  _icons.insert(IconDsblLayer, QIcon(":/images/resources/images/tree_layer_dsbl.png"));
  _icons.insert(IconActiveLayer, QIcon(":/images/resources/images/tree_layer_active.png"));


}


QIcon ObjectsTreeModel::getIcon(const CGraphics& object, IconRole icon_role)
{
  if (!object)
    return QIcon();

  //bool locked = object.isLocked();

  //bool is_ready=true;
  //if (object.objectType() == BigImage)
  //{
  //  vxBigImage image = object;
  //  is_ready = image.imageIsReady();
 // }

  if (icon_role == ItemIcons)
  {
    switch(object.type())
    {

    case Document:
      return _icons.value(IconDocument);

    case Layer:
    {
      CLayer layer = object;

      if (hprj->document().activeLayer() == layer)
        return _icons.value(IconActiveLayer);

      return _icons.value(object.isLocked() ? IconDsblLayer : IconLayer);
    }

    default:;
    }
  }

  return QIcon();

}


bool ObjectsTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  switch(index.column())
  {
  case 0:
  {
    switch(role)
    {
    case Qt::EditRole:
    {
      ObjectsTreeItem * tree_item = static_cast<ObjectsTreeItem*>(index.internalPointer());
      CGraphics object = tree_item->object();
      object.setName(value.toString());
      emit dataChanged(index, index);
      return true;
    }

    case roleSelected:
    {
      ObjectsTreeItem * item = static_cast<ObjectsTreeItem*>(index.internalPointer());
      CGraphics object = item->object();
      hprj->select(object);
      break;
    }


    case roleObjectId:
      break;

    default:;
    }
  }

  default:;
  }

  return QAbstractItemModel::setData(index, value, role);
}


class SelectedFinder : public IVisitor
{
public:
  bool visitTo(const CGraphics &) override
  {
    return false;
  }
  QList<CGraphics> selected;
};


QVariant ObjectsTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  ObjectsTreeItem * item = static_cast<ObjectsTreeItem*>(index.internalPointer());

  CGraphics object = item->object();

  switch(index.column())
  {
  case 0:
  {
    switch(role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
      if (object.name() != "")
        return object.name();

      return QString("%1 %2").arg(CModel::typeToStr(object.type())).arg(object.id());
    }

    case Qt::FontRole:
      return getFont(object);

    case Qt::DecorationRole:
      return getIcon(object, ItemIcons);

    case roleSelected:
      return object.isSelected();

    case roleObjectId:
      return object.id();

    //case Qt::TextColorRole:
    //  return QVariant::fromValue(QColor(215,215,215));
    }
  }

  default:;
  }


  switch(index.column())
  {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  {
    switch(role)
    {
    case roleSelected:
      return object.isSelected();

      default:;
    }
  }

  default:;
  }


  return QVariant();
}


Qt::ItemFlags ObjectsTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  ObjectsTreeItem * tree_item = static_cast<ObjectsTreeItem*>(index.internalPointer());
  CGraphics object = tree_item->object();

  if (object && object.type() == Layer && index.column() == 0)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;


  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant ObjectsTreeModel::headerData(int, Qt::Orientation, int) const
{
  return QVariant();
}


QModelIndex ObjectsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  ObjectsTreeItem * parent_item;

  if (!parent.isValid())
    parent_item = _root_item;
  else
    parent_item = static_cast<ObjectsTreeItem*>(parent.internalPointer());

  ObjectsTreeItem *child_item = parent_item->child(row);

  if (!child_item)
    return QModelIndex();

  return createIndex(row, column, child_item);
}



QModelIndex ObjectsTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  ObjectsTreeItem *child_item = static_cast<ObjectsTreeItem*>(index.internalPointer());
  ObjectsTreeItem *parent_item = child_item->parent();

  if (parent_item == _root_item)
    return QModelIndex();

  return createIndex(parent_item->row(), 0, parent_item);
}


int ObjectsTreeModel::rowCount(const QModelIndex &index) const
{
  ObjectsTreeItem * tree_item;

  if (!index.isValid())
    tree_item = _root_item;
  else
    tree_item = static_cast<ObjectsTreeItem*>(index.internalPointer());

  return tree_item ? tree_item->childsCount() : 0;
}


void ObjectsTreeModel::_document_changed(CDocument doc)
{
  createModel(doc);
}


void ObjectsTreeModel::createModel(const CDocument &doc)
{
  if (_root_item)
  {
    beginResetModel();
    delete _root_item;
    endResetModel();
  }
  _root_item = ObjectsTreeItem::createItem(doc.root(), 0);

  createLayerItem(doc.root(), _root_item, false);
}


void ObjectsTreeModel::createObjectItem(const CGraphics &object,  ObjectsTreeItem * parent)
{
  switch(object.type())
  {
  case Layer:
    createLayerItem(object, parent, true);
    break;
  case Group:
    createGroupItem(object, parent);
    break;
  case Shape:
    createElementItem(object, parent);
    break;
  case Template:
    createTemplateItem(object, parent);
    break;
  default:;
  }
}


void ObjectsTreeModel::removeObjectItem(const CGraphics &object,  ObjectsTreeItem * parent)
{
  ObjectsTreeItem * item = ObjectsTreeItem::_object_items.value(object.id());
  if (item)
  {
    parent->removeChild(item);
  }
}



void ObjectsTreeModel::createLayerItem(const CLayer &layer, ObjectsTreeItem * parent_item, bool create_layer_item)
{
  ObjectsTreeItem * layer_item = parent_item;
  if (create_layer_item)
  {
    layer_item = ObjectsTreeItem::createItem(layer, parent_item);
    parent_item->appendChild(layer_item);
  }

  for(const CGraphics &object : layer.objects())
  {
    createObjectItem(object, layer_item);
  }
}


void ObjectsTreeModel::createElementItem(const CShape &element, ObjectsTreeItem *parent_item)
{
  ObjectsTreeItem * element_item = ObjectsTreeItem::createItem(element, parent_item);
  parent_item->appendChild(element_item);
  for(const CStrokesTemplate &tmpl : element.templates())
  {
    createTemplateItem(tmpl, element_item);
  }
}


void ObjectsTreeModel::createGroupItem(const CGroup &group, ObjectsTreeItem *parent_item)
{
  ObjectsTreeItem * group_item = ObjectsTreeItem::createItem(group, parent_item);
  parent_item->appendChild(group_item);
  for(const CGraphics &object : group.objects())
  {
    createObjectItem(object, group_item);
  }
}



void ObjectsTreeModel::createTemplateItem(const CStrokesTemplate &tmpl, ObjectsTreeItem *parent_item)
{
  ObjectsTreeItem * element_item = ObjectsTreeItem::createItem(tmpl, parent_item);
  parent_item->appendChild(element_item);

  connect(tmpl.notifier(), SIGNAL(selectedChanged(bool)), this, SLOT(_selected_changed(bool)));
}


void ObjectsTreeModel::removeObjectFrom(const CGraphics &object, const CGraphics &target)
{
  QModelIndex target_index;
  if (findModelIndex(target, 0,0, _root_item, target_index))
  {
    ObjectsTreeItem * target_item = static_cast<ObjectsTreeItem*>(target_index.internalPointer());
    if (target_item)
    {
      beginRemoveRows(target_index, 0, target_item->childsCount());
      removeObjectItem(object, target_item);
      endRemoveRows();
    } else {
      beginRemoveRows(QModelIndex(), 0, _root_item->childsCount());
      removeObjectItem(object, _root_item);
      endRemoveRows();
    }
  }
}


void ObjectsTreeModel::addObjectTo(const CGraphics &object, const CGraphics &target)
{
  QModelIndex target_index;
  if (findModelIndex(target, 0,0, _root_item, target_index))
  {
    ObjectsTreeItem * target_item = static_cast<ObjectsTreeItem*>(target_index.internalPointer());
    if (target_item)
    {
      beginInsertRows(target_index, target_item->childsCount(), target_item->childsCount());
      createObjectItem(object, target_item);
      endInsertRows();
    } else {
      beginInsertRows(QModelIndex(), _root_item->childsCount(), _root_item->childsCount());
      createObjectItem(object, _root_item);
      endInsertRows();
    }
  }

}


bool ObjectsTreeModel::findModelIndex(const CGraphics &object, int row, int col, ObjectsTreeItem *tree_item, QModelIndex &object_index)
{
  if (object == _root_item->object())
  {
    object_index = QModelIndex();
    return true;
  }

  if (tree_item->object() == object)
  {
     object_index = createIndex(row, col, tree_item);
     return true;
  }

  for(int i=0; i < tree_item->childsCount(); ++i)
  {
    if (findModelIndex(object, i, 0, tree_item->child(i), object_index))
      return true;
  }
  return false;
}


void ObjectsTreeModel::_layer_activated(CLayer )
{
  emit layoutChanged();
}


void ObjectsTreeModel::_selected_changed(bool)
{
}


