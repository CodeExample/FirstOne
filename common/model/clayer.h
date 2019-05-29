#ifndef CLAYER_H
#define CLAYER_H

#include "cimage.h"

namespace  sm {

class CLayer : public CGraphics
{
public:
  static CGraphics createInstance(const QString& ="");

  CLayer();
  CLayer(const CGraphics&);

  void setActive(bool) const;
  bool isActive() const;

  bool readObjects(const QDomElement&) const;

  CImage sourceImage() const;
};

}

#endif // CLAYER_H
