#ifndef CCURVE_H
#define CCURVE_H

#include "cgraphics.h"

#include <QPainterPath>

namespace sm
{

class CCurve : public CGraphics
{
public:
  static CGraphics createInstance(const QPainterPath& =QPainterPath());

  CCurve() : CGraphics() {}

  CCurve(const CGraphics& object) : CGraphics(object) {}

  void setPath(const QPainterPath&) const;
  QPainterPath path() const;

};

}
#endif // CCURVE_H
