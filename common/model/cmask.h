#ifndef CMASK_H
#define CMASK_H

#include "cgraphics.h"

#include <QPainterPath>

namespace sm
{

class CMask : public CGraphics
{
public:
  static CGraphics createInstance();

  CMask() : CGraphics() {}
  CMask(const CGraphics &object) : CGraphics(object) {}
  CMask(const QPainterPath&);

  QPainterPath &path() const;
  void setPath(const QPainterPath&) const;

  QImage image() const;
  QImage * image_ptr();
  void setImage(const QImage&) const;
  void updateImage() const;

  void setInverted(bool) const;

  void setChanged(bool) const;

  bool isChanged() const;

  //void applyTransform() const;

};

}
#endif // CMASK_H
