#ifndef CPROJECTHANDLER_H
#define CPROJECTHANDLER_H

#include "cenvironment.h"
#include "cobject.h"
#include "ctools.h"
#include "cdocument.h"
#include "citems.h"

#include <QUndoStack>
#include <QActionGroup>

#define hprj CProjectHandler::instance()

class CTransformControl;

namespace sm
{

enum Actions
{
  ActionSelector,
  ActionPen,
  ActionLasso,
  ActionEllipse,
  ActionSquare,
  ActionShowImage,
  ActionParamAngle,
  ActionParamInterval,
  ActionParamBasrelief,
  ActionParamThickness,
  ActionParamThreshold,
  ActionParamDottedLine,
  ActionParamSmooth,
  ActionUndo,
  ActionRedo,
  ActionTransform,
  ActionApplyTransform,
  ActionGroup,
  ActionUngroup
};


class CLayer;
class CShape;
class CGroup;
class CStrokesTemplate;



class CProjectHandler : public QObject
{
  Q_OBJECT

public:
  static CProjectHandler * instance()
  {
    if (!_instance)
      _instance = new CProjectHandler();

    return _instance;
  }

  void init(QGraphicsScene *, QGraphicsView *);

  void updateItems();

  void repaintScene();

  void setTool(CAbstractTool *);
  void setTool(int);

  CAbstractTool * tool();

  bool eventFilter(QObject *, QEvent * ) override;

  bool loadImage(const QString &);
  bool loadImage(const QImage &);

  void setAction(int key, QAction*, QActionGroup* =0);
  QAction * action(int key);

  CDocument document() const
  {
    return _document;
  }

  void setDocument(const CGraphics &doc);

  void addMask(const QString &name, const CGraphics&);

  CGraphics mask(const QString &name);

  void loadLayout(QMainWindow*);
  void saveLayout(QMainWindow*);

  void loadLayout(QMainWindow*, const QString& file_name);
  void saveLayout(QMainWindow*, const QString& file_name);

  QString layoutFileName();


  void initLog(QTextEdit*);

  void log(const QString&, bool no_cr=false);

  void log(const QString&, const QString&);

  void log(const QString&, const QStringList&);

  void log(const QStringList&);


  QUndoStack * undoStack()
  {
    return &_undo_stack;
  }

  QImage sourceImage() const
  {
    return _source_image;
  }

  QOpenGLContext * context() const
  {
    return _viewport->context();
  }

  void execute(QUndoCommand*);

  void assignItemToObject(QGraphicsItem*,const CGraphics&);

  QGraphicsScene *scene()
  {
    return _scene;
  }

  void updateStrokesTemplate(const CStrokesTemplate&);

  void startRenderProcess(const CShape&);

  bool getHitItem(const QPointF &, QGraphicsItem * &);

  bool getSelection(QList<CGraphics>&);

  void unselectAll(QGraphicsItem * exclude_item=nullptr);

  void select(const CGraphics&, bool keep_selection=false);
  void select(const QList<CGraphics>&, bool keep_selection=false);

  void registerThread(QThread*);

  void updateControls();

  bool hasTransformControl() const;

  CTransformControl * transformControl() const;

  bool clearTransformControl();

  bool cancelTransform();

  QGraphicsItem * hoveredItem()
  {
    return _hovered_item;
  }

  void setHoveredItem(QGraphicsItem * item);


  void notifySceneMouseMove(const QPointF &scene_pos);

  QWidget * workspaceWidget() const
  {
    return _viewport;
  }

  CGraphics hoveredTmpl() const
  {
    return _hovered_tmpl;
  }

  void setHoveredTmpl(const CGraphics &tmpl)
  {
    _hovered_tmpl = tmpl;
  }

  bool getSelected(Types, QList<CGraphics>&) const;

  void notifySelectionChanged();


  void actionNewLayer();

  void groupSelected();

  void ungroupSelected();

  // drag selection

  TxSelectionRec beginDragSelection(const QPointF &scene_pos, const QList<QGraphicsItem*> &selection);
  void dragSelection(const QPointF &scene_pos, TxSelectionRec&);
  bool endDragSelection(TxSelectionRec&);

  bool readProject(const QString &file_name);

  bool doCheckSaveChanges();
  bool hasUnsavedChanges();

  bool doNewProject();
  bool doOpenFile();
  bool doSaveProject();

  void newProject(const QSize&, int r);

  void buildRecentFilesMenu(QMenu * menu);

  void updateRecentFilesSettings();

  bool isShowSourceImage() const
  {
    return _is_show_source_image;
  }


private:
  static CProjectHandler * _instance;

  CProjectHandler();

  QGraphicsScene * _scene;
  QGraphicsView * _graphics_view;
  QOpenGLWidget * _viewport;

  QRectF _view_rect;

  QSet<ObjId> _items_in_process;
  QSet<ObjId> _items_is_edged;
  QSet<ObjId> _items_is_knife;

  QSet<ObjId> _edges_in_process;

  CAbstractTool * _current_tool=0;

  CGraphics _selected_object;

  QMap<int, QAction*> _actions_map;

  QImage _source_image;
  bool _need_update_scene_after_edges_finished=false;

  QTextEdit * _log_edit;

  CDocument _document;

  QMap<QString, CGraphics> _masks_map;

  QUndoStack _undo_stack;
  int _save_history_ix=0;

  QTimer _update_items_timer;
  QSet<QGraphicsView*> _to_update;
  QSet<QGraphicsItem*> _trash_items;

  int _saved_tool;
  bool _restore_tool=false;

  QGraphicsItem *_hovered_item=0, *_prev_hovered_item=0;

  CGraphics _hovered_tmpl;

  QStringList _recent_files;

  bool _is_show_source_image = true;

  void _update_strokes_layer(const CLayer&);
  void _update_strokes_element(const CShape&);
  void _update_edged_templates();

  QAtomicInt _thread_counter=0;


private slots:
  void _selection_changed();

  void _on_action_show_image(bool);
  void _on_action_selector(bool);
  void _on_action_pen(bool);
  void _on_action_lasso(bool);
  void _on_action_square(bool);
  void _on_action_ellipse(bool);

  void _thread_started();
  void _thread_finished();

  void _history_index_changed(int);

  void _on_undo_triggered(bool);
  void _on_redo_triggered(bool);
  void _can_undo_changed(bool);
  void _can_redo_changed(bool);
  void _clear_items_trash();
  void _on_group_triggered(bool);
  void _on_ungroup_triggered(bool);

  void _validate_actions();

  void _on_transform_triggered(bool);
  void _on_apply_transform_triggered(bool);

  void _order_z_items(const CLayer &root);

  void _current_metric_changed(vxMetrics);

  void _load_image_file(const QString&);
  void _load_qimage(const QImage&);

  void _recent_menu_triggered(QAction*);
  void _open_project(const QString&);

signals:
  void toolChanged(CAbstractTool*tool);
  void imageLoaded(const QImage&);
  void requestShowWorkspace();
  void requestShowWelcome();
  void documentChanged(const CDocument&);
  void selectionChanged();

  void objectAdded(const CGraphics &object, const CGraphics &dest);
  void objectRemoved(const CGraphics &object, const CGraphics &dest);

  void layerActivated(const CLayer&);
  void layerDeactivated(const CLayer&);

  void viewUpdated(QGraphicsView*);

  void threadsStop();

  void historyChanged();

  void sceneMouseMove(const QPointF&);

  void projectReaded(const CDocument&);

  void showSourceImage(bool);
};

}
#endif // CPROJECTHANDLER_H
