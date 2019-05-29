#include "ccurve.h"


using namespace sm;

class CurveImpl : public GraphicsImpl
{
public:
  CurveImpl(const QPainterPath &path) : _path(path)
  {
  }

  int type() const override
  {
    return Curve;
  }



  QPainterPath _path;
};


CGraphics CCurve::createInstance(const QPainterPath &path)
{
  return CGraphics(new CurveImpl(path));
}


void CCurve::setPath(const QPainterPath &path) const
{
  IMPL_D(Curve);
  d->_path = path;
}


QPainterPath CCurve::path() const
{
  IMPL_D(Curve);
  return d->_path;
}

