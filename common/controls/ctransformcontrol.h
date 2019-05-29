#ifndef CTRANSFORMCONTROL_H
#define CTRANSFORMCONTROL_H

#include "fltransformframe.h"

#include <QGraphicsObject>


class CTransformControl: public QGraphicsObject
{
  Q_OBJECT

public:
  enum
  {
    TransformControl = QGraphicsItem::UserType + 1001
  };

  enum TransformSelectionMode
  {
    NoSelected,
    ActiveShape,
    SelectedShapes,
    SelectenConturs
  };

  CTransformControl();

  int type() const override
  {
    return TransformControl;
  }

  void prepareTransform();

  bool cancelTransform();

  bool hasChanges() const;

  void finish();

  QList<QGraphicsItem*> selection() const
  {
    return _selection;
  }

protected:
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  QVariant itemChange(GraphicsItemChange, const QVariant &) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *) override;

  QRectF boundingRect() const override;

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;


private:
  static QCursor _cursor_move;
  static QCursor _cursor_rotate;
  static QCursor _cursor_shear;

  static QCursor _cursors_scale[4];
  static QCursor _cursors_rotate[8];
  static bool _cursors_loaded;

  int _cursor_id;
  QCursor _prev_cursor;

  QList<QGraphicsItem*> _handler_items;

  bool _hovered;

  QPointF _scene_pos, _start_pos, _rotate_center;

  double _started_angle;
  double _angle;

  QList<QGraphicsItem*> _selection;

  TransformFrame _ini_tf, _tf;
  TransformFrameControl _hovered_control;

  QMap<QGraphicsItem*, QTransform> _ini_tx_map;

  QPen _frame_pen;


  QRectF _get_selection_rect() const;

  double _vector_t;

  bool _rotation_enabled=false, _projection_enabled=false;

  TransformSelectionMode _selection_mode;

  QPolygonF _initial_scene_plgn;

  double _view_scale=1;

  QMap<QGraphicsItem*, QTransform> _item_tx_map, _item_to_scene_tx_map;

  QString _tx_caption;

  void _draw_size_controls(QPainter * pp);
  void _draw_size_control(uint control, QPainter * pp);

  void _set_control_positions(const QPointF& canvas_pos,
                              bool reflection, bool proportional, bool projection, bool fixed);

  QPointF _get_control_pos(uint control);
  bool _check_hover_control(TransformFrameControl, const QPointF&);
  bool _can_accept_hover(QGraphicsSceneHoverEvent*);
  bool _check_controls_hover(const QPointF&);

  bool _update_transform_frame_data();

  void _notify_changes();

private slots:
  void _selection_changed();

signals:
  void transformStarted(const QList<QGraphicsItem*>&);
  void transformFinished(const QString&, const QList<QGraphicsItem*>&);

};

#endif // CTRANSFORMCONTROL_H
