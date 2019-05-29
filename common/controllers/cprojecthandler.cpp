#include "cprojecthandler.h"

#include "cmodel.h"
#include "cgroup.h"
#include "clayer.h"
#include "cproject.h"
#include "cshape.h"
#include "cattributes.h"

#include "cselectiontool.h"
#include "chandtool.h"
#include "ccompasstool.h"
#include "cshapetool.h"
#include "cdocumentitem.h"
#include "classotool.h"
#include "ctransformcontrol.h"
#include "cshapeitem.h"
#include "ccommands.h"
#include "cgroupitem.h"

#include "cparserstrx.h"
#include "cattributes.h"

#include "newprojectdlg.h"
#include "citemscontroller.h"

#include <QFileDialog>
#include <QDragEnterEvent>

#define BACKGROUND_COLOR   0xffaaaaaa//0xff747781 //0xff4e505d

using namespace sm;

CProjectHandler * CProjectHandler::_instance=0;


CProjectHandler::CProjectHandler()
{
  qApp->installEventFilter(this);
}


void CProjectHandler::init(QGraphicsScene * scene, QGraphicsView * view)
{
  _scene = scene;
  _graphics_view = view;

  _scene->setBackgroundBrush(QBrush(QColor(BACKGROUND_COLOR), Qt::SolidPattern));

  connect(_scene, SIGNAL(selectionChanged()), this, SLOT(_selection_changed()));

  connect(this, SIGNAL(objectRemoved(CGraphics, CGraphics)),
          this, SLOT(_object_removed(CGraphics, CGraphics)));

  connect(this, SIGNAL(objectAdded(CGraphics, CGraphics)),
          this, SLOT(_object_added(CGraphics, CGraphics)));

  connect(&_undo_stack, SIGNAL(indexChanged(int)),
          this, SLOT(_history_index_changed(int)));

  connect(&_undo_stack, SIGNAL(canUndoChanged(bool)),
          this, SLOT(_can_undo_changed(bool)));

  connect(&_undo_stack, SIGNAL(canRedoChanged(bool)),
          this, SLOT(_can_redo_changed(bool)));

  connect(CMetric::instance(), SIGNAL(metricChanged(vxMetrics)),
          this, SLOT(_current_metric_changed(vxMetrics)));

  connect(CEnvironment::instance(), SIGNAL(imageDroppedToWorkspace(QString)),
          this, SLOT(_load_image_file(QString)));

  connect(CEnvironment::instance(), SIGNAL(imageDroppedToWorkspace(QImage)),
          this, SLOT(_load_qimage(QImage)));

  connect(CEnvironment::instance(), SIGNAL(openProjectRequest(QString)),
          this, SLOT(_open_project(QString)));

  setTool(new CSelectionTool());

  _viewport = static_cast<QOpenGLWidget*>(view->viewport());
  _viewport->installEventFilter(this);
/*
  if (qApp->arguments().count() > 0)
  {
    if (QFileInfo(qApp->arguments().at(1)).exists())
    {
      loadImage(qApp->arguments().at(1));
    }
  }
*/
}




class OrderItems : public IVisitor
{
public:
  OrderItems(ItemsMap *items_map) : _items_map_ptr(items_map)
  {

  }

  bool visitTo(const CGraphics &object) override
  {
    if (_items_map_ptr->contains(object.id()))
    {
      QGraphicsItem * item = _items_map_ptr->value(object.id());
      item->setZValue(++_z_order);
    }
    return false;
  }

  ItemsMap * _items_map_ptr;
  int _z_order = Z_VALUE_ENGRAVING_ITEM;

};

void CProjectHandler::_order_z_items(const CLayer &)
{
  //OrderItems ov(&_items_map);
  //root.iterate(&ov);
}


void CProjectHandler::_update_strokes_layer(const CLayer &layer)
{
  for(const CGraphics &object: layer.objects())
  {
    switch (object.type())
    {
    case Layer:
      _update_strokes_layer(object);
      break;

    case Shape:
      _update_strokes_element(object);
      break;

    default:;
    }
  }
}


void CProjectHandler::_update_strokes_element(const CShape &element)
{
  for(const CStrokesTemplate &tmpl : element.templates())
  {
    updateStrokesTemplate(tmpl);
  }
}


void CProjectHandler::startRenderProcess(const CShape &shape)
{
  for(const CStrokesTemplate &tmpl : shape.templates())
  {
    updateStrokesTemplate(tmpl);
  }
}


void CProjectHandler::updateStrokesTemplate(const CStrokesTemplate &)
{
/*  ObjId id = tmpl.id();
  CShapeItem * item = _items_map.contains(id) ? static_cast<CShapeItem*>(_items_map.value(id)) : 0;

  if (!item)
    return;

  if (!tmpl.isChanged())
    return;

  item->startStrokesRender(tmpl);*/
}


void CProjectHandler::repaintScene()
{
  _scene->update();
}


void CProjectHandler::updateItems()
{
/*  if (_edges_in_process.count() > 0)
    _need_update_scene_after_edges_finished = true;

  if (_need_update_scene_after_edges_finished)
    return;

  _check_document_item(_document);
  _check_object_item(_document.root());
  _update_strokes_layer(_document.root());
  _order_z_items(_document.root());*/
}




