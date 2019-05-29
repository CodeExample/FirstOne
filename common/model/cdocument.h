#ifndef CDOCUMENT_H
#define CDOCUMENT_H

#include "clayer.h"
#include "cenvironment.h"
#include "cimage.h"

namespace  sm {

class CDocument : public CGraphics
{
public:
  static CDocument createInstance();

  CDocument() {}
  CDocument(const CObject &object) : CGraphics(object) {}
  CDocument(const QSizeF&, int dpi =300, vxMetrics =vxmMillimeters);

  QSizeF size() const;

  void addLayer(const CLayer&) const;

  CLayer &root() const;

  CLayer activeLayer() const;
  void setActiveLayer(const CLayer&) const;

  int resolutionDPI() const;

  vxMetrics metric() const;

  QString fileName() const;
  void setFileName(const QString&) const;

  CImage sourceImage() const;
};

}
#endif // CDOCUMENT_H
