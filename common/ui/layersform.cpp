#include "layersform.h"
#include "ui_layersform.h"

#include "cprojecthandler.h"
#include "ccommands.h"

#include <QtGui>
#include <QStyledItemDelegate>
#include <QScrollBar>

#define ROLE_ITEM_TYPE    Qt::UserRole + 1

#define ROLE_TYPE_LAYER   Qt::UserRole + 11
#define ROLE_TYPE_IMAGE   Qt::UserRole + 12
#define ROLE_TYPE_RENDER  Qt::UserRole + 13
#define ROLE_TYPE_MASK    Qt::UserRole + 14

#define ROLE_LAYER_ID     Qt::UserRole + 101
#define ROLE_IMAGE_ID     Qt::UserRole + 102
#define ROLE_RENDER_ID    Qt::UserRole + 103
#define ROLE_MASK_ID      Qt::UserRole + 104

#define TEXT_COLUMN       0
#define EXPR_COLUMN       1
#define VIEW_COLUMN       2
#define LOCK_COLUMN       3



class LockItemDelegate : public QStyledItemDelegate
{
public:
  explicit LockItemDelegate(QObject * = 0)
  {
    _img_lock = QImage(":/images/lock.png");
    _img_unlock = QImage(":/images/lock_open.png");
  }

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    QStyledItemDelegate::paint(painter, option, index);

    ObjectsTreeItem * tree_item = static_cast<ObjectsTreeItem*>(index.internalPointer());

    if (!tree_item)
      return;

    CGraphics object = tree_item->object();

    if (object.isLocked())
    {
      painter->drawImage(option.rect.topLeft() + QPoint(0,2), _img_lock);
    } else {
      painter->drawImage(option.rect.topLeft() + QPoint(0,2), _img_unlock);
    }
  }


  QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
  {
    return QSize(20,20);
  }

private:
  QImage _img_lock;
  QImage _img_unlock;

};


class ViewItemDelegate : public QStyledItemDelegate
{
public:
  explicit ViewItemDelegate(QObject * = 0)
  {
    _img_visible = QImage(":/images/eye.png");
    _img_hidden = QImage(":/images/eye_off.png");
  }


  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    QStyledItemDelegate::paint(painter, option, index);

    ObjectsTreeItem * tree_item = static_cast<ObjectsTreeItem*>(index.internalPointer());

    if (!tree_item)
      return;

    CGraphics object = tree_item->object();

    /*if (object && object.owner() && object.owner().objectType() == Grid)
      return;

    switch(object.objectType())
    {
    //case Document:
    case RenderDepth:
    case RenderOrdinaryDepth:
    case RenderProfiledDepth:
      return;

    default:;
    }*/

    if (object.isVisible())
    {
      painter->drawImage(option.rect.topLeft() + QPoint(0,2), _img_visible);
    } else {
      painter->drawImage(option.rect.topLeft() + QPoint(0,2), _img_hidden);
    }
  }


  QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
  {
    return QSize(20,20);
  }


private:
  QImage _img_visible;
  QImage _img_hidden;

};


class ExportItemDelegate : public QStyledItemDelegate
{
public:
  explicit ExportItemDelegate(QObject * = 0)
  {
    _img_export_enabled = QImage(":/images/resources/layers_document_export.png");
    _img_export_disabled = QImage(":/images/resources/layers_document_export_dsbl.png");
  }


  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    QStyledItemDelegate::paint(painter, option, index);

    ObjectsTreeItem * tree_item = static_cast<ObjectsTreeItem*>(index.internalPointer());

    if (!tree_item)
      return;

    CGraphics object = tree_item->object();

    //if (object && object.owner() && object.owner().objectType() == Grid)
    //  return;

    /*bool export_enbl = object.isExportEnabled();
    switch(object.objectType())
    {
    case Layer:
      break;
    case BigImage:
    case Grid:
      export_enbl &= object.owner() && object.owner().isExportEnabled();
      break;

    default:;
      return;
    }*/

    bool export_enbl = true;

    if (export_enbl)
    {
      painter->drawImage(option.rect.topLeft() + QPoint(0,2), _img_export_enabled);
    } else {
      painter->drawImage(option.rect.topLeft() + QPoint(0,2), _img_export_disabled);
    }
  }


  QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
  {
    return QSize(20,20);
  }