class CollectIntersectedEdges : public IVisitor
{
public:
  bool visitTo(const CGraphics &in_object)
  {
    if (in_object.type() != Template)
      return false;

    CStrokesTemplate edge = in_object;

    if (!edge.data()->is_edged)
      return false;

    if (edges.contains(edge))
        return false;

    //QRectF e_r = edge.transform().mapRect(edge.boundingRect());

    for(const CStrokesTemplate &knife : knife_tmpls)
    {
      if (edge == knife)
        continue;

      //QRectF k_r = knife.transform().mapRect(knife.boundingRect());

      //if (e_r.intersects(k_r))
      {
        edges << edge;
        break;
      }
    }

    return false;
  }
  QList<CGraphics> edges;
  QList<CGraphics> knife_tmpls;
};


class CollectIntersectedKnives : public IVisitor
{
public:
  bool visitTo(const CGraphics &in_object)
  {
    if (in_object.type() != Template)
      return false;

    CStrokesTemplate knife = in_object;

    if (knife == edge)
      return false;

    if (!knife.data()->is_knife)
      return false;

    //QRectF e_r = edge.transform().mapRect(edge.boundingRect());
    //QRectF k_r = knife.transform().mapRect(knife.boundingRect());

    //if (e_r.intersects(k_r))
      knives << knife;

    return false;
  }

  CStrokesTemplate edge;

  QList<CObject> knives;
};




CAbstractTool * CProjectHandler::tool()
{
  return _current_tool;
}


void CProjectHandler::setTool(int tool_type)
{
  if (_current_tool->toolType() == tool_type)
    return;

  switch(tool_type)
  {
  case ToolSelector:
    setTool(new CSelectionTool());
    break;

  case ToolHand:
    setTool(new CHandTool());
    break;

  case ToolCompass:
    setTool(new CCompassTool());
    break;

    case ToolRectangle:
    setTool(new CShapeTool(CShapeTool::Rectangle));
    break;

  case ToolEllipse:
    setTool(new CShapeTool(CShapeTool::Ellipse));
    break;

  }
}


void CProjectHandler::setTool(CAbstractTool *tool)
{
  if (_current_tool && _current_tool->toolType() == tool->toolType())
  {
    tool->deleteLater();
    return;
  }

  if (_current_tool)
  {
    _current_tool->toolDeactivated();
    delete _current_tool;
  }

  switch (tool->toolType())
  {
  case ToolCompass:
  case ToolSelector:
    break;
  default:;
    clearTransformControl();
  }

  _current_tool = tool;

  _current_tool->setGraphicsView(_graphics_view);

  _current_tool->toolActivated();

  emit toolChanged(_current_tool);
}


class ToUpdateTemplates : public IVisitor
{
public:
  ToUpdateTemplates(const QRectF &scene_rect) : _scene_rect(scene_rect)
  {

  }

  bool visitTo(const CGraphics &object) override
  {
    if (object.type() == Template)
    {
      CStrokesTemplate tmpl = object;
      if (tmpl.boundingRect().intersects(_scene_rect))
        templates << tmpl;
    }
    return false;
  }

  QRectF _scene_rect;
  QList<CStrokesTemplate> templates;
};




