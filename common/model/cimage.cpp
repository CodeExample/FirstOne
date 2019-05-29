#include "cimage.h"

#include "cattributes.h"

#include <QBuffer>

using namespace sm;

class ImageImpl : public GraphicsImpl
{
public:
  ImageImpl(const QImage &image, int dpi) : GraphicsImpl(),
    _image(image), _dpi(dpi)
  {
  }

  void setImage(const QImage &image, int dpi)
  {
    _image = image;
    _dpi = dpi;
  }



  bool read(const QDomElement &dom_elm)
  {
    if (!GraphicsImpl::read(dom_elm))
      return false;

      double v_res = dom_elm.attribute("v_resolution", "0").toDouble();
      if (v_res > 0)
        _dpi = 72. * v_res;

    QDomElement image_data_elm = dom_elm.firstChildElement(TAG_IMAGE_DATA);
    if (!image_data_elm.isNull())
    {
      QByteArray ba = image_data_elm.text().toLatin1();

      QByteArray decompressed = qUncompress(QByteArray::fromBase64(ba.data()));
      QBuffer buffer(&decompressed);
      buffer.open(QIODevice::ReadOnly);

      if (!_image.load(&buffer, "PNG"))
        return false;


    }

    return true;
  }

  QRectF boundingRect() const
  {
    QRectF r = _image.rect();
    return QRectF(QPointF(), 72.*r.size()/_dpi);
  }


  QImage _image;
  double _dpi=72;
};


CGraphics CImage::createInstance(const QImage &image, int dpi)
{
  return CGraphics(new ImageImpl(image, dpi));
}


QImage CImage::image() const
{
  IMPL_D(Image);
  return d->_image;
}


void CImage::setImage(const QImage &image, int dpi) const
{
  IMPL_D(Image);
  d->setImage(image, dpi);
}



int CImage::dpi() const
{
  IMPL_D(Image);
  return d->_dpi;
}

void CImage::setDpi(int dpi) const
{
  IMPL_D(Image);
  d->_dpi = dpi;
}

