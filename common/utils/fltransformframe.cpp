#include "fltransformframe.h"


QPointF TransformFrame::getControlPos(TransformFrameControl control) const
{
  switch(control)
  {
  case tfTopLeft:
    return tl;

  case tfTopRight:
    return tr;

  case tfBottomLeft:
    return bl;

  case tfBottomRight:
    return br;

  case tfTopHCenter:
    return (tl + tr)/2;

  case tfBottomHCenter:
    return (bl + br)/2;

  case tfVCenterLeft:
    return (tl + bl)/2;

  case tfVCenterRight:
    return (tr + br)/2;

  case tfCenter:
    return cr;

  case tfTop:
    return tr*_vector_t + tl*(1 - _vector_t);

  case tfBottom:
    return br*_vector_t + bl*(1 - _vector_t);

  case tfLeft:
    return bl*_vector_t + tl*(1 - _vector_t);

  case tfRight:
    return br*_vector_t + tr*(1 - _vector_t);

  default:;
  }

  return (tl + br)/2;
}


void TransformFrame::horizontalReflection()
{
  qSwap<QPointF>(tl,tr);
  qSwap<QPointF>(bl,br);
}


void TransformFrame::verticalReflection()
{
  qSwap<QPointF>(tl,bl);
  qSwap<QPointF>(tr,br);
}


void TransformFrame::flipHorizontal()
{
  bl.setX(2*cr.x() - bl.x());
  tl.setX(2*cr.x() - tl.x());
  br.setX(2*cr.x() - br.x());
  tr.setX(2*cr.x() - tr.x());
}


void TransformFrame::flipVertical()
{
  bl.setY(2*cr.y() - bl.y());
  tl.setY(2*cr.y() - tl.y());
  br.setY(2*cr.y() - br.y());
  tr.setY(2*cr.y() - tr.y());
}




void TransformFrame::rotate(double angle)
{
  QTransform tx;
  tx.translate(_cr.x(), _cr.y());
  tx.rotateRadians(angle);
  tx.translate(-_cr.x(), -_cr.y());

  tl = tx.map(_tl);
  tr = tx.map(_tr);
  bl = tx.map(_bl);
  br = tx.map(_br);
}


void TransformFrame::_position_controls_to(const QRectF& r, const QTransform& shape_to_canvas)
{
  if (fabs(r.width()) < 1 || fabs(r.height()) < 1)
    return;

  tl = shape_to_canvas.map(r.topLeft());
  tr = shape_to_canvas.map(r.topRight());
  bl = shape_to_canvas.map(r.bottomLeft());
  br = shape_to_canvas.map(r.bottomRight());

  double dx = (_initial_cr.x() - _initial_rect.left())/_initial_rect.width();
  double dy = (_initial_cr.y() - _initial_rect.top())/_initial_rect.height();

  QPointF p(r.left() + r.width()*dx, r.top() + r.height()*dy);

  cr = shape_to_canvas.map(p);
}



bool TransformFrame::getTransform(QTransform& tx) const
{
  return QTransform::quadToQuad(_polygon, polygon(), tx);
}


void TransformFrame::prepareTransformations(double scale)
{
  _scale = scale;

  _tl = tl;
  _tr = tr;
  _bl = bl;
  _br = br;
  _cr = cr;

  _polygon = polygon();

  _initial_rect= QRectF(-1000,-1000,2000,2000);

  QPolygonF shape_rect_plgn = _initial_rect;
  shape_rect_plgn.pop_back();

  QPolygonF frame_plgn;
  frame_plgn << polygon();

  if (!QTransform::quadToQuad(frame_plgn, shape_rect_plgn, _canvas_to_shape_tx))
    return;

  _initial_cr = _canvas_to_shape_tx.map(_cr);

  _shape_to_canvas_tx = _canvas_to_shape_tx.inverted();

  _vector_t = 0.5;
}