bool CProjectHandler::eventFilter(QObject * object, QEvent * event)
{
  QWidget * widget = dynamic_cast<QWidget*>(object);
  if (widget && widget->parent())
  {
    QGraphicsView * view = dynamic_cast<QGraphicsView*>(widget->parent());
    if (view)
    {
      QRectF view_r_on_scene = view->mapToScene(view->viewport()->rect()).boundingRect();
      if (view->property(ATTR_VIEW_RECT).toRectF() != view_r_on_scene)
      {
        view->setProperty(ATTR_VIEW_RECT, view_r_on_scene);
        emit viewUpdated(view);
      }
    }
  }

  bool accepted = false;

  switch(event->type())
  {
  case QEvent::KeyPress:
  {
    if (qApp->focusWidget() == _graphics_view)
    {
      QKeyEvent * ke = static_cast<QKeyEvent*>(event);

      if (ke->isAutoRepeat())
      {
        _restore_tool = true;
        break;
      }

      _saved_tool = _current_tool->toolType();
      _restore_tool = false;

      switch(ke->key())
      {
      case Qt::Key_Enter:
      case Qt::Key_Return:
        if (hasTransformControl())
        {
          clearTransformControl();
          //applyTransform();
          updateItems();
          return true;
        }
        return false;


      case Qt::Key_Escape:
        if (hasTransformControl())
        {
          if (!cancelTransform())
          {
            clearTransformControl();
          } else {
            CTransformControl * transform_ctrl = transformControl();
            transform_ctrl->prepareTransform();
          }
          return true;
        }
        if (_scene->selectedItems().count() > 0)
        {
          _scene->clearSelection();
          return true;
        }

        return false;

      case Qt::Key_Space:
        setTool(new CHandTool());
        return true;

      case Qt::Key_R:
        setTool(new CCompassTool());
        return true;


      case Qt::Key_S:
        setTool(new CSelectionTool());
        return true;

      case Qt::Key_V:
        setTool(new CShapeTool(CShapeTool::Rectangle));
        return true;

      case Qt::Key_C:
        setTool(new CShapeTool(CShapeTool::Ellipse));
        return true;

      case Qt::Key_L:
        setTool(new CLassoTool());
        return true;

      default:;
      }
    }
    break;
  }
  case QEvent::KeyRelease:
  {
    if (qApp->focusWidget() == _graphics_view)
    {
      QKeyEvent * ke = static_cast<QKeyEvent*>(event);
      switch (ke->key())
      {
      case Qt::Key_Space:
      case Qt::Key_R:
      case Qt::Key_A:
      case Qt::Key_W:
      case Qt::Key_E:
      case Qt::Key_S:
      case Qt::Key_Q:
      case Qt::Key_C:
      case Qt::Key_D:
      case Qt::Key_F:
      case Qt::Key_V:
      case Qt::Key_G:
      case Qt::Key_L:
        if (_restore_tool)
          setTool(_saved_tool);
        return true;
      }
      _saved_tool = NoTool;
    }
    break;
  }

  case QEvent::Quit:
  {
    emit threadsStop();

    while(_thread_counter > 0)
    {
      qApp->processEvents();
      QThread::msleep(100);
    }
    return false;
  }

  //////////////////
  //
  // Drop actions
  //
  //
  /*
  case QEvent::DragEnter:
  {
    if (!_accept_drops)
      return false;

    if (object != _viewport)
      return false;

    QDragEnterEvent * drag_event = static_cast<QDragEnterEvent*>(event);

    if (drag_event->mimeData()->hasFormat("text/uri-list") ||
        drag_event->mimeData()->hasImage())
    {
      drag_event->acceptProposedAction();
    }
    else
    {
      drag_event->ignore();
    }

    accepted = true;
    break;
  }

  case QEvent::Drop:
  {
    if (!_accept_drops)
      return false;

    QDropEvent *drop_event = static_cast<QDropEvent*>(event);

    if (drop_event->mimeData()->hasImage())
    {
      if (_drop_image(drop_event->mimeData()))
      {
        drop_event->acceptProposedAction();
        accepted = true;
      }
    } else
      if (drop_event->mimeData()->hasFormat("text/uri-list"))
      {
        if (_drop_from_uri_list(drop_event->mimeData()))
        {
          drop_event->acceptProposedAction();
          accepted = true;
        }
      }
    break;
  }
*/
  default:;
  }

  return accepted;
}


bool CProjectHandler::loadImage(const QString &in_file_name)
{
  QString file_name = in_file_name;

  if (file_name == "" || !QFileInfo(file_name).exists())
  {
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QString formats_str;
    for(const QString &s : formats)
    {
      formats_str += QString("*.%1;").arg(s);
    }
    QFileDialog fileDialog(nullptr, tr("Load image"), "",
                           QString("All Images (%1)").arg(formats_str));

    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    QSettings settings;
    if (!settings.value("path_to_image", "").isNull())
    {
      fileDialog.setDirectory(settings.value("path_to_image").toString());
    }
    if (!fileDialog.exec())
      return false;

    QStringList fileNames = fileDialog.selectedFiles();

    if (fileNames.count() == 0)
      return false;

    file_name = fileNames.first();
    settings.setValue("path_to_image", QFileInfo(file_name).absoluteFilePath());

  }

  QImage image = QImage(file_name);
  _source_image = image;

  _document = CDocument(image.size(), 72, vxmPixels);

  emit documentChanged(_document);
  emit imageLoaded(image);

  /*if (_image_item)
  {
    delete _image_item;
    _image_item = nullptr;
  }*/
  _on_action_show_image(true);

  QTimer::singleShot(10, this, SIGNAL(requestShowWorkspace()));


  return true;
}


bool CProjectHandler::loadImage(const QImage &in_image)
{
  _source_image = in_image;

  _document = CDocument(in_image.size(), 72, vxmPixels);

  emit documentChanged(_document);
  emit imageLoaded(in_image);
/*
  if (_image_item)
  {
    delete _image_item;
    _image_item = nullptr;
  }*/
  _on_action_show_image(true);

  QTimer::singleShot(10, this, SIGNAL(requestShowWorkspace()));

  return true;
}


void CProjectHandler::_selection_changed()
{
  bool has_selection = _scene->selectedItems().count() > 0;
  if (!has_selection)
  {
    _selected_object = nullptr;
    CTransformControl * transform_ctrl = transformControl();
    if (transform_ctrl)
    {
      if (transform_ctrl->hasChanges())
      {
        transform_ctrl->finish();
        //applyTransform(transform_ctrl->selection());
      }
      delete transform_ctrl;
    }

  } else {
      _selected_object = nullptr;
  }

  _validate_actions();

  emit selectionChanged();
}


