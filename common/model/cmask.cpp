#include "cmask.h"

#include <QPainter>

#define MASK_BORDER_W 10

#include "cattributes.h"
#include "cprojecthandler.h"

using namespace  sm;

class MaskImpl : public GraphicsImpl
{
public:
  MaskImpl() : GraphicsImpl() {}
  MaskImpl(const QPainterPath &path) : GraphicsImpl(),
    _path(path)
  {
  }


  int type() const override
  {
    return Mask;
  }

  QRectF boundingRect() const override
  {
    return _path.boundingRect().adjusted(-MASK_BORDER_W,-MASK_BORDER_W,MASK_BORDER_W,MASK_BORDER_W);
  }

  void updateImage()
  {
    if (_image.size() != hprj->sourceImage().size())
    {
      _image = QImage(hprj->sourceImage().size(), QImage::Format_ARGB32);
      QPainter painter(&_image);
      painter.drawImage(0,0, hprj->sourceImage());

      painter.setPen(Qt::NoPen);
      painter.setBrush(Qt::white);
      painter.drawEllipse(QRectF(0,0,100,100));
    }
  }

  void applyTransform()
  {
    _path = _transform.map(_path);
    _is_changed = true;
    setTransform(QTransform());
  }

  void setPath(const QPainterPath &path)
  {
    _path = path;
    _is_changed = true;
    emit _notifier.maskChanged();
  }

  void write(QDomElement &elm, QDomDocument &doc, bool recursive) const override
  {
    if (!_path.isEmpty())
    {
      QDomElement path_elm = doc.createElement(ATTR_PATH);
      elm.appendChild(path_elm);
      QString path_str;
      for(int i=0; i < _path.elementCount(); ++i)
      {
        QPainterPath::Element e = _path.elementAt(i);
        if (!path_str.isEmpty())
          path_str += " ";

        switch(e.type)
        {
        case QPainterPath::MoveToElement:
          path_str += QString("%1 %2 m").arg(e.x).arg(e.y);
          break;
        case QPainterPath::LineToElement:
          path_str += QString("%1 %2 l").arg(e.x).arg(e.y);
          break;
        case QPainterPath::CurveToElement:
          path_str += QString("%1 %2 ").arg(e.x).arg(e.y);
          e = _path.elementAt(++i);
          path_str += QString("%1 %2 ").arg(e.x).arg(e.y);
          e = _path.elementAt(++i);
          path_str += QString("%1 %2 c").arg(e.x).arg(e.y);
          break;
        case QPainterPath::CurveToDataElement:
          break;
        }
      }
      path_elm.setAttribute(ATTR_ELEMENTS, path_str);
    }
    GraphicsImpl::write(elm, doc, recursive);
  }


  bool read(const QDomElement &elm) override
  {
    QDomElement path_elm = elm.firstChildElement(ATTR_PATH);
    QPainterPath path;
    if (!path_elm.isNull())
    {
      QString path_str = path_elm.attribute(ATTR_ELEMENTS);
      QStringList path_elements = path_str.split(" ");
      QList<double> stack;
      bool ok;
      double x,y,x1,y1,x2,y2;
      while(path_elements.count() > 0)
      {
        const QString &token = path_elements.takeFirst();
        if (token == "m")
        {
          y = stack.takeLast();
          x = stack.takeLast();
          path.moveTo(x,y);
          continue;
        }
        if (token == "l")
        {
          y = stack.takeLast();
          x = stack.takeLast();
          path.lineTo(x,y);
          continue;
        }
        if (token == "c")
        {
          y2 = stack.takeLast();
          x2 = stack.takeLast();
          y1 = stack.takeLast();
          x1 = stack.takeLast();
          y = stack.takeLast();
          x = stack.takeLast();
          path.cubicTo(x,y, x1,y1, x2,y2);
          continue;
        }
        stack << token.toDouble(&ok);
        if (!ok)
          return false;
      }
    }
    _path = path;

    _is_changed = true;

    emit _notifier.maskChanged();

    return GraphicsImpl::read(elm);
  }

  QPainterPath _path;
  bool _inverted=false;
  QImage _image;
  bool _is_changed=false;
};


CGraphics CMask::createInstance()
{
  return CGraphics(new MaskImpl());
}

CMask::CMask(const QPainterPath &path) : CGraphics(new MaskImpl(path))
{

}


QPainterPath &CMask::path() const
{
  IMPL_D(Mask);
  return d->_path;
}


void CMask::setPath(const QPainterPath &path) const
{
  IMPL_D(Mask);
  d->setPath(path);
}


void CMask::setInverted(bool b) const
{
  IMPL_D(Mask);
  d->_inverted = b;
}


QImage CMask::image() const
{
  IMPL_D(Mask);
  return d->_image;
}

QImage * CMask::image_ptr()
{
  IMPL_D(Mask);
  return &d->_image;
}


void CMask::setImage(const QImage &img) const
{
  IMPL_D(Mask);
  d->_image = img;
}


void CMask::updateImage() const
{
  IMPL_D(Mask);
  d->updateImage();
}

void CMask::setChanged(bool c) const
{
  IMPL_D_RET_IF_NULL(Mask);
  d->_is_changed = c;
}

bool CMask::isChanged() const
{
  IMPL_D_RET_FALSE_IF_NULL(Mask);
  return d->_is_changed;
}

/*
void CMask::applyTransform() const
{
  IMPL_D(Mask);
  d->applyTransform();
}
*/
