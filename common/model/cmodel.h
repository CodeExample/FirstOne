#ifndef CMODEL_H
#define CMODEL_H

#include <QSharedData>
#include <QDomElement>
#include <QDomDocument>

#define IMPL_D(Class) Class##Impl * const d = static_cast<Class##Impl*>(_d.data())
#define IMPL_D_RET_FALSE_IF_NULL(Class) Class##Impl * const d = static_cast<Class##Impl*>(_d.data()); if(!d) return false;
#define IMPL_D_RET_IF_NULL(Class) Class##Impl * const d = static_cast<Class##Impl*>(_d.data()); if(!d) return;

#define cnull CObject()

#define DATA_KEY_OBJ_ID 1

#define MM_PER_INCH         25.4
#define PIX_PER_MM          2.83
#define PIX_PER_INCH        (PIX_PER_MM*MM_PER_INCH)

namespace sm
{

enum Types
{
  Document = 1,
  Template = 2,
  Shape    = 3,
  Group    = 4,
  Layer    = 5,
  Mask     = 6,
  Curve    = 7,
};


typedef int ObjId;

class IVisitor;


class ObjectImpl :
    public QSharedData
{
public:
  ObjectImpl() {}

  virtual ~ObjectImpl() {}

  virtual int type() const
  {
    return -1;
  }

  int getId()
  {
    if (_id < 0)
      _id = ++_global_id;

    return _id;
  }

  virtual void write(QDomElement&, QDomDocument&, bool =true) const;

  virtual bool read(const QDomElement&);

  ObjId _id=-1;
  static int _global_id;
};


class CObject
{
public:
  CObject(ObjectImpl * d =nullptr)
  {
    _d = d;
  }

  CObject(const CObject& other) :
    _d(other._d)
  {
  }


  inline int type() const
  {
    return _d->type();
  }


  inline ObjId id() const
  {
    if (!_d)
      return -1;

    return _d->getId();
  }


  inline CObject& operator=(const CObject& object)
  {
    if (_d != object._d)
      _d = object._d;
    return *this;
  }

  inline CObject operator=(const ObjectImpl*const_data) const
  {
    ObjectImpl * data = const_cast<ObjectImpl*>(const_data);
    if (_d != data)
      _d = data;
    return *this;
  }

  inline bool operator==(const CObject& p) const
  {
    return _d == p._d;
  }


  inline bool operator!=(const CObject& p) const
  {
    return _d != p._d;
  }


  inline bool operator==(const ObjectImpl * d) const
  {
    return _d == d;
  }


  inline bool operator!=(const ObjectImpl * d) const
  {
    return _d != d;
  }


  inline operator bool() const
  {
    return _d;
  }

  inline bool operator !() const
  {
    return !_d;
  }


  inline void write(QDomElement &elm, QDomDocument &doc, bool recursive=true) const
  {
    _d->write(elm, doc, recursive);
  }

  inline bool read(const QDomElement &elm) const
  {
    return _d->read(elm);
  }


protected:
  mutable QExplicitlySharedDataPointer<ObjectImpl> _d;

};




class CMutex : public CObject
{
public:
  CMutex();
  CMutex(const CObject &object) : CObject(object) {}

  QMutex * mutex() const;
};



class CModel
{

public:
  static QString typeToStr(int);
  static CObject readObject(const QDomElement&);

};

}
#endif // CMODEL_H