private:
  QImage _img_export_enabled;
  QImage _img_export_disabled;

};




LayersForm::LayersForm(QWidget * parent) :
  QWidget(parent),
  ui(new Ui::LayersForm)
{
  ui->setupUi(this);

/*  connect(ui->buttonNewLayer, SIGNAL(clicked()), ui->actionNewLayer, SLOT(trigger()));

  ui->buttonRemove->setEnabled(false);

  ui->buttonCombineToGrid->setVisible(true);
  ui->buttonBreakAppart->setVisible(true);
*/
  _item_icon_bmp = QIcon(":/images/bmp.png");
  _item_icon_eps = QIcon(":/images/eps.png");
  _item_icon_hyp = QIcon(":/images/hyp.png");
  _item_icon_tbl = QIcon(":/images/table.png");

  _item_icon_bmp_dsbl = QIcon(":/images/bmp_dsbl.png");
  _item_icon_eps_dsbl = QIcon(":/images/eps_dsbl.png");
  _item_icon_hyp_dsbl = QIcon(":/images/hyp_dsbl.png");
  _item_icon_tbl_dsbl = QIcon(":/images/table_dsbl.png");

  _item_icon_mask = QIcon(":/images/mask.png");

  _update_tree_disabled = false;

  _disable_all_actions();

  _tree_model = new ObjectsTreeModel(this);
  ui->treeView->setModel(_tree_model);

  ui->treeView->setItemDelegateForColumn(EXPR_COLUMN, new ExportItemDelegate);
  ui->treeView->setItemDelegateForColumn(VIEW_COLUMN, new ViewItemDelegate);
  ui->treeView->setItemDelegateForColumn(LOCK_COLUMN, new LockItemDelegate);

  connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(_tree_selection_changed(QItemSelection,QItemSelection)));

  connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
          this, SLOT(_current_changed(QModelIndex,QModelIndex)));

  connect(hprj, SIGNAL(selectionChanged()), this, SLOT(_scene_selection_changed()));


  connect(hprj, SIGNAL(documentChanged(CDocument)), this, SLOT(_document_changed(CDocument)));


  connect(hprj, SIGNAL(objectAdded(CGraphics, CGraphics)),
          this, SLOT(_object_added(CGraphics, CGraphics)));

  connect(hprj, SIGNAL(objectRemoved(CGraphics, CGraphics)),
          this, SLOT(_object_removed(CGraphics, CGraphics)));

  connect(hprj, SIGNAL(layerActivated(CLayer)),
          this, SLOT(_layer_activated(CLayer)));

  connect(hprj, SIGNAL(layerDeactivated(CLayer)),
          this, SLOT(_layer_deactivated(CLayer)));



  if (hprj->document())
    _document_changed(hprj->document());
}


void LayersForm::resizeEvent(QResizeEvent *)
{
  int w = ui->treeView->width();

  if (ui->treeView->verticalScrollBar()->isVisible())
    w -= ui->treeView->verticalScrollBar()->width();

  ui->treeView->setColumnWidth(TEXT_COLUMN, w-82);
  ui->treeView->setColumnWidth(EXPR_COLUMN, 20);
  ui->treeView->setColumnWidth(VIEW_COLUMN, 20);
  ui->treeView->setColumnWidth(LOCK_COLUMN, 20);
}



LayersForm::~LayersForm()
{
  delete ui;
}


void LayersForm::on_actionNewLayer_triggered()
{
/*  vxDocument document = vxEnvironment::activeDocument();

  vxLayer layer(&document);

  vxCommandAddLayerToProject * add_command = new vxCommandAddLayerToProject(document, layer);

  add_command->setText(tr("add layer"));

  document.execute(add_command);

  _set_current_tree_item(layer);*/
}



