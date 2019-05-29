#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "cstrokestemplate.h"
#include "ctools.h"
#include "toolbar.h"
#include "cdocument.h"

#include <QTimer>
#include <QSettings>

class CEngravingItem;

using namespace sm;


class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);


protected:
  void closeEvent(QCloseEvent * event);

private:

  void _save_layout();
  void _load_layout(bool,bool);

  QGraphicsScene * _scene;
  QImage _image;

  bool _initialized=false;

  bool _docks_created=false;

  // docs
  ToolBar * _tool_bar=nullptr;
  QDockWidget * _tool_bar_dock;
  QDockWidget * _debug_dock;
  QDockWidget * _layers_dock;
  QDockWidget * _console_dock;
  QDockWidget * _history_dock;

  // status bar
  QWidget *_progress_bar;
  QLabel *_status_info;
  QLabel *_status_path;
  QLabel *_status_edit;
  QLabel *_status_resolution;


  void _create_docks(bool);
  void _create_status_bar();
  void _adjust_status_bar(bool);
  QLabel * _create_status_label();

  QDomElement _saved_elm;

private slots:
  void _create_items();

  void _show_workspace();
  void _show_welcome();
  void _image_loaded(const QImage &);
  void _tool_changed(CAbstractTool*);
  void _check_args();
  void _document_changed(const CDocument &);
  void _metric_changed(vxMetrics);

  void on_actionConsole_triggered(bool checked);

  void on_layer_dock_visibility_changed(bool);
  void on_history_dock_visibility_changed(bool);
  void on_console_dock_visibility_changed(bool);
  void on_debug_dock_visibility_changed(bool);

  void on_actionDebug_triggered(bool checked);
  void on_actionLayers_triggered(bool checked);
  void on_actionHistory_triggered(bool checked);
  void on_pushButton_clicked();
  void on_pushButton_2_clicked();
  void on_actionOpen_triggered();
  void on_actionNew_triggered();
};

#endif // MAINWINDOW_H
