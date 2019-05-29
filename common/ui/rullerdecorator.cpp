#include "rullerdecorator.h"


#include <QEvent>
#include <QPainter>

#include <math.h>

using namespace sm;

vxRulerDecorator::vxRulerDecorator(QWidget *parent, RulerInfo *_ruler_info, Orientation orientation) :
  QWidget(parent), _ruler_info(_ruler_info), _orientation(orientation), _inverse_value(0), _cursor_pos(-999999)
{
  _view_port = _ruler_info->viewport();

  _dimension_v = 1;
  _dimension_text = "px";
  _divider = 1;
  _value_mul = 1;
  _updateGeometry();

  _view_port->installEventFilter(this);
}


bool vxRulerDecorator::eventFilter(QObject *, QEvent * event)
{
  if (event->type() == QEvent::Paint)
  {
    _updateGeometry();
  }
  return false;
}


void vxRulerDecorator::_updateGeometry()
{
  switch(_orientation)
  {
  case Horizontal:
    setGeometry(0, 0, _view_port->width() + RULER_HEIGHT, RULER_HEIGHT+1);
    break;

  case Vertical:
    setGeometry(0, 0, RULER_HEIGHT+1,  _view_port->height() + RULER_HEIGHT);
    break;

  case Center:
    setGeometry(0, 0, RULER_HEIGHT+1, RULER_HEIGHT+1);
    break;
  }
  update();
}


void vxRulerDecorator::paintEvent(QPaintEvent * )
{
  QPainter painter(this);
  painter.setPen(_sideLinePen);
  painter.setBrush(_brushBody);

  painter.setFont(QFont("Tahoma", 8));

  _drawScale(&painter);
}


void vxRulerDecorator::setCurrentDimension(double dimension_v, const QString& dimension_text)
{
  _dimension_v = dimension_v;
  _dimension_text = dimension_text;
  _divider = 1;
}


void vxRulerDecorator::setCurrentDimension(double dimension_v, double divider, const QString& dimension_text, double value_mul)
{
  _dimension_v = dimension_v;
  _dimension_text = dimension_text;
  _divider = divider;
  _value_mul = value_mul;
}


QString vxRulerDecorator::_good_number(double v)
{
  QString text = QString::number(v, 'f', 6);

  int comma_ix = text.indexOf('.');
  if (comma_ix > 0)
  {
    int last_ix = text.length()-1;
    while(text.at(last_ix) == '0')
      text.remove(last_ix--, 1);
    if (text.at(last_ix) == '.')
      text.remove(last_ix--, 1);
  }
  return text;
}

