#include "clayer.h"

#include "cmodel.h"
#include "cattributes.h"
#include "cimage.h"

using namespace sm;

class LayerImpl : public GraphicsImpl
{
public:

  LayerImpl(const QString &name) : GraphicsImpl(name)
  {
  }

  int type() const override
  {
    return Layer;
  }

  void add(const CGraphics &object) override
  {
    _elements << object;
    object.setOwner(this);
  }

  void remove(const CGraphics &object) override
  {
    _elements.removeOne(object);
    object.setOwner(cnull);
  }


  bool iterate(IVisitor * v) override
  {
    if (v->visitTo(this))
      return true;

    for(const CGraphics &object : _elements)
    {
      if (object.iterate(v))
        return true;
    }

    return false;
  }

  void setActive(bool b)
  {
    _active = b;
  }

  QList<CGraphics> objects() override
  {
    return _elements;
  }

  bool read(const QDomElement &dom_element) override
  {
    GraphicsImpl::read(dom_element);

    QDomElement objects_elm = dom_element.firstChildElement(TAG_OBJECTS);
    if (!objects_elm.isNull())
    {
      if (!readObjects(objects_elm))
        return false;
    }

    QDomElement image_elm = dom_element.firstChildElement(TAG_SOURCE_PICT);
    if (!image_elm.isNull())
    {
      _source_image = CModel::readObject(image_elm);
    }

    return true;
  }


  bool readObjects(const QDomElement &dom_element)
  {
    for(auto object_elm = dom_element.firstChildElement(); !object_elm.isNull();
        object_elm = object_elm.nextSiblingElement())
    {
      CGraphics object = CModel::readObject(object_elm);
      if (object)
      {
        add(object);
      }
    }

    return true;
  }


  bool _active=false;

  QList<CGraphics> _elements;
  CImage _source_image;
};


CGraphics CLayer::createInstance(const QString &name)
{
  return CGraphics(new LayerImpl(name));
}


CLayer::CLayer() : CGraphics()
{
}

CLayer::CLayer(const CGraphics &object) : CGraphics(object)
{
}



void CLayer::setActive(bool a) const
{
  IMPL_D(Layer);
  d->setActive(a);
}


bool CLayer::isActive() const
{
  IMPL_D_RET_FALSE_IF_NULL(Layer);
  return d->_active;
}


bool CLayer::readObjects(const QDomElement &dom_element) const
{
  IMPL_D_RET_FALSE_IF_NULL(Layer);
  return d->readObjects(dom_element);
}

CImage CLayer::sourceImage() const
{
  IMPL_D(Layer);
  return d->_source_image;
}
