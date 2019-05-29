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
  // will come later
}



void LayersForm::on_buttonAddRender_clicked()
{
  // will come later
}




bool LayersForm::_remove_objects()
{
  // will come later
  return true;
}



bool LayersForm::_remove_object(const CGraphics& )
{
  // will come later
  return false;
}


void LayersForm::on_buttonRemove_clicked()
{
  // will come later
}


void LayersForm::on_buttonAddMask_clicked()
{
  // will come later
}




void LayersForm::_active_document_changed()
{
  QTimer::singleShot(100, this, SLOT(_expand_root()));
}


void LayersForm::_expand_root()
{
  ui->treeView->expandToDepth(0);
}


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
  // will come later
}


void LayersForm::_disable_all_actions()
{
  ui->buttonRemove->setEnabled(false);
  ui->buttonAddRender->setVisible(false);
  ui->buttonCombineToGrid->setVisible(false);
  ui->buttonBreakAppart->setVisible(false);
  ui->buttonAddMask->setVisible(false);
  ui->buttonNewLayer->setVisible(false);
  ui->treeView->setEnabled(false);
  ui->refreshButton->setVisible(false);
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
  return objects;
}



void LayersForm::_validate_controls()
{
  _disable_all_actions();

  ui->treeView->setEnabled(true);

  QList<CGraphics> selection = getSelectedObjects();

  if (selection.count() == 0)
    return;

move->setEnabled(one_type);
  }*/

}




void LayersForm::on_treeView_customContextMenuRequested(const QPoint &)
{
  QMenu menu(this);
  vxEnvironment::requestEditMenu(&menu);
  menu.exec(this->mapToGlobal(pos));
}




void LayersForm::on_refreshButton_clicked()
{
  QList<vxGraphics> selection = getSelectedObjects();

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

  vxEnvironment::notifyProjectChanged();
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
