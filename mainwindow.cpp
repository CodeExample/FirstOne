  #include "mainwindow.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QTime>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QOffscreenSurface>
#include <QThread>

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QFileDialog>
#include <QGraphicsWidget>
#include <QSettings>
#include <QDomDocument>

#include "coglpainterthread.h"

#include "cshape.h"

#include "cprojecthandler.h"
#include "citemscontroller.h"
#include "cmask.h"
#include "chandtool.h"
#include "layersform.h"
#include "historyform.h"
#include "debugwidget.h"
#include "cenvironment.h"
#include "ccommands.h"
#include "cshapeitem.h"
#include "glgraphicsscene.h"
using namespace sm;



MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  setupUi(this);

  _scene = new GLGraphicsScene();

  workspaceWidget->init(_scene);

  hprj->init(_scene, workspaceWidget->view());
  itmc->init(_scene, workspaceWidget->view());

  QActionGroup * tools_group=new QActionGroup(this);

  hprj->setAction(ActionSelector, actionSelector, tools_group);
  hprj->setAction(ActionPen, actionPen, tools_group);
  hprj->setAction(ActionLasso, actionLasso, tools_group);
  hprj->setAction(ActionSquare, actionSquare, tools_group);
  hprj->setAction(ActionEllipse, actionEllipse, tools_group);

  hprj->setAction(ActionShowImage, actionShow_Image);

  hprj->setAction(ActionUndo, actionUndo);
  hprj->setAction(ActionRedo, actionRedo);

  hprj->setAction(ActionGroup, actionGroup);
  hprj->setAction(ActionUngroup, actionUngroup);

  hprj->setAction(ActionTransform, actionTransform);

  connect(hprj, SIGNAL(requestShowWorkspace()), this, SLOT(_show_workspace()));
  connect(hprj, SIGNAL(requestShowWelcome()), this, SLOT(_show_welcome()));
  connect(hprj, SIGNAL(imageLoaded(QImage)), this, SLOT(_image_loaded(QImage)));
  connect(hprj, SIGNAL(toolChanged(CAbstractTool*)), this, SLOT(_tool_changed(CAbstractTool*)));
  connect(hprj, SIGNAL(documentChanged(CDocument)), this, SLOT(_document_changed(CDocument)));

  connect(metr, SIGNAL(metricChanged(vxMetrics)), this, SLOT(_metric_changed(vxMetrics)));

  hprj->buildRecentFilesMenu(menuRecent_Files);

  _create_docks(false);

  _create_status_bar();

  _load_layout(true, false);

  _show_welcome();


  QTimer::singleShot(200, this, SLOT(_check_args()));

}


void MainWindow::_document_changed(const CDocument &document)
{
  _adjust_status_bar(_progress_bar->isVisible());
  hprj->updateRecentFilesSettings();
  hprj->buildRecentFilesMenu(menuRecent_Files);
/*  if (vEnvironment::projectFileName().isEmpty() || _reading_project_started)
  {
    setWindowTitle(QString("%1 - [new project]%3")
                   .arg(APPLICATION_TITLE)
                   .arg(_gl_widget->hasUnsavedChanges() ? "*" : ""));

    _status_path->setText("");
    _status_resolution->setText("");

  } else {

    setWindowTitle(QString("%1- %3%4")
                   .arg(APPLICATION_TITLE)
                   .arg(vEnvironment::projectFileName())
                   .arg(_gl_widget->hasUnsavedChanges() ? "*" : ""));

    _status_path->setText(_status_path->fontMetrics().elidedText(
                            vEnvironment::projectFullFileName(), Qt::ElideMiddle, _status_path->width()));

  }

  if (_gl_widget->document() && !_reading_project_started)
  {
    vDocument doc = _gl_widget->document();

    _status_resolution->setText(QString("%1x%2px, %3x%4mm, %5dpi")
                                .arg(doc.documentPict().image().width())
                                .arg(doc.documentPict().image().height())
                                .arg(doc.sizeMM().width())
                                .arg(doc.sizeMM().height())
                                .arg(doc.resolutionDPI(), 0, 'f',0));

  } else {
    _status_resolution->setText("");
  }

  _project_handler->updateRecentFilesSettings();

  _project_handler->buildRecentFilesMenu(ui->menuRecent_files);
*/

  //QSettings settings(ATTR_APPLICATION_CONFIG, ATTR_PROJECT_CONFIG);
  //ui->statusBar->setVisible(settings.value(ATTR_STATUSBAR_VISIBLE, "0").toBool());

  if (document)
  {
    double width_mm = metr->convertPxToMetric(metr->currentMetric(), 72, document.size().width());
    double height_mm = metr->convertPxToMetric(metr->currentMetric(), 72, document.size().height());

    double width = _image.width();
    double height = _image.height();

    QString metric_str = metr->getMetricTitle(metr->currentMetric());

    _status_resolution->setText(QString("%1x%2px, %3x%4%5, %6dpi")
                                .arg(width)
                                .arg(height)
                                .arg(width_mm)
                                .arg(height_mm)
                                .arg(metric_str)
                                .arg(document.resolutionDPI()));

  } else {
    _status_resolution->setText("");
  }

}


