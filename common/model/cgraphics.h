#ifndef CGRAPHICS_H
#define CGRAPHICS_H

#include "cmodel.h"

#include <QTransform>
#include <QGraphicsItem>
#include <QDebug>

namespace sm
{
class IVisitor;
class CGraphics;

class CGraphicsNotifier : public QObject
{
  Q_OBJECT

signals:
  void selectedChanged(bool);
  void transformChanged();
  void maskChanged();
  void hoverEntered();
  void hoverLeaved();
  void dataChanged(const CGraphics&);

};


class GraphicsImpl : public ObjectImpl
{
public:
  GraphicsImpl(const QString &name="") : _name(name) {}

  int type() const override
  {
    return -1;
  }

  QString name() const
  {
    return _name;
  }

  void setName(const QString &name)
  {
    _name = name;
  }

  virtual QRectF boundingRect() const
  {
    return QRectF();
  }

  virtual bool iterate(IVisitor*)
  {
    return false;
  }

  virtual QGraphicsItem * createItem()
  {
    return 0;
  }

  void setTransform(const QTransform &tx);

  void write(QDomElement&, QDomDocument&, bool recursive=true) const override;

  bool read(const QDomElement&) override;


  void setHovered(bool b)
  {
    if (b == _hovered)
      return;

    _hovered = b;

    if (b)
      emit _notifier.hoverEntered();
    else
      emit _notifier.hoverLeaved();
  }


  void setSelected(bool b)
  {
    if (_selected == b)
      return;

    _selected = b;

    emit _notifier.selectedChanged(b);
  }

  virtual void add(const CGraphics &) {}

  virtual void remove(const CGraphics &){}


  virtual QList<CGraphics> objects()
  {
    return QList<CGraphics>();
  }

  CObject _owner;
  QString _name;
  QTransform _transform;

  bool _visible = true;
  bool _locked = false;
  bool _hovered = false;
  bool _selected = false;

  CGraphicsNotifier _notifier;
};



class CGraphics : public CObject
{
public:
  static bool isGraphics(const CObject&);

  CGraphics() : CObject(nullptr) {}
  CGraphics(GraphicsImpl * data) : CObject(data) {}
  CGraphics(const CObject &object) : CObject(object) {}

  CGraphicsNotifier * notifier() const
  {
    IMPL_D(Graphics);
    return &d->_notifier;
  }


  inline bool iterate(IVisitor * v) const
  {
    IMPL_D_RET_FALSE_IF_NULL(Graphics);
    return d->iterate(v);
  }

  void setOwner(const CGraphics &owner) const
  {
    IMPL_D(Graphics);
    d->_owner = owner;
  }

  CGraphics owner() const
  {
    IMPL_D(Graphics);
    return d->_owner;
  }

  inline QGraphicsItem * createItem() const
  {
    IMPL_D(Graphics);
    return d->createItem();
  }

  inline QRectF boundingRect() const
  {
    IMPL_D(Graphics);
    return d ? d->boundingRect() : QRectF();
  }

  inline QString name() const
  {
    IMPL_D(Graphics);
    return d ? d->_name : "";
  }

  inline void setName(const QString &s) const
  {
    IMPL_D(Graphics);
    d->_name = s;
  }

  inline QTransform transform() const
  {
    IMPL_D(Graphics);
    return d->_transform;
  }

  inline void setTransform(const QTransform &tx) const
  {
    IMPL_D(Graphics);
    d->setTransform(tx);
  }

  inline bool isVisible() const
  {
    IMPL_D(Graphics);
    return d->_visible;
  }

  inline void setVisible(bool b) const
  {
    IMPL_D(Graphics);
    d->_visible = b;
  }

  inline bool isLocked() const
  {
    IMPL_D(Graphics);
    return d->_locked;
  }

  inline void setLocked(bool b) const
  {
    IMPL_D(Graphics);
    d->_locked = b;
  }

  inline void setHovered(bool b) const
  {
    IMPL_D(Graphics);
    d->setHovered(b);
  }

  inline bool hovered() const
  {
    IMPL_D(Graphics);
    return d->_hovered;
  }

  inline void setSelected(bool b) const
  {
    IMPL_D(Graphics);
    d->setSelected(b);
  }

  inline bool isSelected() const
  {
    IMPL_D(Graphics);
    return d->_selected;
  }

  inline void notifyDataChanged() const
  {
    IMPL_D(Graphics);
    emit d->_notifier.dataChanged(*this);
  }

  inline void add(const CGraphics &object) const
  {
    IMPL_D(Graphics);
    d->add(object);
  }


  inline void remove(const CGraphics &object) const
  {
    IMPL_D(Graphics);
    d->remove(object);
  }


  inline QList<CGraphics> objects() const
  {
    IMPL_D(Graphics);
    return d->objects();
  }

};


class IVisitor
{
public:
  virtual bool visitTo(const CGraphics&) =0;
};

}


#endif // CGRAPHICS_H
