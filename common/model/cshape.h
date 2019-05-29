#ifndef CSTROKESELEMENT_H
#define CSTROKESELEMENT_H

#include "cstrokestemplate.h"

namespace sm
{

class CShape : public CGraphics
{
public:
  static CGraphics createInstance(const QString &name="");

  CShape() : CGraphics() {}
  CShape(const CObject &object) : CGraphics(object) {}
  CShape(const CMask&);

  QList<CGraphics> templates() const;

  void setMask(const CMask&) const;
  CMask mask() const;
};

}

#endif // CSTROKESELEMENT_H