void MainWindow::_metric_changed(vxMetrics)
{
  _document_changed(hprj->document());
}


void MainWindow::_image_loaded(const QImage &image)
{
  _image = image;
  //_update_status_bar();
}


void MainWindow::_show_workspace()
{
  stackedWidget->setCurrentIndex(1);

  QTimer::singleShot(100, this, SLOT(_create_items()));

  _load_layout(false, true);

  _tool_bar_dock->setFloating(false);
  _tool_bar_dock->setVisible(true);
}


void MainWindow::_show_welcome()
{
  _tool_bar_dock->setVisible(false);

  stackedWidget->setCurrentIndex(0);
}


void MainWindow::closeEvent(QCloseEvent *)
{
  _save_layout();
/*  if (_checkProjectSaved())
  {
    event->accept();
    _save_layout();
  } else {
    event->ignore();
  }
  on_actionSelector_triggered();*/
}


void MainWindow::_load_layout(bool restore_geom, bool restore_state)
{
  Q_UNUSED(restore_geom)
  Q_UNUSED(restore_state)
  hprj->loadLayout(this);
}


void MainWindow::_save_layout()
{
  hprj->saveLayout(this);
}


void MainWindow::_create_docks(bool visible)
{
  if (_docks_created)
    return;

  _docks_created = true;

  // toolbar
  _tool_bar = new ToolBar(this);
  _tool_bar_dock = new QDockWidget(this, Qt::FramelessWindowHint);
  _tool_bar_dock->setObjectName("ToolBar");
  _tool_bar_dock->setVisible(visible);
  _tool_bar_dock->setTitleBarWidget(new QWidget());
  _tool_bar_dock->setAllowedAreas(Qt::TopDockWidgetArea);
  _tool_bar_dock->setWidget(_tool_bar);
  _tool_bar_dock->setMaximumHeight(54);
  _tool_bar_dock->setMinimumHeight(54);
  _tool_bar_dock->setFeatures(0);

  addDockWidget(Qt::TopDockWidgetArea, _tool_bar_dock);

  // layers
  QWidget * layers_widget = new LayersForm(this);
  _layers_dock = new QDockWidget("Layers", this);
  _layers_dock->setObjectName("LayersDock");
  _layers_dock->setVisible(visible);
  _layers_dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
  _layers_dock->setWidget(layers_widget);

  connect(_layers_dock, SIGNAL(visibilityChanged(bool)),
          this, SLOT(on_layer_dock_visibility_changed(bool)));

  addDockWidget(Qt::LeftDockWidgetArea, _layers_dock);

  // history
  QWidget * history_widget = new HistoryForm(this);
  _history_dock = new QDockWidget("History", this);
  _history_dock->setObjectName("HistoryDock");
  _history_dock->setVisible(false);
  _history_dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
  _history_dock->setWidget(history_widget);

  connect(_history_dock, SIGNAL(visibilityChanged(bool)),
          this, SLOT(on_history_dock_visibility_changed(bool)));

  addDockWidget(Qt::LeftDockWidgetArea, _history_dock);

  // debug
  QWidget * debug_widget = new DebugWidget(this);
  _debug_dock = new QDockWidget("Debug", this);
  _debug_dock->setObjectName("DebugDock");
  _debug_dock->setVisible(false);
  _debug_dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
  _debug_dock->setWidget(debug_widget);

  connect(_debug_dock, SIGNAL(visibilityChanged(bool)),
          this, SLOT(on_debug_dock_visibility_changed(bool)));

  addDockWidget(Qt::RightDockWidgetArea, _debug_dock);

  // console
  _console_dock = new QDockWidget("Console", this);
  _console_dock->setObjectName("Console");
  _console_dock->setVisible(false);
  QTextEdit * info_view = new QTextEdit(_console_dock);
  _console_dock->setWidget(info_view);
  _console_dock->setAllowedAreas(Qt::AllDockWidgetAreas);

  connect(_console_dock, SIGNAL(visibilityChanged(bool)),
          this, SLOT(on_console_dock_visibility_changed(bool)));

  addDockWidget(Qt::BottomDockWidgetArea, _console_dock);
  hprj->initLog(info_view);

}