void TransformFrame::prepareTransformations(TransformFrameControl control, const QPointF& start_pos, double scale)
{
  prepareTransformations(scale);

  _control = control;

  double s = 10/_scale;

  switch(control)
  {
  case tfTopLeft:
    _p0 = _initial_rect.bottomRight();
    _p1 = _initial_rect.topLeft();
    break;

  case tfTopRight:
    _p0 = _initial_rect.bottomLeft();
    _p1 = _initial_rect.topRight();
    break;

  case tfBottomLeft:
    _p0 = _initial_rect.topRight();
    _p1 = _initial_rect.bottomLeft();
    break;

  case tfBottomRight:
    _p0 = _initial_rect.topLeft();
    _p1 = _initial_rect.bottomRight();
    break;

  case tfTop:
    if (crossPointToLine(tl, tr, start_pos, s/2))
      _vector_t = getTNormal(tl, tr, start_pos);
  case tfTopHCenter:
    _p0 = QPointF(0, 1000);
    _p1 = QPointF(0,-1000);
    break;

  case tfBottom:
    if (crossPointToLine(bl, br, start_pos, s/2))
      _vector_t = getTNormal(bl, br, start_pos);
  case tfBottomHCenter:
    _p0 = QPointF(0,-1000);
    _p1 = QPointF(0, 1000);
    break;

  case tfLeft:
    if (crossPointToLine(tl, bl, start_pos, s/2))
      _vector_t = getTNormal(tl, bl, start_pos);
  case tfVCenterLeft:
    _p0 = QPointF( 1000, 0);
    _p1 = QPointF(-1000, 0);
    break;

  case tfRight:
    if (crossPointToLine(tr, br, start_pos, s/2))
      _vector_t = getTNormal(tr, br, start_pos);
  case tfVCenterRight:
    _p0 = QPointF(-1000, 0);
    _p1 = QPointF( 1000, 0);
    break;

  default:;
  }

}



void TransformFrame::setControlPos(const QPointF& position, bool reflection, bool proportional, bool fixed)
{
  setControlPos(_control, position, reflection, proportional, fixed);
}


