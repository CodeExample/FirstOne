#include "cdocument.h"
#include "cattributes.h"
#include "cdocumentitem.h"
#include "cprojecthandler.h"
#include "cimage.h"

using namespace  sm;

class DocumentImpl : public GraphicsImpl
{
public:
  DocumentImpl(const QSizeF &size, int dpi, vxMetrics m) : _metric(m), _resolution_dpi(dpi)
  {
    _size.setWidth(metr->convertMetricToPx(m, 72, size.width()));
    _size.setHeight(metr->convertMetricToPx(m, 72, size.height()));
    _root = CLayer::createInstance();
  }

  QGraphicsItem * createItem() override
  {
    return new CDocumentItem(CGraphics(this));
  }

  QRectF boundingRect() const override
  {
    double w = _size.width();
    double h = _size.height();
    return QRectF(0,0, w,h);
  }

  void setActiveLayer(const CLayer &layer)
  {
    if (_active_layer == layer && layer.isActive())
      return;

    if (_active_layer)
    {
      _active_layer.setActive(false);
      emit hprj->layerDeactivated(_active_layer);
    }

    _active_layer = layer;
    if (_active_layer)
      _active_layer.setActive(true);

    emit hprj->layerActivated(_active_layer);
  }


  bool iterate(IVisitor * v) override
  {
    if (v->visitTo(this))
      return true;

    return _root.iterate(v);
  }

  CLayer activeLayer()
  {
    if (!_active_layer)
    {
      _active_layer = CLayer::createInstance();
      _active_layer.setActive(true);
      _root.add(_active_layer);
      emit hprj->objectAdded(_active_layer, _root);
      emit hprj->layerActivated(_active_layer);
    }
    return _active_layer;
  }

  bool read(const QDomElement &dom_elm) override
  {
    QDomElement images_elm = dom_elm.firstChildElement(TAG_IMAGES);
    if (!images_elm.isNull())
    {
      QDomElement objects_elm = images_elm.firstChildElement(TAG_OBJECTS);
      for(auto object_elm = objects_elm.firstChildElement(); !objects_elm.isNull();
          object_elm = objects_elm.nextSiblingElement())
      {
        _source_image = CModel::readObject(object_elm);
        if (_source_image)
          break;
      }
    }

    double w_mm = dom_elm.attribute(ATTR_WIDTH_MM).toDouble();
    double h_mm = dom_elm.attribute(ATTR_HEIGHT_MM).toDouble();

    _size = QSizeF(w_mm*PIX_PER_MM, h_mm*PIX_PER_MM);
    _resolution_dpi = dom_elm.attribute(ATTR_DPI, "72").toInt();

    if (_source_image)
      _source_image.setDpi(_resolution_dpi);

    return true;
  }

  vxMetrics _metric=vxmMillimeters;
  int _resolution_dpi=300;
  QSizeF _size;
  CLayer _root;
  CLayer _active_layer;
  QString _file_name;
  CImage _source_image;
};


CDocument CDocument::createInstance()
{
  return CDocument(new DocumentImpl(QSizeF(0,0), 0, vxmMillimeters));
}

CDocument::CDocument(const QSizeF &sz, int dpi, vxMetrics m) :
  CGraphics(new DocumentImpl(sz, dpi, m))
{
}



QSizeF CDocument::size() const
{
  IMPL_D(Document);
  return d->_size;
}


void CDocument::addLayer(const CLayer &l) const
{
  IMPL_D(Document);
  d->_root.add(l);
}


CLayer &CDocument::root() const
{
  IMPL_D(Document);
  return d->_root;
}


CLayer CDocument::activeLayer() const
{
  IMPL_D(Document);
  return d->activeLayer();
}

void CDocument::setActiveLayer(const CLayer &layer) const
{
  IMPL_D(Document);
  d->setActiveLayer(layer);
}


int CDocument::resolutionDPI() const
{
  IMPL_D(Document);
  return d->_resolution_dpi;
}

vxMetrics CDocument::metric() const
{
  IMPL_D(Document);
  return d->_metric;
}


QString CDocument::fileName() const
{
  IMPL_D(Document);
  return d->_file_name;
}


void CDocument::setFileName(const QString &fn) const
{
  IMPL_D(Document);
  d->_file_name = fn;
}


CImage CDocument::sourceImage() const
{
  IMPL_D(Document);
  return d->_source_image;
}