void CProjectHandler::setAction(int key, QAction *action, QActionGroup * group)
{
  _actions_map.insert(key, action);
  if (group)
    action->setActionGroup(group);

  switch(key)
  {
  case ActionSelector:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_selector(bool)));
    break;

  case ActionPen:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_pen(bool)));
    break;

  case ActionLasso:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_lasso(bool)));
    break;

  case ActionSquare:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_square(bool)));
    break;

  case ActionEllipse:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_ellipse(bool)));
    break;

  case ActionShowImage:
    action->setChecked(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_show_image(bool)));
    break;

  case ActionParamAngle:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_param_angle(bool)));
    break;

  case ActionParamInterval:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_param_interval(bool)));
    break;

  case ActionParamBasrelief:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_param_basrelief(bool)));
    break;

  case ActionParamThickness:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_param_thickness(bool)));
    break;

  case ActionParamThreshold:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_param_threshold(bool)));
    break;

  case ActionParamDottedLine:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_param_dotted_line(bool)));
    break;

  case ActionParamSmooth:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_action_param_smooth(bool)));
    break;

  case ActionUndo:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_undo_triggered(bool)));
    break;

  case ActionRedo:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_redo_triggered(bool)));
    break;

  case ActionTransform:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_transform_triggered(bool)));
    break;

  case ActionApplyTransform:
    //connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_apply_transform_triggered(bool)));
    break;

  case ActionGroup:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_group_triggered(bool)));
    break;

  case ActionUngroup:
    connect(action, SIGNAL(triggered(bool)), this, SLOT(_on_ungroup_triggered(bool)));
    break;

  }
}


QAction * CProjectHandler::action(int key)
{
  return _actions_map.value(key);
}


void CProjectHandler::_on_action_show_image(bool b)
{
  /*if (!_image_item)
  {
    _image_item = new QGraphicsPixmapItem(QPixmap::fromImage(_source_image));
    _image_item->setZValue(Z_VALUE_PIXMAP_ITEM);
    _scene->addItem(_image_item);
    _scene->setSceneRect(_image_item->boundingRect());
    _scene->update();
    _graphics_view->fitInView(_image_item, Qt::KeepAspectRatio);
  }

  _image_item->setVisible(b);*/

  _is_show_source_image = b;

  emit showSourceImage(b);
}



void CProjectHandler::_on_action_selector(bool)
{
  setTool(new CSelectionTool());
}

void CProjectHandler::_on_action_pen(bool)
{
}

void CProjectHandler::_on_action_lasso(bool)
{
  setTool(new CLassoTool());
}

void CProjectHandler::_on_action_square(bool)
{
  setTool(new CShapeTool(CShapeTool::Rectangle));
}


void CProjectHandler::_on_action_ellipse(bool)
{
  setTool(new CShapeTool(CShapeTool::Ellipse));
}



#include "build_number.h"