void MainWindow::on_layer_dock_visibility_changed(bool b)
{
  actionLayers->setChecked(b);
}

void MainWindow::on_history_dock_visibility_changed(bool b)
{
  actionHistory->setChecked(b);
}


void MainWindow::on_console_dock_visibility_changed(bool b)
{
  actionConsole->setChecked(b);
}

void MainWindow::on_debug_dock_visibility_changed(bool b)
{
  actionDebug->setChecked(b);
}

void MainWindow::on_actionDebug_triggered(bool b)
{
  _debug_dock->setVisible(b);
}

void MainWindow::on_actionConsole_triggered(bool b)
{
  _console_dock->setVisible(b);
}


void MainWindow::on_actionLayers_triggered(bool b)
{
  _layers_dock->setVisible(b);
}


void MainWindow::on_actionHistory_triggered(bool b)
{
  _history_dock->setVisible(b);
}

void MainWindow::_adjust_status_bar(bool progress_bar_visible)
{
  int w = width();
  int w3 = w/3;
  int status_edit_width = 20;
  int _status_mode_width = 150;

  _status_edit->setMaximumWidth(status_edit_width);
  _status_edit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  _status_path->setMinimumWidth(w3 - status_edit_width);
  _status_path->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

  _status_resolution->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  if (progress_bar_visible)
  {
    _status_info->setMinimumWidth(_status_mode_width);
    _status_info->setMaximumWidth(_status_mode_width);
    _status_info->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    _progress_bar->setMaximumWidth(w3 - _status_mode_width);
    _progress_bar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  } else {
    _status_info->setMaximumWidth(w3);
    _status_info->setMinimumWidth(w3);
    _status_info->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  }
}


QLabel * MainWindow::_create_status_label()
{
  QLabel * label = new QLabel("");
  label->setContentsMargins(0, 1, 0, 0);
  label->setFont(QFont("Proxima Nova", 12));
  return label;
}


void MainWindow::_create_status_bar()
{
  QStatusBar *status_bar = static_cast<QMainWindow*>(this)->statusBar();

  _status_edit = _create_status_label();
  _status_path = _create_status_label();
  _status_info = _create_status_label();
  _status_resolution = _create_status_label();

  _progress_bar = new QWidget(this);
  _progress_bar->setMaximumHeight(10);
  _progress_bar->setFont(QFont("Proxima Nova", 12));


  _status_edit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  _status_edit->setText(tr("    "));
  _status_edit->setToolTip(tr(""));
  _status_info->setAlignment(Qt::AlignRight |
                             Qt::AlignVCenter);

  _status_resolution->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

  _status_resolution->setText( tr("") );
  _status_resolution->setToolTip( tr("Project resolution"));

  _status_path->setAlignment( Qt::AlignLeft | Qt::AlignVCenter);
  _status_path->setText( tr("") );
  _status_path->setToolTip( tr("Path to project") );

  //_progress_bar->setTextVisible(false);
  //_progress_bar->setRange( 0, 100 );

  _status_edit->setMinimumWidth(20);
  _status_path->setMinimumWidth(300);
  _status_resolution->setMinimumWidth(100);

  status_bar->addWidget(_status_edit, 0);
  status_bar->addWidget(_status_path, 1);
  status_bar->addWidget(_status_resolution, 2);
  status_bar->addWidget(_status_info, 3);
  status_bar->addWidget(_progress_bar, 4);

  _progress_bar->setVisible(false);

  _adjust_status_bar(false);
}

