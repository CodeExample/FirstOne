#include "cgroup.h"

using namespace  sm;

class GroupImpl : public GraphicsImpl
{
public:
  GroupImpl(const QString &name) : GraphicsImpl(name)
  {
  }


  int type() const override
  {
    return Group;
  }


  void add(const CGraphics &object) override
  {
    _objects << object;
    object.setOwner(this);
  }

  void remove(const CGraphics &object) override
  {
    _objects.removeOne(object);
    object.setOwner(cnull);
  }


  bool iterate(IVisitor * v) override
  {
    if (v->visitTo(this))
      return true;

    for(const CGraphics &object : _objects)
    {
      if (object.iterate(v))
        return true;
    }

    return false;
  }


  QList<CGraphics> objects() override
  {
    return _objects;
  }

  QList<CGraphics> _objects;
};


CGraphics CGroup::createInstance(const QString &name)
{
  return CGraphics(new GroupImpl(name));
}