void CProjectHandler::initLog(QTextEdit * log_edit)
{
  _log_edit = log_edit;

  QDir dir_info(qApp->applicationDirPath());
  QFileInfo file_info(dir_info, "log.txt");
  //QFile::remove(file_info.absoluteFilePath());

  log("****************************************************************");
  log("*");
  log(QString("* StrokesMaker started %1").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm")));
#ifdef Q_OS_WIN
  log(QString("* build %1 %2").arg(BUILD_NUMBER_STR).arg("win"));
#endif
#ifdef Q_OS_MAC
  log(QString("* build %1 %2").arg(BUILD_NUMBER_STR).arg("mac"));
#endif
  log("*");
  log("****************************************************************");
  log(QString("Log file: %1").arg(file_info.absoluteFilePath()));
}


void CProjectHandler::log(const QString &log_str, bool) //no_cr)
{
  QMutex mutex;
  QMutexLocker l(&mutex);
  Q_UNUSED(l)

  if (_log_edit)
  {
    _log_edit->append(log_str);
    _log_edit->update();
  }

  qDebug() << log_str;

  QDir dir_info(qApp->applicationDirPath());
  QFileInfo file_info(dir_info, "log.txt");
  QFile log_file(file_info.absoluteFilePath());

  if (!log_file.open(QFile::Append))
    return;

  QTextStream text_stream(&log_file);
  text_stream << log_str << "\n";

  text_stream.flush();

  log_file.close();
}


void CProjectHandler::log(const QString &info1, const QString &info2)
{
  log(QString("%1 %2").arg(info1).arg(info2));
}


void CProjectHandler::log(const QString &info1, const QStringList &info2)
{
  log(QString("%1").arg(info1));
  for(const QString &s : info2)
  {
    log(QString("  %1").arg(s));
  }
}


void CProjectHandler::log(const QStringList &info2)
{
  for(const QString &s : info2)
  {
    log(QString("  %1").arg(s));
  }
}



void CProjectHandler::addMask(const QString &name, const CGraphics &mask)
{
  int index=0;
  QString mask_name=name;
  while(_masks_map.contains(mask_name))
  {
    mask_name = QString("%1-%2").arg(name).arg(++index);
  }

  _masks_map.insert(mask_name, mask);
}



CGraphics CProjectHandler::mask(const QString &name)
{
  return _masks_map.value(name);
}


void CProjectHandler::execute(QUndoCommand * comm)
{
  _undo_stack.push(comm);
}


void CProjectHandler::assignItemToObject(QGraphicsItem *, const CGraphics &)
{
//  _items_map.insert(object.id(), item);
}



/*
void CProjectHandler::_element_removed(const CShape &shape)
{
  if (_items_map.contains(shape.id()))
  {
    QGraphicsItem * shape_item = static_cast<CShapeItem*>(_items_map.value(shape.id()));
    _items_map.remove(shape.id());
    shape_item->hide();
    _trash_items << shape_item;
    _clear_items_trash();
  }
}
*/

void CProjectHandler::registerThread(QThread * thread)
{
  connect(thread, SIGNAL(started()), this, SLOT(_thread_started()), Qt::BlockingQueuedConnection);
  connect(thread, SIGNAL(finished()), this, SLOT(_thread_finished()), Qt::BlockingQueuedConnection);
}


void CProjectHandler::_thread_started()
{
  ++_thread_counter;
}


void CProjectHandler::_thread_finished()
{
  --_thread_counter;
  _clear_items_trash();
}


void CProjectHandler::_clear_items_trash()
{
  if (_thread_counter == 0)
  {
    for(QGraphicsItem * item : _trash_items)
    {
      delete item;
    }
    _trash_items.clear();
  }
}



void CProjectHandler::_can_undo_changed(bool b)
{
  QAction * a = action(ActionUndo);
  if (a)
    a->setEnabled(b);
}

void CProjectHandler::_can_redo_changed(bool b)
{
  QAction * a = action(ActionRedo);
  if (a)
    a->setEnabled(b);
}

void CProjectHandler::_on_undo_triggered(bool)
{
  _undo_stack.undo();
}

void CProjectHandler::_on_redo_triggered(bool)
{
  _undo_stack.redo();
}


void CProjectHandler::_history_index_changed(int)
{
  updateItems();

  updateControls();

  emit historyChanged();
}

bool CProjectHandler::getSelection(QList<CGraphics> &selection)
{
  selection.clear();

  for(QGraphicsItem * item : _scene->items())
  {
    if (!item->isSelected())
      continue;

    if (item->type() == CItemsTypes::ShapeItem)
    {
      CShapeItem * shape_item = static_cast<CShapeItem*>(item);
      selection << shape_item->shapeObject();
      continue;
    }
    if (item->type() == CItemsTypes::GroupItem)
    {
      CGroupItem * group_item = static_cast<CGroupItem*>(item);
      selection << group_item->groupObject();
      continue;
    }
  }
  return selection.count() > 0;
}


bool CProjectHandler::getHitItem(const QPointF &scene_pos, QGraphicsItem * &hit_item)
{
  for(QGraphicsItem * item : scene()->items())
  {
    if (item->contains(item->mapFromScene(scene_pos)))
    {
      hit_item = item;
      return true;
    }
  }
  return false;
}


class UnselectAll : public IVisitor
{
public:
  bool visitTo(const CGraphics &obj)
  {
  if (obj.type() == Layer)
  {
    obj.setSelected(false);
  } else {
    obj.setSelected(false);
    }
    return false;
  }
};


void CProjectHandler::unselectAll(QGraphicsItem * )
{
  UnselectAll u;
  _document.root().iterate(&u);
}


void CProjectHandler::select(const CGraphics &object, bool keep_selection)
{
  if (!keep_selection)
    unselectAll();

  object.setSelected(true);
}



void CProjectHandler::select(const QList<CGraphics> &selection, bool keep_selection)
{
  if (!keep_selection)
    unselectAll();

  for(const CGraphics &object : selection)
  {
    object.setSelected(true);
  }
}


void CProjectHandler::_validate_actions()
{
  QList<CGraphics> selection;
  bool has_selection = getSelection(selection);

  _actions_map[ActionTransform]->setEnabled(has_selection);

  _actions_map[ActionGroup]->setEnabled(commGroup::validate(selection));
  _actions_map[ActionUngroup]->setEnabled(commUngroup::validate(selection));
}


void CProjectHandler::_on_transform_triggered(bool)
{
/*  if (hasTransformControl())
    return;

  setTool(ToolSelector);

  CTransformControl * tc = new CTransformControl();
  tc->setZValue(Z_VALUE_CONTROLS + 1000);
  connect(tc, SIGNAL(transformStarted(QList<QGraphicsItem*>)),
          this, SLOT(_item_transform_started(QList<QGraphicsItem*>)));
  connect(tc, SIGNAL(transformFinished(QString, QList<QGraphicsItem*>)),
          this, SLOT(_item_transform_finished(QString, QList<QGraphicsItem*>)));

  _scene->addItem(tc);*/
}


/*
void CProjectHandler::applyTransform(const QList<QGraphicsItem*> &items)
{
  for(QGraphicsItem * item : items)
  {
    if (item->type() == ShapeItem)
    {
      CShapeItem * shape_item = static_cast<CShapeItem*>(item);
      CMask mask = shape_item->mask();
      mask.applyTransform();
    }
  }
}
*/


bool CProjectHandler::cancelTransform()
{
  CTransformControl * transform_ctrl = transformControl();
  if (transform_ctrl)
    return transform_ctrl->cancelTransform();

  return false;
}


void CProjectHandler::_on_apply_transform_triggered(bool)
{
  //applyTransform();
  //hprj->updateControls();
}


void CProjectHandler::updateControls()
{
  CTransformControl * transform_ctrl = transformControl();
  if (transform_ctrl)
    transform_ctrl->prepareTransform();
}



CTransformControl * CProjectHandler::transformControl() const
{
  for(QGraphicsItem * item : _scene->items())
  {
    if (item->type() == CTransformControl::TransformControl)
      return static_cast<CTransformControl*>(item);
  }
  return nullptr;
}



bool CProjectHandler::hasTransformControl() const
{
  return transformControl();
}


bool CProjectHandler::clearTransformControl()
{
  CTransformControl * transform_ctrl = transformControl();
  if (!transform_ctrl)
    return false;

  if (transform_ctrl->hasChanges())
  {
    transform_ctrl->finish();
  }

  delete transform_ctrl;

  return true;
}
/*

void CProjectHandler::_item_transform_started(const QList<QGraphicsItem*> &selection)
{
  QList<CGraphics> objects;
  for(QGraphicsItem * item : selection)
  {
    if (item->type() == ShapeItem)
    {
      CShapeItem * shape_item = static_cast<CShapeItem*>(item);
      objects << shape_item->shapeObject();
    } else
    if (item->type() == GroupItem)
    {
      CGroupItem * group_item = static_cast<CGroupItem*>(item);
      objects << group_item->groupObject();
    }

  }
  _transform_started(objects);
}


void CProjectHandler::_item_transform_finished(const QString &caption, const QList<QGraphicsItem*> &items)
{
  //applyTransform(items);
  _transform_finished(caption);
}
*/



void CProjectHandler::notifySceneMouseMove(const QPointF &scene_pos)
{
  _prev_hovered_item = _hovered_item;

  _hovered_item = nullptr;

  emit instance()->sceneMouseMove(scene_pos);

  if (_hovered_item)
  {
    _hovered_item->update();
  }

  if (_prev_hovered_item && _hovered_item != _prev_hovered_item)
  {
    _prev_hovered_item->update();
  }
}


void CProjectHandler::setHoveredItem(QGraphicsItem * item)
{
  if (hasTransformControl())
  {
    _hovered_item = nullptr;
    return;
  }

  if (!_hovered_item || item->zValue() > _hovered_item->zValue())
  {
    _hovered_item = item;
  }
}


class SearchTypes : public IVisitor
{
public:
  SearchTypes(Types t) : type(t) {}
  bool visitTo(const CGraphics &object)
  {
    if (object.type() == type && object.isSelected())
      objects << object;
    return false;
  }

  Types type;
  QList<CGraphics> objects;
};


bool CProjectHandler::getSelected(Types type, QList<CGraphics> &objects) const
{
  SearchTypes s(type);
  _document.root().iterate(&s);
  objects = s.objects;
  return objects.count() > 0;
}


void CProjectHandler::notifySelectionChanged()
{
  emit selectionChanged();
}


QString CProjectHandler::layoutFileName()
{
  QString config_path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  if (!QDir(config_path).exists())
  {
    QDir("").mkpath(config_path);
  }

  return QFileInfo(QDir(config_path), "layout.cfg").absoluteFilePath();
}


void CProjectHandler::saveLayout(QMainWindow *main_window)
{
  saveLayout(main_window, layoutFileName());

}


void CProjectHandler::loadLayout(QMainWindow *main_window)
{
  loadLayout(main_window, layoutFileName());
}



void CProjectHandler::saveLayout(QMainWindow*main_window, const QString& file_name)
{
  QFile file(file_name);
  if (!file.open(QFile::WriteOnly))
    return;

  QByteArray geo_data = main_window->saveGeometry();
  QByteArray layout_data = main_window->saveState();

  bool ok = file.putChar((uchar)geo_data.size());

  if (ok)
    ok = file.write(geo_data) == geo_data.size();

  if (ok)
    ok = file.write(layout_data) == layout_data.size();

  if (!ok)
  {
    QString msg = QString("Error writing to %1\n%2").arg(file_name).arg(file.errorString());
    qWarning() << msg;
  }
}


void CProjectHandler::loadLayout(QMainWindow*main_window, const QString& file_name)
{
  QFile file(file_name);

  if (!file.open(QFile::ReadOnly))
    return;

  uchar geo_size;
  QByteArray geo_data;
  QByteArray layout_data;

  bool ok = file.getChar((char*)&geo_size);

  if (ok)
  {
    geo_data = file.read(geo_size);
    ok = geo_data.size() == geo_size;
  }

  if (ok)
  {
    layout_data = file.readAll();
    ok = layout_data.size() > 0;
  }

  if (ok)
  {
    ok = main_window->restoreGeometry(geo_data);
  }

  if (ok)
  {
    ok = main_window->restoreState(layout_data);
  }

  if (!ok) {
    qDebug() << QString("Error reading %1").arg(file_name);
    return;
  }

}


void CProjectHandler::actionNewLayer()
{
  CLayer active_layer = _document.activeLayer();
  CLayer dest_layer = active_layer.owner();
  CLayer layer = CLayer::createInstance();

  commAddToLayer * comm_add = new commAddToLayer("New layer", dest_layer, layer, true);
  hprj->execute(comm_add);
}


void CProjectHandler::groupSelected()
{
  QList<CGraphics> selection;
  if (!getSelection(selection))
    return;

  if (!commGroup::validate(selection))
    return;

  commGroup * comm = new commGroup("Group", selection);
  execute(comm);
}




void CProjectHandler::ungroupSelected()
{
  QList<CGraphics> selection;
  if (!getSelection(selection))
    return;

  if (!commUngroup::validate(selection))
    return;

  commUngroup * comm = new commUngroup("Ungroup", selection.first());
  execute(comm);
}



void CProjectHandler::_on_group_triggered(bool)
{
  groupSelected();
}

void CProjectHandler::_on_ungroup_triggered(bool)
{
  ungroupSelected();
}


void CProjectHandler::_current_metric_changed(vxMetrics)
{

}


TxSelectionRec CProjectHandler::beginDragSelection(const QPointF &scene_pos, const QList<QGraphicsItem*> &selection)
{
  TxSelectionRec tx_rec;
  tx_rec.scene_begin_pos = scene_pos;

  QSet<QGraphicsItem*> items_set = selection.toSet();

  for(QGraphicsItem * item : selection)
  {
    QGraphicsItem * parent_item = item->parentItem();
    while(parent_item)
    {
      if (items_set.contains(parent_item))
        break;
      parent_item = parent_item->parentItem();
    }
    if (!parent_item)
    {
      tx_rec.selection_map.insert(item, QPair<QTransform, QTransform>(item->sceneTransform(), item->transform()));
    }
  }
  return tx_rec;
}


void CProjectHandler::dragSelection(const QPointF &scene_pos, TxSelectionRec &tx_rec)
{
  for(TxSelectionMap::iterator iter=tx_rec.selection_map.begin(); iter != tx_rec.selection_map.end(); ++iter)
  {
    QGraphicsItem * item = iter.key();
    QTransform item_to_scene_tx = iter.value().first;
    QTransform scene_to_item_tx = item_to_scene_tx.inverted();
    QPointF offset = scene_to_item_tx.map(scene_pos) -
                     scene_to_item_tx.map(tx_rec.scene_begin_pos);

    QTransform item_tx = iter.value().second;
    QTransform move_tx;
    move_tx.translate(offset.x(), offset.y());
    item->setTransform(move_tx*item_tx);
  }
  tx_rec.has_offset = true;
}


bool CProjectHandler::endDragSelection(TxSelectionRec &tx_rec)
{
  if (!tx_rec.has_offset)
    return false;

  QList<commSetTransform::CommPair> old_tx_list, new_tx_list;

  for(TxSelectionMap::iterator iter=tx_rec.selection_map.begin(); iter != tx_rec.selection_map.end(); ++iter)
  {
    QGraphicsItem * item = iter.key();
    CGraphics object;

    switch(item->type())
    {
    case ShapeItem:
      object = static_cast<CShapeItem*>(item)->shapeObject();
      break;
    case GroupItem:
      object = static_cast<CGroupItem*>(item)->groupObject();
      break;
    default:
      continue;
    }
    QTransform old_item_tx = iter.value().second;
    QTransform new_item_tx = item->transform();
    old_tx_list  << commSetTransform::CommPair(object, old_item_tx);
    new_tx_list  << commSetTransform::CommPair(object, new_item_tx);
  }

  if (old_tx_list.isEmpty())
    return false;

  commSetTransform * comm = new commSetTransform("move", old_tx_list, new_tx_list);
  execute(comm);

  return true;
}



bool CProjectHandler::readProject(const QString &file_name)
{
  QFile file(file_name);

  if (!file.open(QIODevice::ReadOnly))
  {
    qWarning() << "Can't create project" << file_name;
    return false;
  }

  QDomDocument doc;

  QString errorStr;
  int errorLine;
  int errorColumn;

  if (!doc.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
    qWarning() << QString("Parse error at line %1, column %2:\n%3")
                       .arg(errorLine)
                       .arg(errorColumn)
                       .arg(errorStr);
    return false;
  }

  QDomElement project_elm = doc.firstChildElement(TAG_PROJECT);

  CProject project = CModel::readObject(project_elm);
  if (!project)
    return false;

  CDocument document = project.document();
  document.setFileName(file_name);

  setDocument(document);

/*  CParserSTRX parser(file_name);

  QList<CGraphics> objects;

  CLayer layer = CLayer::createInstance();
  if (!parser.parseTo(layer))
    return false;

  setDocument(parser.document);

  objects = layer.objects();

  CLayer root = _document.root();
  for(const CGraphics &object : objects)
  {
    root.add(object);
    emit objectAdded(object, root);
  }*/

  emit projectReaded(hprj->document());

  return true;
}




void CProjectHandler::_load_image_file(const QString &file_name)
{
  loadImage(file_name);
}

void CProjectHandler::_load_qimage(const QImage &image)
{
  loadImage(image);
}


bool CProjectHandler::hasUnsavedChanges()
{
  return _undo_stack.index() != _save_history_ix;
}


bool CProjectHandler::doSaveProject()
{
  _save_history_ix = _undo_stack.index();
  return true;
}


bool CProjectHandler::doCheckSaveChanges()
{
  if (!hasUnsavedChanges())
    return true;

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setText("The document has been modified.");
  msgBox.setInformativeText("Do you want to save your changes?");
  msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Save);

  int responce = msgBox.exec();

  if (responce == QMessageBox::Cancel)
    return false;

  if (responce == QMessageBox::Discard)
    return true;

  return doSaveProject();
}



