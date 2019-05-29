#include "cmodel.h"

#include <QString>
#include <QObject>
#include <QDebug>

#include "cattributes.h"
#include "cproject.h"
#include "cimage.h"
#include "cshape.h"

using namespace sm;

void ObjectImpl::write(QDomElement&, QDomDocument&, bool) const
{

}

bool ObjectImpl::read(const QDomElement&)
{
  return true;
}


QString CModel::typeToStr(int t)
{
  switch(t)
  {
  case Document:
    return QObject::tr("Document");
  case Template:
    return QObject::tr("Template");
  case Shape:
    return QObject::tr("Element");
  case Group:
    return QObject::tr("Group");
  case Layer:
    return QObject::tr("Layer");
  case Mask:
    return QObject::tr("Mask");
  default:;
  }
  return QString("undef-%1").arg(t);
}

int ObjectImpl::_global_id=0;


class MutexImpl : public ObjectImpl
{
public:
  QMutex _mutex;
};


CMutex::CMutex() : CObject(new MutexImpl())
{
}


QMutex * CMutex::mutex() const
{
  IMPL_D(Mutex);
  return &d->_mutex;
}


CObject CModel::readObject(const QDomElement &dom_element)
{
  QString tag = dom_element.tagName();
  CObject obj;
  if (tag == TAG_PROJECT)
  {
    obj = CProject::createInstance();
  } else
  if (tag == TAG_DOCUMENT)
  {
    obj = CDocument::createInstance();
  } else
  if (tag == TAG_BIG_PICT || tag == TAG_SOURCE_PICT)
  {
    obj = CImage::createInstance();
  } else
  if (tag == TAG_LR_SECTION)
  {
    obj = CLayer::createInstance();
  } else
  if (tag == TAG_FREE_MESH)
  {
    obj = CShape::createInstance();
  }

  if (obj && obj.read(dom_element))
    return obj;

  return cnull;
}


