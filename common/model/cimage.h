#ifndef CIMAGE_H
#define CIMAGE_H

#include "cgraphics.h"

namespace  sm
{


class CImage : public CGraphics
{
public:
  static CGraphics createInstance(const QImage &image=QImage(), int dpi=72);

  CImage() : CGraphics() {}
  CImage(const CObject &object) : CGraphics(object) {}

  QImage image() const;
  void setImage(const QImage&, int dpi) const;

  int dpi() const;
  void setDpi(int) const;
};


}

#endif // CIMAGE_H