void MainWindow::on_pushButton_clicked()
{
  CLayer layer = CLayer::createInstance();
/*
    CStrokesElement strokes = CStrokesElement::createInstance();
    CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
    tmpl.data()->image = _image;
    tmpl.data()->bounding_rect = tmpl.data()->create_rect = QRectF(92,26,100,100);

    tmpl.data()->name = "top";
    tmpl.data()->is_knife = true;
    tmpl.data()->is_edged = false;
    tmpl.data()->black =  0.05;
    tmpl.data()->white = 0.15;
    tmpl.data()->dotline_threshold = 0.55;
    tmpl.data()->round_cups = true;
    tmpl.data()->threshold = 0.75;

    tmpl.data()->angle = 32;
    tmpl.data()->item_angle = 0;

    strokes.add(tmpl);
    layer.add(strokes);
*/

  for(int i=0; i < 21; ++i)
  {
    QPainterPath mask_path;
    mask_path.addEllipse(0,0,100,100);
    QRectF r(rand()%800,rand()%600,100,100);
    mask_path.translate(r.left(), r.top());

    CShape strokes = CShape::createInstance();

    strokes.setMask(CMask(mask_path));

    CStrokesTemplate tmpl = CStrokesTemplate::createInstance();

    tmpl.data()->name = "top";
    tmpl.data()->is_knife = true;
    tmpl.data()->is_edged = false;
    tmpl.data()->black =  0.25;
    tmpl.data()->white = 0.05;
    tmpl.data()->dotline_threshold = 0;
    tmpl.data()->round_cups = true;
    tmpl.data()->threshold = 0.3;

    tmpl.data()->angle = (rand()%180);
    tmpl.data()->item_angle = 0;


    QTransform tx;
    tx.rotate(45);
    tmpl.setTransform(tx);

    strokes.add(tmpl);
    layer.add(strokes);

  }
  commAddToLayer * comm = new commAddToLayer("Add", layer, true);
  hprj->execute(comm);

  hprj->updateItems();
}


void MainWindow::_tool_changed(CAbstractTool *tool)
{
  switch(tool->toolType())
  {
  case ToolSelector:
    actionSelector->setChecked(true);
    break;

  case ToolLasso:
    actionLasso->setChecked(true);
    break;


  case ToolRectangle:
    actionSquare->setChecked(true);
    break;

  case ToolEllipse:
    actionEllipse->setChecked(true);
    break;
  }
}



