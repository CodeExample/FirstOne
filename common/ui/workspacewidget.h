#ifndef WORKSPACEWIDGET_H
#define WORKSPACEWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QTime>
#include <QScrollBar>

#include "rullerdecorator.h"
#include "cdocument.h"

namespace Ui {
class WorkspaceWidget;
}

using namespace  sm;


class WorkspaceWidget :
    public QWidget,
    private RulerInfo
{
  Q_OBJECT

public:
  explicit WorkspaceWidget(QWidget *parent = 0);
  ~WorkspaceWidget();

  void init(QGraphicsScene *);

  QGraphicsView * view() const;

  void hideRulers();
  void showRulers();
  QImage getSceneScreen() const;

  double scale() const;

  void setInverseRuler(double w, double h);

  bool isFocused()
  {
    return _focused;
  }

  void zoomToFit();

protected:
  bool eventFilter(QObject *, QEvent *);

private:
  Ui::WorkspaceWidget *ui;
  vxRulerDecorator * _horz_ruler;
  vxRulerDecorator * _vert_ruler;
  vxRulerDecorator * _center_ruler;

  bool _mouse_move;
  bool _alt_pressed;
  double _wheel_dlt;
  double _prev_delta;
  QTime _wheel_time;
  bool _focused=false;

  QWidget * viewport();
  QPointF mapToScene(const QPoint&) const;

  void _increase_zoom_to(double d, const QPointF& scene_pos);
  void _adjust_scrollbars();
  bool _scrollbar_action(QScrollBar*,int action);

private slots:

  void _metric_changed(vxMetrics);
  void _active_project_changed();

  void _project_readed(const CDocument&);

  void on_vertScrollBar_sliderMoved(int position);
  void on_horzScrollBar_sliderMoved(int position);
  void on_vertScrollBar_actionTriggered(int action);
  void on_horzScrollBar_actionTriggered(int action);
  void on_comboBox_activated(int index);
};

#endif // WORKSPACEWIDGET_H