void TransformFrame::setControlPos(uint control, const QPointF& position, bool reflection, bool proportional, bool fixed)
{
  QRectF r = _initial_rect;
  QPointF c = _canvas_to_shape_tx.map(_cr);

  QPointF p = _canvas_to_shape_tx.map(position);

  double dx = _p1.x() - _p0.x();
  double dy = _p1.y() - _p0.y();

  if (dx == 0 && dy == 0)
    return;


  if (fixed)
  {
    double ddx = 10*dx/100.0;
    double ddy = 10*dy/100.0;

    double px = ddx*round((p.x() - _p0.x())/ddx) + _p0.x();
    double py = ddy*round((p.y() - _p0.y())/ddy) + _p0.y();

    p = QPointF(px, py);
  }


  if (proportional)
  {
    double t = 1;
    if (dy == 0)
      t = (p.x() - _p0.x())/dx;
    else
    if (dx == 0)
      t = (p.y() - _p0.y())/dy;
    else
    {
      double tx = (p.x() - _p0.x())/dx;
      double ty = (p.y() - _p0.y())/dy;
      t = tx/2 + ty/2;
    }
    p = _p0 + (_p1 - _p0)*t;
  }


  switch(control)
  {
  case tfTopLeft:
  {
    QPointF tl = r.topLeft();

    if (reflection)
    {
      QPointF br = r.bottomRight();

      double dx = (tl.x() - c.x());
      double dy = (tl.y() - c.y());

      if (dx != 0)
      {
        dx = (p.x() - c.x())/dx;
        br.setX((br.x() - c.x())*dx + c.x());
      }

      if (dy != 0)
      {
        dy = (p.y() - c.y())/dy;
        br.setY((br.y() - c.y())*dy + c.y());
      }

      r.setBottomRight(br);
    }

    r.setTopLeft(p);

    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }
  case tfTopRight:
  {
    QPointF tr = r.topRight();

    if (reflection)
    {
      QPointF bl = r.bottomLeft();

      double dx = (tr.x() - c.x());
      if (dx != 0)
      {
        dx = (p.x() - c.x())/dx;
        bl.setX((bl.x() - c.x())*dx + c.x());
      }

      double dy = (tr.y() - c.y());
      if (dy != 0)
      {
        dy = (p.y() - c.y())/dy;
        bl.setY((bl.y() - c.y())*dy + c.y());
      }

      r.setBottomLeft(bl);
    }

    r.setTopRight(p);

    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }

  case tfBottomLeft:
  {
    QPointF bl = r.bottomLeft();

    if (reflection)
    {
      QPointF tr = r.topRight();

      double dx = (bl.x() - c.x());
      if (dx != 0)
      {
        dx = (p.x() - c.x())/dx;
        tr.setX((tr.x() - c.x())*dx + c.x());
      }

      double dy = (bl.y() - c.y());
      if (dy != 0)
      {
        dy = (p.y() - c.y())/dy;
        tr.setY((tr.y() - c.y())*dy + c.y());
      }

      r.setTopRight(tr);
    }

    r.setBottomLeft(p);

    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }

  case tfBottomRight:
  {
    QPointF br = r.bottomRight();

    if (reflection)
    {
      QPointF tl = r.topLeft();

      double dx = (br.x() - c.x());
      if (dx != 0)
      {
        dx = (p.x() - c.x())/dx;
        tl.setX((tl.x() - c.x())*dx + c.x());
      }

      double dy = (br.y() - c.y());
      if (dy != 0)
      {
        dy = (p.y() - c.y())/dy;
        tl.setY((tl.y() - c.y())*dy + c.y());
      }

      r.setTopLeft(tl);
    }

    r.setBottomRight(p);

    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }

  case tfTop:
  case tfTopHCenter:
  {
    double t = r.top();
    if (reflection)
    {
      double b = r.bottom();
      double dy = (t - c.y());
      if (dy != 0)
      {
        dy = (p.y() - c.y())/dy;
        b = ((b - c.y())*dy + c.y());
      }
      r.setBottom(b);
    }
    r.setTop(p.y());
    if (proportional)
    {
      double d = -1000 - p.y();
      if (!reflection)
        d /= 2;

      if (d < -1000)
      {
        r.setRight(-1000 - d);
        r.setLeft(  1000 + d);
      } else {
        r.setLeft( -1000 - d);
        r.setRight( 1000 + d);
      }
    }
    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }

  case tfBottom:
  case tfBottomHCenter:
  {
    double b = r.bottom();
    if (reflection)
    {
      double t = r.top();
      double dy = (b - c.y());
      if (dy != 0)
      {
        dy = (p.y() - c.y())/dy;
        t = ((t - c.y())*dy + c.y());
      }
      r.setTop(t);
    }
    r.setBottom(p.y());
    if (proportional)
    {
      double d = 1000 - p.y();

      if (!reflection)
        d /= 2;

      if (d < 1000)
      {
        r.setRight(1000 - d);
        r.setLeft(-1000 + d);
      } else {
        r.setLeft(1000 - d);
        r.setRight(-1000 + d);
      }
    }
    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }

  case tfLeft:
  case tfVCenterLeft:
  {
    double l = r.left();
    if (reflection)
    {
      double b = r.right();
      double dx = (l - c.x());
      if (dx != 0)
      {
        dx = (p.x() - c.x())/dx;
        b = ((b - c.x())*dx + c.x());
      }
      r.setRight(b);
    }

    r.setLeft(p.x());

    if (proportional)
    {
      double d = -1000 - p.x();

      if (!reflection)
        d /= 2;

      if (d < -1000)
      {
        r.setBottom(-1000 - d);
        r.setTop( 1000 + d);
      } else {
        r.setTop(-1000 - d);
        r.setBottom(1000 + d);
      }
    }

    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }
  case tfRight:
  case tfVCenterRight:
  {
    double b = r.right();
    if (reflection)
    {
      double t = r.left();
      double dx = (b - c.x());
      if (dx != 0)
      {
        dx = (p.x() - c.x())/dx;
        t = ((t - c.x())*dx + c.x());
      }
      r.setLeft(t);
    }

    r.setRight(p.x());
    if (proportional)
    {
      double d = 1000 - p.x();

      if (!reflection)
        d /= 2;

      if (d < 1000)
      {
        r.setBottom(1000 - d);
        r.setTop(-1000 + d);
      } else {
        r.setTop(1000 - d);
        r.setBottom(-1000 + d);
      }
    }
    _position_controls_to(r, _shape_to_canvas_tx);
    break;
  }

  default:;
  }

}



void TransformFrame::setProjectionPos(const QPointF& position, bool reflection, bool proportional, bool fixed)
{
  setProjectionPos(_control, position, reflection, proportional, fixed);
}



bool _is_valid(const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& p)
{
  QPolygonF plgn;
  plgn << a << b << c;
  if (plgn.containsPoint(p, Qt::OddEvenFill))
    return false;

  return true;
}


QPointF _prop_d(const QPointF& p, bool check)
{
  if (!check)
    return p;

  if (fabs(p.x()) > fabs(p.y()))
    return QPointF(p.x(), 0);
  else
    return QPointF(0, p.y());
}