bool CProjectHandler::doOpenFile()
{
  if (!doCheckSaveChanges())
    return false;

  QSettings settings;
  QString file_name;
  {
    QFileDialog file_dlg(nullptr, tr("Open"), "",
                         tr("All (*.smpx;*.strx;*.str;*.smp;*.bmp;*.png;*.jpg;);;"
                            "Engraving Projects (*.smpx;*.strx;*.str;*.smp;);;"
                            "Images (*.bmp;*.png;*.jpg;)"));

    file_dlg.setFileMode(QFileDialog::ExistingFiles);

    QString presets_path = settings.value(ATTR_PROJECTS_PATH, "").toString();

    if (presets_path != "")
      file_dlg.setDirectory(presets_path);

    if (!file_dlg.exec())
      return false;

    file_name = file_dlg.selectedFiles().first();
  }

  if (QRegExp("\\b(.smpx|.strx|.str|.smp)\\b").indexIn(file_name) > 0)
  {
    settings.setValue(ATTR_PROJECTS_PATH, QFileInfo(file_name).path());
    return readProject(file_name);
  }

  if (QRegExp("\\b(.png|.jpg|.jpeg|.bmp)\\b").indexIn(file_name) > 0)
  {
    settings.setValue(ATTR_SOURCE_IMAGE_PATH, QFileInfo(file_name).absolutePath());
    return loadImage(file_name);
  }

  return false;
}


