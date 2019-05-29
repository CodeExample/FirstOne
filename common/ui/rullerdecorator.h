#ifndef VXRULLERDECORATOR_H
#define VXRULLERDECORATOR_H

#include <QWidget>
#include <QPen>

#include "cenvironment.h"

#define RULER_HEIGHT   16

class RulerInfo
{
public:
  virtual QWidget * viewport() =0;
  virtual QPointF mapToScene(const QPoint&) const =0;

  QRectF mapToScene(const QRect &r) const
  {
    return QRectF(mapToScene(r.topLeft()), mapToScene(r.bottomRight()));
  }

  virtual double scale() const =0;

};


class vxRulerDecorator : public QWidget
{
  Q_OBJECT

public:
  enum Orientation
  {
    Center      = 0x0,
    Horizontal,
    Vertical
  };

  vxRulerDecorator(QWidget*, RulerInfo*, Orientation);

  void setCurrentDimension(double, const QString&);
  void setCurrentDimension(double, double divider, const QString&, double value_mul=1);

  void setInverseRelative(double);

  void setCursorPos(double);

  void setStyle(const QBrush&, const QBrush&, const QPen&, const QPen&, const QPen&);

  void setBrushBody(QBrush b)
  {
    _brushBody = b;
  }

  void setBrushCorner(QBrush b)
  {
    _brushCorner = b;
  }

  void setSidePen(QPen p)
  {
    _sideLinePen = p;
  }

  void setTickPen(QPen p)
  {
    _tickPen = p;
  }

  void setTextPen(QPen p)
  {
    _textPen = p;
  }


protected:
  void paintEvent(QPaintEvent *);

  bool eventFilter(QObject *, QEvent *);

private:

  RulerInfo * _ruler_info;
  QWidget * _view_port;
  Orientation _orientation;
  double _dimension_v;
  double _divider;
  double _value_mul;

  double _inverse_value;
  double _cursor_pos;

  QString _dimension_text;

  QBrush _brushBody;
  QBrush _brushCorner;
  QPen _sideLinePen;
  QPen _tickPen;
  QPen _textPen;

  void _drawScale(QPainter*);
  QString _good_number(double v);

private slots:
  void _updateGeometry();
};

#endif // VXRULLERDECORATOR_H