void LayersForm::on_buttonAddRender_clicked()
{
/*  QList<vxGraphics> selection = getSelectedObjects();

  if (selection.count() != 1)
    return;

  vxGraphics object = selection.first();
  if (object.objectType() != Layer)
    return;

  vxDocument document = vxEnvironment::activeDocument();

  vxLayer layer = object;

  QMenu menu;
  {
    QAction * action = menu.addAction("Depth render");
    action->setIcon(QIcon(QPixmap(":/images/hammer.png")));
    action->setData(RenderOrdinaryDepth);
  }

  if (document.imageFormat() == bifGrayscale)
  {
    QAction * action = menu.addAction("Profiled render");
    action->setIcon(QIcon(QPixmap(":/images/hammer.png")));
    action->setData(RenderProfiledDepth);
  }

  QAction * selected_action = menu.exec(parentWidget()->mapToGlobal(ui->buttonAddRender->geometry().bottomLeft()));

  if (!selected_action)
    return;

  int render_type_id = selected_action->data().toInt();

  vxRender render;

  switch(render_type_id)
  {
  case RenderOrdinaryDepth:
    render = vxDepthRender();
    break;

  case RenderProfiledDepth:
    render = vxProfiledDepthRender();
    break;
  }

  vxCommandAddRender * command = new vxCommandAddRender(document, layer, render);
  command->setText(tr("add render to layer"));

  document.execute(command);

  _set_current_tree_item(render);*/
}




bool LayersForm::_remove_objects()
{
/*  vxDocument document = vxEnvironment::activeDocument();

  QList<vxGraphicsObject> objects = document.getSelectedObjects();

  if (objects.count() == 0)
    return false;

  vxCommandDeleteObjects * delete_command = new vxCommandDeleteObjects(document, objects);

  delete_command->setText(tr("delete object%1").arg(objects.count() == 1 ? "s" : ""));

  document.execute(delete_command);

  vxEnvironment::notifyObjectsRemoved(objects);
*/
  return true;
}



bool LayersForm::_remove_object(const CGraphics& )
{
/*  bool need_notify = false;

  vxDocument document = vxEnvironment::activeDocument();

  switch (object.objectType())
  {
  case ImageMask:
  {
    vxCommandRemoveMask * remove_mask = new vxCommandRemoveMask(document, object.owner());
    remove_mask->setText(QString("remove mask"));
    document.execute(remove_mask);

    _set_current_tree_item(object.owner());

    break;
  }

  case RenderDepth:
  case RenderOrdinaryDepth:
  case RenderProfiledDepth:
  case RenderConturs:
  {
    vxRender render = object;
    vxLayer layer = render.ownerLayer();
    if (layer.isPresent())
    {
      vxCommandRemoveRender * command = new vxCommandRemoveRender(document, layer, render);
      command->setText(tr("remove render %1").arg(object.caption()));
      document.execute(command);

      _set_current_tree_item(layer);
    }
    break;
  }

  case Grid:
  case Blank:
  case BigImage:
  {
    vxGraphicsObject graphics = object;
    vxLayer layer = graphics.layer();
    int object_ix = layer.objects().indexOf(object);

    vxCommandDeleteObjects * command = new vxCommandDeleteObjects(document, object);
    command->setText(tr("remove object %1").arg(object.caption()));
    document.execute(command);

    vxGraphics current;

    if (layer.objects().count() > object_ix)
      current = layer.objects().at(object_ix);
    else
    if (layer.objects().count() == object_ix && object_ix > 0)
      current = layer.objects().at(object_ix-1);
    else
      current = layer;

    _set_current_tree_item(current);

    break;
  }

  case Layer:
  {
    vxLayer layer = object;
    int layer_ix = document.layers().indexOf(layer);

    vxCommandRemoveLayerFromProject * command = new vxCommandRemoveLayerFromProject(document, layer);
    command->setText(tr("remove layer %1").arg(layer.caption()));
    document.execute(command);

    vxGraphics current;

    if (document.layers().count() > layer_ix)
      current = document.layers().at(layer_ix);
    else
    if (document.layers().count() == layer_ix && layer_ix > 0)
      current = document.layers().at(layer_ix-1);

    if (current.isPresent())
      _set_current_tree_item(current);

    break;
  }

  default:;

  }

  return need_notify;*/
  return false;
}


void LayersForm::on_buttonRemove_clicked()
{
/*  bool need_notify = false;

  QList<vxGraphics> selection = getSelectedObjects();

  if (selection.count() == 0)
    return;

  vxDocument document = vxEnvironment::activeDocument();

  if (selection.count() == 1 && document.getSelectedObjects().count() <= 1)
  {
    vxGraphics object = selection.first();
    need_notify = _remove_object(object);
  } else {
    need_notify = _remove_objects();
  }

  if (need_notify)
  {
    vxEnvironment::notifySceneChanged();
    vxEnvironment::notifyProjectChanged();
  }*/
}


