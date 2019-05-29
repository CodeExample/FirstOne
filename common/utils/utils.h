#ifndef UTILS_H
#define UTILS_H

#include <QLineF>
#include "math.h"

namespace utils
{

inline double sqr(double x)
{
  return x*x;
}

inline QPointF bic_at(double t, QPointF p0, QPointF p1, QPointF p2)
{
  return sqr(1-t)*p0 + 2*t*(1-t)*p1 + sqr(t)*p2;
}

inline double distancePointf(double x0, double y0, double x1, double y1)
{
  return sqrt(pow(x1-x0, 2) + pow(y1-y0, 2));
}


inline double distancePointf(const QPointF& p)
{
  return sqrt(pow(p.x(),2) + pow(p.y(),2));
}


inline double distancePointf(const QPointF& p1, const QPointF& p0)
{
  return distancePointf(p1 - p0);
}

inline bool crossPointToLine(double x0, double y0, double x1, double y1, double px, double py, double mind)
{
  double dx = x1 - x0;
  double dy = y1 - y0;

  double ct = 0;
  if (fabs(dx) > fabs(dy)){
    ct = (px - x0)/dx;
  } else
    if (dy != 0){
      ct = (py - y0)/dy;
    } else {
      ct = 0;
    }

  if (ct < 0 || ct > 1)
    return false;

  double x = x0 + dx * ct - px;
  double y = y0 + dy * ct - py;
  double d = sqrt(x*x + y*y);

  return d < mind;
}

inline bool crossPointToLine(const QPointF p0, const QPointF& p1, const QPointF& p, double mind)
{
  return crossPointToLine(p0.x(), p0.y(), p1.x(), p1.y(), p.x(), p.y(), mind);
}



inline bool crossPointToLine(const QLineF &l, const QPointF& p, double mind)
{
  return crossPointToLine(l.p1(), l.p2(), p, mind);
}


inline double getTNormal(const QPointF& p1, const QPointF& p2, const QPointF& p)
{
  double d = sqr(p1.x()-p2.x()) + sqr(p1.y()-p2.y());
  if (qFuzzyIsNull(d)) return 0.0;
  return ((p.x()-p1.x())*(p2.x()-p1.x())+(p.y()-p1.y())*(p2.y()-p1.y())) / d;
}


inline double getTNormal(const QLineF& l, const QPointF& p)
{
  return getTNormal(l.p1(), l.p2(), p);
}


inline double distanceSqr(QPointF dp)
{
  return dp.x()*dp.x() + dp.y()*dp.y();
}


inline double distanceSqr(QPointF p0, QPointF p1)
{
  return distanceSqr(p1 - p0);
}



inline double distanceToLine(const QPointF& p, const QPointF& l0, const QPointF& l1)
{
  double d = sqr(l0.x()-l1.x()) + sqr(l0.y()-l1.y());
  double dn = ((p.x()-l0.x())*(l1.x()-l0.x())+(p.y()-l0.y())*(l1.y()-l0.y()));
  if (dn>0 && dn<d)
    return fabs((l0.x()-p.x())*(l1.y()-p.y())-(l1.x()-p.x())*(l0.y()-p.y())) / sqrt(d);

  double d0 = distanceSqr(p, l0);
  double d1 = distanceSqr(p, l1);
  return (d0 < d1)? sqrt(d0) : sqrt(d1);
}



}




#endif // UTILS_H
