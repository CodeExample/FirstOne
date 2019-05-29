#include "cshape.h"

#include "cattributes.h"

using namespace  sm;

class ShapeImpl : public GraphicsImpl
{
public:
  ShapeImpl(const QString &name) : GraphicsImpl(name)
  {
    _mask = CMask::createInstance();
  }

  ShapeImpl(const CMask &m) : _mask(m)
  {
  }

  int type() const override
  {
    return Shape;
  }


  void add(const CGraphics &tmpl) override
  {
    _templates << tmpl;
    tmpl.setOwner(this);
  }


  bool iterate(IVisitor * v) override
  {
    if (v->visitTo(this))
      return true;

    for(const CGraphics &tmpl : _templates)
    {
      if (tmpl.iterate(v))
        return true;
    }

    return false;
  }

  bool read(const QDomElement &dom_element) override
  {
    if (!GraphicsImpl::read(dom_element))
      return false;

    QDomElement objects_elm = dom_element.firstChildElement(TAG_OBJECTS);
    if (!readTemplates(objects_elm))
      return false;

    return true;
  }

  bool readTemplates(const QDomElement &dom_element)
  {
    for(auto object_elm = dom_element.firstChildElement(); !object_elm.isNull();
        object_elm = object_elm.nextSiblingElement())
    {
      CGraphics tmpl = CModel::readObject(object_elm);
      if (tmpl)
      {
        add(tmpl);
      }
    }

    return true;
  }


  void setMask(const CMask &m)
  {
    _mask = m;
    emit _notifier.maskChanged();
  }

  QList<CGraphics> _templates;
  CMask _mask;
};


CGraphics CShape::createInstance(const QString &name)
{
  return CGraphics(new ShapeImpl(name));
}


CShape::CShape(const CMask &mask) : CGraphics(new ShapeImpl(mask))
{
}


QList<CGraphics> CShape::templates() const
{
  IMPL_D(Shape);
  return d->_templates;
}


void CShape::setMask(const CMask &m) const
{
  IMPL_D(Shape);
  d->setMask(m);
}


CMask CShape::mask() const
{
  IMPL_D(Shape);
  return d->_mask;
}