void LayersForm::on_buttonAddMask_clicked()
{
/*  QList<vxGraphics> selection = getSelectedObjects();

  if (selection.count() != 1)
    return;

  vxGraphics object = selection.first();
  if (object.objectType() == BigImage)
  {
    object = vxBigImage(object.owner());
  }

  if (object.objectType() != Layer)
    return;

  vxLayer layer = object;

  QString fileName;
  {
    MasksForm masks_form;
    if (!masks_form.exec())
      return;

    fileName = masks_form.maskFileName();
  }

  vxProject * project = vxEnvironment::activeProject();
  QString  mask_file_name;

  if (!project->addBmp(fileName,  mask_file_name))
  {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Can't open image.");
    msgBox.setInformativeText(QString("Image data corrupted or have unsupported format."));
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    return;
  }

  vxDocument document = vxEnvironment::activeDocument();

  vxImageMask image_mask(mask_file_name, 10, &document);

  vxCommandAddMask * add_mask_command = new vxCommandAddMask(document, layer, image_mask);

  add_mask_command->setText(tr("add mask %1").arg(QFileInfo(mask_file_name).fileName()));

  document.execute(add_mask_command);

  vxEnvironment::notifyProjectChanged();
*/
}




void LayersForm::_active_document_changed()
{
  QTimer::singleShot(100, this, SLOT(_expand_root()));
}


void LayersForm::_expand_root()
{
  ui->treeView->expandToDepth(0);
}

/*
void LayersForm::_update_indexes(const QModelIndex &model_index, int column)
{
  if (_tree_model->rowCount(model_index) > 0)
  {
    int n = _tree_model->rowCount(model_index);
    for(int i=0; i < n; ++i)
    {
      QModelIndex child_index = model_index.child(i, column);
      ui->treeView->update(child_index);
      _update_indexes(child_index, column);
    }
  }
}
*/

void LayersForm::on_treeView_pressed(const QModelIndex &model_index)
{
  ObjectsTreeItem * item = static_cast<ObjectsTreeItem*>(model_index.internalPointer());
  CGraphics object = item->object();


  switch(model_index.column())
  {
  case TEXT_COLUMN:
  {
    if (object.type() == Layer)
    {
      CLayer layer = object;
      hprj->document().setActiveLayer(layer);


      _prev_model_index = model_index;
    }
    break;
  }
  default:;
  }
/*
  case EXPR_COLUMN:
  {
    bool expr_enbl = !object.isExportEnabled();
    object.setExportEnabled(expr_enbl);
    _update_indexes(model_index, EXPR_COLUMN);
    vxEnvironment::notifySceneChanged();
    break;
  }

  case VIEW_COLUMN:
  {
    bool visible = !object.isVisible();
    if (visible && object.objectType() == Layer)
    {
      vxLayer layer = object;
      vxDocument doc = layer.owner();
      if (!doc.isVisible())
      {
        doc.setVisible(true, false);
        for(const vxLayer &l : doc.layers())
        {
          l.setVisible(false, false);
        }
      }
    }

    if (object.objectType() == Document)
    {
      vxDocument doc = object;
      bool all_visible=true;
      bool all_invisible=true;
      for(const vxLayer &layer : doc.layers())
      {
        all_visible &= layer.isVisible();
        all_invisible &= !layer.isVisible();
      }
      if (visible && all_invisible)
      {
        for(const vxLayer &layer : doc.layers())
        {
          layer.setVisible(true, false);
        }
      } else
      if (!visible && all_visible)
      {
        for(const vxLayer &layer : doc.layers())
        {
          layer.setVisible(false, false);
        }
      }
    }

    object.setVisible(visible, true);
    if (object.objectType() == ImageMask)
    {
      vxImageMask mask = object;
      mask.owner().doChanged_s();
    }
    _update_indexes(model_index, VIEW_COLUMN);
    vxEnvironment::notifySceneChanged();
    break;
  }

  case LOCK_COLUMN:
  {
    bool locked = !object.isLocked();

    if (!locked && object.objectType() == Layer)
    {
      vxLayer layer = object;
      vxDocument doc = layer.owner();
      if (doc.isLocked())
      {
        doc.setLocked(false, false);
        for(const vxLayer &l : doc.layers())
        {
          l.setLocked(true, false);
        }
      }
    }

    if (object.objectType() == Document)
    {
      vxDocument doc = object;
      bool all_locked=true;
      bool all_unlocked=true;
      for(const vxLayer &layer : doc.layers())
      {
        all_locked &= layer.isLocked();
        all_unlocked &= !layer.isLocked();
      }
      if (locked && all_unlocked)
      {
        for(const vxLayer &layer : doc.layers())
        {
          layer.setLocked(true, false);
        }
      } else
      if (!locked && all_locked)
      {
        for(const vxLayer &layer : doc.layers())
        {
          layer.setLocked(false, false);
        }
      }
    }

    if (object.objectType() == Layer)
    {
      vxLayer layer = object;
      for(const vxGraphicsObject &obj : layer.objects())
      {
        obj.setLocked(!object.isLocked(), true);
      }
    }

    object.setLocked(!object.isLocked(), true);
    _update_indexes(model_index, LOCK_COLUMN);
    vxEnvironment::notifySceneChanged();
    break;
  }

  case GROUP_COLUMN:
  {
    //object.setGroupEnabled(!object.isGroupEnabled());
    //vxEnvironment::notifySceneChanged();
    return;
  }

  default:;
  }

  ui->treeView->update(model_index);

  project->updateScene();
*/
}