void CProjectHandler::setDocument(const CGraphics &doc)
{
  _document = doc;
  _undo_stack.clear();
  emit documentChanged(doc);

  CMetric::setCurrentMetric(_document.metric());
  setTool(new CSelectionTool());
}


bool CProjectHandler::doNewProject()
{
  if (!doCheckSaveChanges())
    return false;

  NewProjectDlg new_proj;
  if (new_proj.exec() != QDialog::Accepted)
    return false;

  setDocument(CDocument(new_proj.docSize(), new_proj.docResolution(), new_proj.docMetric()));
  return true;
}



void CProjectHandler::updateRecentFilesSettings()
{
  QString current_file_name = hprj->document().fileName();
  _recent_files.clear();

  _recent_files.append(current_file_name);

  //read from settings
  QSettings settings;
  {
    int files_cnt = settings.beginReadArray("recent_files");

    QSet<QString> used_files;
    used_files.insert(current_file_name);

    for(int i=0; i < files_cnt; ++i)
    {
      settings.setArrayIndex(i);

      QString file_name = settings.value("project_name").toString();

      if (used_files.contains(file_name))
        continue;

      _recent_files.append(file_name);

      used_files.insert(file_name);
    }

    settings.endArray();
  }

  //write to settings
  {
    settings.beginWriteArray("recent_files");
    int recent_ix=0;
    for(int i=0; i < _recent_files.count(); ++i)
    {
      QString project_name = _recent_files.at(i);

      if (!QFileInfo(project_name).exists())
        continue;

      settings.setArrayIndex(recent_ix++);
      settings.setValue("project_name", project_name);

      if (recent_ix > 25)
        break;
    }
    settings.endArray();
  }
}


