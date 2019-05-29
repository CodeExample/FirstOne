#ifndef LAYERSFORM_H
#define LAYERSFORM_H

#include "objectstreemodel.h"
#include "cdocument.h"

#include <QTreeWidgetItem>


namespace Ui {
class LayersForm;
}

class MainWindow;

class LayersForm : public QWidget
{
  Q_OBJECT

public:
  explicit LayersForm(QWidget *parent);
  ~LayersForm();


private slots:

  void on_actionNewLayer_triggered();

  void on_buttonAddRender_clicked();

  void on_buttonRemove_clicked();

  void on_buttonAddMask_clicked();

  void _active_document_changed();

  void _expand_root();

protected:
  void resizeEvent(QResizeEvent *);

private:
  Ui::LayersForm *ui;

  QIcon _item_icon_bmp;
  QIcon _item_icon_eps;
  QIcon _item_icon_hyp;
  QIcon _item_icon_tbl;

  QIcon _item_icon_bmp_dsbl;
  QIcon _item_icon_eps_dsbl;
  QIcon _item_icon_hyp_dsbl;
  QIcon _item_icon_tbl_dsbl;

  QIcon _item_icon_mask;

  QModelIndex _prev_model_index;

  bool _update_tree_disabled;

  void _update_model_selection();

  QList<CGraphics> getSelectedObjects();

  bool _remove_objects();

  bool _remove_object(const CGraphics&);

  bool _expand_selected(const QModelIndex&, QModelIndexList&);

  void _set_current_tree_item(const CGraphics&);
  bool _set_current_tree_item(const QModelIndex&, const CGraphics&);


  //void _update_indexes(const QModelIndex &model_index, int);

private slots:
  void _tree_selection_changed(QItemSelection,QItemSelection);
  void _current_changed(QModelIndex,QModelIndex);
  void _scene_selection_changed();

  void on_treeView_customContextMenuRequested(const QPoint &pos);

  void _disable_all_actions();

  void _validate_controls();

  void _active_doc_changed();

  void _select_root();

  void on_refreshButton_clicked();

  void on_treeView_pressed(const QModelIndex &index);

  void _document_changed(const CDocument&);

  void _layer_activated(CLayer);

  void _layer_deactivated(CLayer);

  void _object_added(CGraphics, CGraphics);

  void _object_removed(CGraphics, CGraphics);

  void on_buttonNewLayer_clicked();

public:
  ObjectsTreeModel * _tree_model;


};

#endif // LAYERSFORM_H