void LayersForm::_current_changed(QModelIndex, QModelIndex)
{
  _update_model_selection();
}


void LayersForm::_tree_selection_changed(QItemSelection selected_item, QItemSelection deselected_items)
{
  if (_update_tree_disabled)
    return;

  _update_tree_disabled = true;

  for(const QModelIndex& index : deselected_items.indexes())
  {
    _tree_model->setData(index, 0, roleSelected);
  }

  for(const QModelIndex& index : selected_item.indexes())
  {
    _tree_model->setData(index, 1, roleSelected);
  }

  _validate_controls();

  _update_tree_disabled = false;
}



void LayersForm::_scene_selection_changed()
{
  if (_update_tree_disabled)
    return;

  QModelIndexList selected_list;
  ObjectsTreeItem * root_item = _tree_model->rootItem();
  bool has_selected = false;
  for(int row=0; row < root_item->childsCount(); ++row)
  {
    QModelIndex root_index = _tree_model->index(row, 0);

    has_selected |= _expand_selected(root_index, selected_list);
  }

  if (has_selected)
    ui->treeView->scrollTo(selected_list.first());
  else
    ui->treeView->scrollTo(ui->treeView->currentIndex());

  _update_tree_disabled = true;

  QItemSelectionModel * selection_model = ui->treeView->selectionModel();
  selection_model->clear();

  for(const QModelIndex& index : selected_list)
  {
    selection_model->select(index, QItemSelectionModel::Select);
    selection_model->select(index.sibling(index.row(), 1), QItemSelectionModel::Select);
    selection_model->select(index.sibling(index.row(), 2), QItemSelectionModel::Select);
  }
  _update_tree_disabled = false;

}


bool LayersForm::_expand_selected(const QModelIndex & item_index, QModelIndexList& selected_list)
{
  if (!item_index.isValid())
    return false;

  int n = _tree_model->rowCount(item_index);

  bool is_selected = item_index.data(roleSelected).toBool();

  if (is_selected)
  {
    selected_list.append(item_index);
    is_selected = item_index.data(roleSelected).toBool();
  }


  for(int i=0; i < n; ++i)
  {
    QModelIndex model_index = item_index.child(i, 0);
    is_selected |= _expand_selected(model_index, selected_list);
  }

  if (is_selected)
    ui->treeView->expand(item_index);

  return is_selected;
}



void LayersForm::_set_current_tree_item(const CGraphics& object)
{
  ui->treeView->selectionModel()->clear();
  ui->treeView->update();

  ObjectsTreeItem * root_item = _tree_model->rootItem();

  QModelIndex root_index = _tree_model->index(root_item->row(), 0);

  _set_current_tree_item(root_index, object);

  _validate_controls();
}