void TransformFrame::setProjectionPos(TransformFrameControl control, const QPointF& position, bool reflection, bool proportional, bool fixed)
{
  QRectF r = _initial_rect;

  QPointF p = _canvas_to_shape_tx.map(position);

  double dx = _p1.x() - _p0.x();
  double dy = _p1.y() - _p0.y();

  if (dx == 0 && dy == 0)
    return;

  if (fixed)
  {
    double ddx = 10*dx/100.0;
    double ddy = 10*dy/100.0;

    double px = ddx*round((p.x() - _p0.x())/ddx) + _p0.x();
    double py = ddy*round((p.y() - _p0.y())/ddy) + _p0.y();

    p = QPointF(px, py);
  }


  QPointF rtl = r.topLeft();
  QPointF rbl = r.bottomLeft();
  QPointF rtr = r.topRight();
  QPointF rbr = r.bottomRight();

  QPointF c, d;


  switch(control)
  {
  case tfTopLeft:
    d = _prop_d(p - rtl, proportional);
    rtl += d;

    if (reflection)
      rbr -= d;

    break;

  case tfTopRight:
    d = _prop_d(p - rtr, proportional);
    rtr += d;

    if (reflection)
      rbl -= d;

    break;

  case tfBottomLeft:
    d = _prop_d(p - rbl, proportional);
    rbl += d;

    if (reflection)
      rtr -= d;

    break;

  case tfBottomRight:
    d = _prop_d(p - rbr, proportional);
    rbr += d;

    if (reflection)
      rtl -= d;

    break;

  case tfTop:
  case tfTopHCenter:
    c = _canvas_to_shape_tx.map(_tr*_vector_t + _tl*(1 - _vector_t));
    d = p - c;
    if (proportional)
      d.setY(0);
    rtr += d;
    rtl += d;
    if (reflection)
    {
      rbr -= d;
      rbl -= d;
    }
    break;

  case tfBottom:
  case tfBottomHCenter:
    c = _canvas_to_shape_tx.map(_br*_vector_t + _bl*(1 - _vector_t));
    d = p - c;
    if (proportional)
      d.setY(0);
    rbr += d;
    rbl += d;
    if (reflection)
    {
      rtr -= d;
      rtl -= d;
    }
    break;

  case tfLeft:
  case tfVCenterLeft:
    c = _canvas_to_shape_tx.map(_bl*_vector_t + _tl*(1 - _vector_t));
    d = p - c;
    if (proportional)
      d.setX(0);
    rtl += d;
    rbl += d;
    if (reflection)
    {
      rtr -= d;
      rbr -= d;
    }
    break;

  case tfRight:
  case tfVCenterRight:
    c = _canvas_to_shape_tx.map(_br*_vector_t + _tr*(1 - _vector_t));
    d = p - c;
    if (proportional)
      d.setX(0);
    rtr += d;
    rbr += d;
    if (reflection)
    {
      rtl -= d;
      rbl -= d;
    }
    break;

  default:;
  }

  QPointF ntl = _shape_to_canvas_tx.map(rtl);
  QPointF nbl = _shape_to_canvas_tx.map(rbl);
  QPointF ntr = _shape_to_canvas_tx.map(rtr);
  QPointF nbr = _shape_to_canvas_tx.map(rbr);

  if (QLineF(ntl, nbr).intersect(QLineF(ntr, nbl), &c) != QLineF::BoundedIntersection)
    return;

  d = ntl - nbr;
  double t=0;
  if (fabs(d.x()) > fabs(d.y()))
    t = (c.x() - nbr.x())/d.x();
  else
    t = (c.y() - nbr.y())/d.y();

  if (t < 0.1 || t > 0.9)
    return;

  d = ntr - nbl;
  t=0;
  if (fabs(d.x()) > fabs(d.y()))
    t = (c.x() - nbl.x())/d.x();
  else
    t = (c.y() - nbl.y())/d.y();

  if (t < 0.1 || t > 0.9)
    return;

  QPolygonF old_plgn;
  old_plgn << polygon();

  tl = ntl;
  bl = nbl;
  tr = ntr;
  br = nbr;

  QPolygonF new_plgn;
  new_plgn << polygon();

  QTransform tx;

  if (!QTransform::quadToQuad(old_plgn, new_plgn, tx))
    return;

  cr = tx.map(cr);

}


bool TransformFrame::operator==(const TransformFrame& tf) const
{
  return tf.tl == tl && tf.tr == tr && tf.bl == bl && tf.br == br;
}