void vxRulerDecorator::_drawScale(QPainter * painter)
{
  QPointF null_p;

  bool is_inverted = false;
  if (_inverse_value > 0)
  {
    switch (_orientation)
    {
    case Horizontal:
      null_p.setX(_inverse_value);
      break;
    case Vertical:
      null_p.setY(_inverse_value);
      break;
    default:;
    }
    is_inverted = true;
  }

  QRectF scene_r_px = _ruler_info->mapToScene(_view_port->rect());
  scene_r_px.translate(-null_p);

  QRectF scene_r_curr_dm = QRectF(scene_r_px.topLeft()/_dimension_v,
                                  scene_r_px.bottomRight()/_dimension_v);

  QTransform scene_to_vp_tx;
  QPolygonF one = scene_r_curr_dm; one.pop_back();
  QPolygonF two = QRectF(_view_port->rect()); two.pop_back();
  if (!QTransform::quadToQuad(one, two, scene_to_vp_tx))
    return;


  double doc_points_to_one_screen_px = 1./(_ruler_info->scale()*_dimension_v);

  int min_metric_item_len = 50;
  double doc_points_in_one_metric_item = min_metric_item_len*doc_points_to_one_screen_px;

  double mu_log = floor(log10(doc_points_in_one_metric_item*10));
  double mu = pow(10.0, mu_log);

  double x0 = floor(scene_r_curr_dm.left()/mu)*mu;
  double y0 = floor(scene_r_curr_dm.top()/mu)*mu;
  double x1 = ceil(scene_r_curr_dm.right()/mu)*mu;
  double y1 = ceil(scene_r_curr_dm.bottom()/mu)*mu;

  QPointF p0 = scene_to_vp_tx.map(QPointF(x0,y0));
  QPointF p1 = scene_to_vp_tx.map(QPointF(x1,y1));

  switch (_orientation)
  {

  case Horizontal:
  {
    QRectF r(0,0, parentWidget()->width() - 1, RULER_HEIGHT);
    painter->setPen(Qt::NoPen);
    painter->drawRect(r);
    painter->setPen(_sideLinePen);
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(r.bottomLeft()-QPointF(0,1), r.bottomRight()-QPointF(0,1));

    QFontMetrics m = painter->fontMetrics();
    int text_w = m.width(_dimension_text) + 2;
    int label_x = parentWidget()->width() - text_w - 2;

    int nx = (x1 - x0)/mu;
    QPointF p0 = scene_to_vp_tx.map(QPointF(x0,y0));
    QPointF p1 = scene_to_vp_tx.map(QPointF(x1,y1));
    int item_l = (p1.x() - p0.x())/nx;
    bool half_item=item_l > min_metric_item_len*2;
    if (half_item)
    {
      nx *= 2;
      mu /= 2.;
    }

    for(int m_ix=0; m_ix <= nx; ++m_ix)
    {
      int h = RULER_HEIGHT;
      double x = x0 + m_ix*mu;
      QPointF scene_m(x, y0);
      QPointF vp_m = scene_to_vp_tx.map(scene_m);

      int i0=1;

      if (vp_m.x() < label_x)
        painter->drawLine(vp_m.x(), 0, vp_m.x(), h);
      else
        i0 = 0;

      QString text = _good_number(is_inverted ? -x*_value_mul : x*_value_mul);

      int w = m.width(text) + 2;
      if (vp_m.x() + w < label_x)
        painter->drawText(QPoint(vp_m.x()+2, 10), text);

      double hd = mu/10.;
      for(int i=i0; i < 10; ++i)
      {
        double mx = x + i*hd;
        QPointF scene_m(mx, y0);
        QPointF vp_m = scene_to_vp_tx.map(scene_m);
        if (i == 5 && vp_m.x() < label_x)
          painter->drawLine(vp_m.x(), h, vp_m.x(), h/2.);
        else
          painter->drawLine(vp_m.x(), h, vp_m.x(), 3.*h/4.);
      }
    }

    double c_curr_dm = (_cursor_pos- null_p.x())/_dimension_v;
    {
      painter->save();
      QPointF scene_m(c_curr_dm, y0);
      QPointF cr_m = scene_to_vp_tx.map(scene_m);
      painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
      painter->setBrush(Qt::NoBrush);
      painter->setOpacity(0.6);
      painter->drawLine(cr_m.x(), RULER_HEIGHT-2, cr_m.x(), 0);
      painter->restore();
    }

    //painter->drawText(label_x, 10, _dimension_text);

    break;
  }

  case Vertical:
  {
    QRectF r(0,0, RULER_HEIGHT, parentWidget()->height() - 1);
    painter->setPen(Qt::NoPen);
    painter->drawRect(r);
    painter->setPen(_sideLinePen);
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(r.topRight() - QPointF(1,0), r.bottomRight() - QPointF(1,0));

    int ny = (y1 - y0)/mu;
    int item_l = (p1.y() - p0.y())/ny;
    bool half_item=item_l > min_metric_item_len*2;
    if (half_item)
    {
      ny *= 2;
      mu /= 2.;
    }

    for(int m_ix=0; m_ix <= ny; ++m_ix)
    {
      int w = RULER_HEIGHT;
      double y = y0 + m_ix*mu;
      QPointF scene_m(x0, y);
      QPointF vp_m = scene_to_vp_tx.map(scene_m);

      int i0=1;

      painter->drawLine(0, vp_m.y(), w, vp_m.y());

      QString text = _good_number(is_inverted ? -y*_value_mul : y*_value_mul);
      painter->save();
      painter->rotate(-90);
      painter->drawText(QPoint(-vp_m.y()+2, 10), text);
      painter->restore();

      double hd = mu/10.;
      for(int i=i0; i < 10; ++i)
      {
        double my = y + i*hd;
        QPointF scene_m(x0, my);
        QPointF vp_m = scene_to_vp_tx.map(scene_m);
        if (i == 5)
          painter->drawLine(w,vp_m.y(), w/2., vp_m.y());
        else
          painter->drawLine(w,vp_m.y(), 3.*w/4., vp_m.y());
      }
    }

    double c_curr_dm = (_cursor_pos- null_p.y())/_dimension_v;
    {
      QPointF scene_m(x0, c_curr_dm);
      QPointF cr_m = scene_to_vp_tx.map(scene_m);
      painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
      painter->setBrush(Qt::NoBrush);
      painter->setOpacity(0.6);
      painter->drawLine(RULER_HEIGHT-2, cr_m.y(), 0, cr_m.y());
    }

    break;
  }
  case Center:
  {
    QRectF r(0,0, RULER_HEIGHT, RULER_HEIGHT);
    painter->setPen(Qt::NoPen);
    painter->drawRect(r);
    painter->setPen(_sideLinePen);
    painter->drawLine(4, RULER_HEIGHT/2, RULER_HEIGHT-4, RULER_HEIGHT/2);
    painter->drawLine(RULER_HEIGHT/2, 4, RULER_HEIGHT/2, RULER_HEIGHT-4);
    painter->drawLine(r.bottomRight() - QPointF(1,1),  r.bottomRight() + QPointF(-1,1));
    painter->drawLine(r.bottomRight() - QPointF(1,1),  r.bottomRight() + QPointF(1,-1));
    break;
  }

  default:;
  }

}


void vxRulerDecorator::setInverseRelative(double v)
{
  _inverse_value = v;
  update();
}


void vxRulerDecorator::setCursorPos(double m)
{
  _cursor_pos = m;
  update();
}


void vxRulerDecorator::setStyle(const QBrush& body_b, const QBrush& corner_b, const QPen& side_p, const QPen& tick_p, const QPen& text_p)
{
  _brushBody = body_b;
  _brushCorner = corner_b;
  _sideLinePen = side_p;
  _tickPen = tick_p;
  _textPen = text_p;
}