bool LayersForm::_set_current_tree_item(const QModelIndex& parent_index, const CGraphics& object)
{
  if (!parent_index.isValid())
    return false;

  int n = _tree_model->rowCount(parent_index);

  ObjectsTreeItem * tree_item = static_cast<ObjectsTreeItem*>(parent_index.internalPointer());

  if (tree_item->object() == object)
  {
    ui->treeView->setCurrentIndex(parent_index);
    ui->treeView->scrollTo(parent_index);
    return true;
  }

  for(int i=0; i < n; ++i)
  {
    QModelIndex model_index = parent_index.child(i, 0);
    if (_set_current_tree_item(model_index, object))
      return true;
  }

  return false;
}


void LayersForm::_update_model_selection()
{
/*  QItemSelection selection = ui->treeView->selectionModel()->selection();

  QModelIndexList indexes = selection.indexes();

  vxDocument document = vxEnvironment::activeDocument();
  document.clearSelection();

  foreach (QModelIndex index, indexes)
  {
    HypTreeItem * item = static_cast<HypTreeItem*>(index.internalPointer());
    if (!item)
      continue;

    vxGraphics object = item->object();
    object.setSelected(true);
  }

  disconnect(vxEnvironment::instance(), SIGNAL(selectionChanged(vxDocument)),
             this, SLOT(_scene_selection_changed(vxDocument)));

  vxEnvironment::notifySelectionChanged();

  vxEnvironment::notifyProjectChanged();

  connect(vxEnvironment::instance(), SIGNAL(selectionChanged(vxDocument)),
          this, SLOT(_scene_selection_changed(vxDocument)));
*/
}


void LayersForm::_disable_all_actions()
{return;
/*  ui->buttonRemove->setEnabled(false);
  ui->buttonAddRender->setVisible(false);
  ui->buttonCombineToGrid->setVisible(false);
  ui->buttonBreakAppart->setVisible(false);
  ui->buttonAddMask->setVisible(false);
  ui->buttonNewLayer->setVisible(false);
  ui->treeView->setEnabled(false);
  ui->refreshButton->setVisible(false);*/
}


QList<CGraphics> LayersForm::getSelectedObjects()
{
  QList<CGraphics> objects;

  QItemSelection selection = ui->treeView->selectionModel()->selection();

  QModelIndexList indexes = selection.indexes();

  foreach(QModelIndex index, indexes)
  {
    ObjectsTreeItem * item = static_cast<ObjectsTreeItem*>(index.internalPointer());

    if (!item) continue;

    CGraphics object = item->object();

    if (!objects.contains(object))
      objects.append(object);
  }
/*
  if (objects.count() == 0)
  {
    vxDocument document = vxEnvironment::activeDocument();
    QList<vxGraphicsObject> graphics = document.getSelectedObjects();
    if (graphics.count() > 0)
    {
      vxGraphics object = graphics.first();
      objects.append(object);
    }
  }
*/
  return objects;
}



void LayersForm::_validate_controls()
{
  _disable_all_actions();

  //ui->buttonNewLayer->setVisible(true);
  ui->treeView->setEnabled(true);

  QList<CGraphics> selection = getSelectedObjects();

  if (selection.count() == 0)
    return;

 // ui->buttonRemove->setEnabled(true);

  /*CGraphics object = selection.first();

  bool one_selected = selection.count() == 1;

  bool one_layer = true;
  bool one_type=true;
  int type_obj=-1;
  foreach(const CGraphics& graphics, selection)
  {
    if (!vxGraphicsObject::isGraphicsObject(graphics))
    {
      one_layer = false;
      break;
    }

    if (type_obj == -1)
    {
      type_obj = object.objectType();
    } else {
      if (object.objectType() != type_obj)
      {
        one_type = false;
      }
    }
    vxGraphicsObject object = graphics;
    if (!object.layer())
    {
      one_layer = false;
      break;
    }
    if (!layer)
    {
      layer = object.layer();
      continue;
    }
    if (layer != object.layer())
    {
      one_layer = false;
      break;
    }
  }

  if (object.objectType() == Layer)
  {
    vxLayer layer = object;
    if (layer.allIsImages())
    {
      if (!layer.isBackground())
      {
        ui->buttonAddRender->setVisible(!layer.haveRender());
        ui->buttonAddMask->setVisible(true);
      }
    }
  }

  if (one_selected)
  {
    if (vxGraphicsObject::isGraphicsObject(object))
      ui->buttonCombineToGrid->setVisible(true);
    bool owner_grid=object.owner().objectType() == Grid;
    if (owner_grid)
    {
      ui->buttonCombineToGrid->setVisible(false);
      ui->buttonRemove->setEnabled(false);
    } else {
      if (object.objectType() == Grid)
      {
        ui->buttonBreakAppart->setVisible(true);
        vxGrid grid = object;
        if (grid.sourceObject().objectType() == Grid)
        {
          ui->buttonCombineToGrid->setVisible(false);
        }
      }

      if (object.objectType() == Blank ||
          object.objectType() == Grid)
      {
        ui->refreshButton->setVisible(true);
      }
    }
  } else {
    if (one_layer && one_type)
    {
      //if (vxGraphicsObject::isGraphicsObject(object))
      //  ui->buttonCombineToGrid->setVisible(true);
      //ui->buttonRemove->setEnabled(false);
    }
    ui->buttonRemove->setEnabled(one_type);
  }*/

}