void CProjectHandler::_recent_menu_triggered(QAction*action)
{
  if (!doCheckSaveChanges())
    return;

  QString file_name = action->data().toString();
  readProject(file_name);
}


void CProjectHandler::buildRecentFilesMenu(QMenu * menu)
{
  QSettings settings;

  menu->clear();

  int files_cnt = settings.beginReadArray("recent_files");

  if (files_cnt == 0)
    menu->setEnabled(false);
  else
    menu->setEnabled(true);

  QSet<QString> used;

  QFontMetrics fm(menu->font());

  for(int i=0; i < files_cnt; ++i)
  {
    settings.setArrayIndex(i);

    QString file_name = settings.value("project_name").toString();

    if (used.contains(file_name))
      continue;

    used.insert(file_name);

    if (!QFileInfo(file_name).exists())
      continue;

    QString path = fm.elidedText(QFileInfo(file_name).path(), Qt::ElideMiddle, 200);

    QAction * action = menu->addAction(QString("%1/%2").arg(path).arg(QFileInfo(file_name).fileName()));
    action->setData(file_name);
  }

  disconnect(menu, SIGNAL(triggered(QAction*)), this, SLOT(_recent_menu_triggered(QAction*)));
  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(_recent_menu_triggered(QAction*)));


  settings.endArray();
}


void CProjectHandler::_open_project(const QString &project_name)
{
  readProject(project_name);
}