void MainWindow::_create_items()
{
  _initialized = true;
  return;
  const CGroup group = CGroup::createInstance();
  {
    QPainterPath mask_path;
    mask_path.addEllipse(10,10,333,333);

    CShape shape = CShape::createInstance();
    shape.setMask(mask_path);
    //group.add(shape);
    //emit hprj->objectAdded(shape, group);


    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
      tmpl.data()->bounding_rect = tmpl.data()->create_rect = QRect(10,10,333,333);
      tmpl.data()->name = "top";
      tmpl.data()->angle = 0;
      tmpl.data()->black =  0.05;
      tmpl.data()->white = 0.15;
      tmpl.data()->interval = 7;
      tmpl.data()->dotline_threshold=0.55;
      tmpl.data()->item_angle = 0;
      tmpl.data()->is_knife = true;
      tmpl.data()->is_edged = false;
      tmpl.data()->thickness = 0.9;

      shape.add(tmpl);
    }

    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
      tmpl.data()->bounding_rect = tmpl.data()->create_rect = QRect(10,10,333,333);
      tmpl.data()->name = "top";
      tmpl.data()->angle = 45;
      tmpl.data()->black =  0.50;
      tmpl.data()->white = 0.950;
      tmpl.data()->interval = 7;
      //tmpl.data()->dotline_threshold=0.55;
      tmpl.data()->item_angle = 0;
      tmpl.data()->is_knife = false;
      tmpl.data()->is_edged = true;
      tmpl.data()->is_cell = true;

      tmpl.data()->thickness = 0.9;
      tmpl.data()->cell_size = 0.5;

      shape.add(tmpl);
    }

    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
      tmpl.data()->bounding_rect = tmpl.data()->create_rect = QRect(10,10,333,333);
      tmpl.data()->name = "top";
      tmpl.data()->angle = 45;
      tmpl.data()->black =  0.50;
      tmpl.data()->white = 0.950;
      tmpl.data()->interval = 7;
      tmpl.data()->item_angle = 0;
      tmpl.data()->is_knife = false;
      tmpl.data()->is_edged = false;
      tmpl.data()->line_position = 0.5;
      tmpl.data()->thickness = 0.9;
      tmpl.data()->cell_size = 0.5;

      shape.add(tmpl);
    }

    commAddToLayer * comm = new commAddToLayer("Add shape", shape, true);
    hprj->execute(comm);

  }


  {
    QPainterPath mask_path;
    mask_path.addEllipse(310,110,133,133);

    CShape shape = CShape::createInstance();
    shape.setMask(mask_path);
    //group.add(shape);
    //emit hprj->objectAdded(shape, group);

    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
      tmpl.data()->bounding_rect = tmpl.data()->create_rect = QRect(10,10,333,333);
      tmpl.data()->name = "top";
      tmpl.data()->angle = 0;
      tmpl.data()->black =  0.05;
      tmpl.data()->white = 0.15;
      tmpl.data()->interval = 7;
      tmpl.data()->dotline_threshold=0.55;
      tmpl.data()->item_angle = 0;
      tmpl.data()->is_knife = true;
      tmpl.data()->is_edged = false;
      tmpl.data()->thickness = 0.9;

      shape.add(tmpl);
    }

    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
      tmpl.data()->bounding_rect = tmpl.data()->create_rect = QRect(10,10,333,333);
      tmpl.data()->name = "top";
      tmpl.data()->angle = 45;
      tmpl.data()->black =  0.50;
      tmpl.data()->white = 0.950;
      tmpl.data()->interval = 7;
      tmpl.data()->item_angle = 0;
      tmpl.data()->is_knife = false;
      tmpl.data()->is_edged = true;
      tmpl.data()->is_cell = true;

      tmpl.data()->thickness = 0.9;
      tmpl.data()->cell_size = 0.5;

      shape.add(tmpl);
    }

    {
      CStrokesTemplate tmpl = CStrokesTemplate::createInstance();
      tmpl.data()->bounding_rect = tmpl.data()->create_rect = QRect(10,10,333,333);
      tmpl.data()->name = "top";
      tmpl.data()->angle = 45;
      tmpl.data()->black =  0.50;
      tmpl.data()->white = 0.950;
      tmpl.data()->interval = 7;
      tmpl.data()->item_angle = 0;
      tmpl.data()->is_knife = false;
      tmpl.data()->is_edged = false;
      tmpl.data()->line_position = 0.5;
      tmpl.data()->thickness = 0.9;
      tmpl.data()->cell_size = 0.5;

      shape.add(tmpl);
    }

    commAddToLayer * comm = new commAddToLayer("Add shape", shape, true);
    hprj->execute(comm);
  }

  //commAddToLayer * comm = new commAddToLayer("Add group", group, true);
  //hprj->execute(comm);

  emit hprj->documentChanged(hprj->document());
}


void MainWindow::on_pushButton_2_clicked()
{
  QList<CGraphics> selection;
  if (hprj->getSelection(selection))
  {
    if (_saved_elm.isNull())
    {
      QDomDocument doc("root");
      _saved_elm = doc.createElement("object");
      selection.first().write(_saved_elm, doc);
    } else {
      selection.first().read(_saved_elm);
      hprj->updateItems();
    }
  }
}




void MainWindow::_check_args()
{
  QString file_name;
  if (qApp->arguments().count() > 1 &&
      QFileInfo(qApp->arguments().at(1)).exists())
  {
    file_name=qApp->arguments().at(1);
  }
  if (QFileInfo(file_name).exists())
  {
    hprj->loadImage(file_name);
  } else {
    hprj->loadImage(":/examples/resources/examples/nifertiti_m.jpg");
  }
}


void MainWindow::on_actionOpen_triggered()
{
  hprj->doOpenFile();
}

void MainWindow::on_actionNew_triggered()
{
  hprj->doNewProject();
}