void LayersForm::on_treeView_customContextMenuRequested(const QPoint &)
{
//  QMenu menu(this);
 // vxEnvironment::requestEditMenu(&menu);
//  menu.exec(this->mapToGlobal(pos));
}




void LayersForm::on_refreshButton_clicked()
{
/*  QList<vxGraphics> selection = getSelectedObjects();

  if (selection.count() != 1)
    return;

  vxGraphics object = selection.first();

  if (object.objectType() == Grid)
  {
    vxGrid grid = object;
    object = grid.sourceObject();
  }

  if (object.objectType() != Blank)
    return;

  vxBlank blank = object;
  vxDocument document = blank.sourceDocument();

  QString doc_file_name = QFileInfo(QDir(document.project()->projectPath()), document.documentName()).absoluteFilePath();
  vxEnvironment::instance()->log(doc_file_name);

  if (!QFile(doc_file_name).exists())
  {
    vxEnvironment::instance()->log(QString("File does't exists %1").arg(doc_file_name));
    return;
  }

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setText(tr("Reload source"));

  msgBox.setInformativeText("Are you sure ? This command isn't available for undo");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  if (msgBox.exec() == QMessageBox::No)
    return;


  QFile file(doc_file_name);

  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    vxEnvironment::instance()->log(QString("Can't open file %1:\n%2.")
                         .arg(doc_file_name)
                         .arg(file.errorString()));
    return;
  }

  QString errorStr;
  int errorLine;
  int errorColumn;

  QDomDocument doc;

  if (!doc.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
    vxEnvironment::instance()->log(QString("Parse error at line %1, column %2:\n%3")
                       .arg(errorLine)
                       .arg(errorColumn)
                       .arg(errorStr));
    return;
  }

  QDomElement elm_project = doc.documentElement();

  QDomElement elm_document = elm_project.firstChildElement(TAG_DOCUMENT);

  document.readData(elm_document);

  document.notifyChanges(StructureChanged);

  document.clearSelection();

  vxEnvironment::notifyActiveDocumentChanged();

  vxEnvironment::notifyProjectChanged();*/
}


void LayersForm::_select_root()
{
  if (!_tree_model)
    return;

  ObjectsTreeItem * project_item = _tree_model->rootItem();

  if (!project_item)
    return;

  QModelIndex project_index = _tree_model->index(project_item->row(), 0);

  if (!project_index.isValid())
    return;

  _validate_controls();
}


void LayersForm::_active_doc_changed()
{
  QTimer::singleShot(500, this, SLOT(_select_root()));
}

void LayersForm::_document_changed(const CDocument &)
{
}


void LayersForm::_layer_activated(CLayer )
{
}


void LayersForm::_layer_deactivated(CLayer )
{
}


void LayersForm::_object_added(CGraphics object, CGraphics dest)
{
  _tree_model->addObjectTo(object, dest);
  _scene_selection_changed();
}


void LayersForm::_object_removed(CGraphics object, CGraphics dest)
{
  _tree_model->removeObjectFrom(object, dest);
  _scene_selection_changed();
}



void LayersForm::on_buttonNewLayer_clicked()
{
  hprj->actionNewLayer();
}
