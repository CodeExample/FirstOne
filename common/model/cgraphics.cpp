#include "cgraphics.h"

#include "cattributes.h"

using namespace sm;




bool CGraphics::isGraphics(const CObject &object)
{
  switch(object.type())
  {
  case Document:
  case Template:
  case Shape:
  case Layer:
  case Mask:
    return true;
  }
  return false;
}



void GraphicsImpl::setTransform(const QTransform &tx)
{
  if (_transform != tx)
  {
    _transform = tx;
    emit _notifier.transformChanged();
  }
}


void GraphicsImpl::write(QDomElement &elm, QDomDocument &doc, bool) const
{
  if (_name != "")
    elm.setAttribute(ATTR_NAME, _name);

  if (!_visible)
    elm.setAttribute(ATTR_VISIBLE, _visible);

  if (_locked)
    elm.setAttribute(ATTR_LOCKED, _locked);

  if (!_transform.isIdentity())
  {
    QDomElement tx_elm = doc.createElement(ATTR_TRANSFORM);
    elm.appendChild(tx_elm);
    elm.setAttribute("m11", _transform.m11());
    elm.setAttribute("m12", _transform.m12());
    elm.setAttribute("m21", _transform.m21());
    elm.setAttribute("m22", _transform.m22());
    elm.setAttribute("m13", _transform.m13());
    elm.setAttribute("m31", _transform.m31());
    elm.setAttribute("m32", _transform.m32());
    elm.setAttribute("m23", _transform.m23());
    elm.setAttribute("m33", _transform.m33());
  }


}

bool GraphicsImpl::read(const QDomElement &elm)
{
  _name = elm.attribute(ATTR_CAPTION, "");
  _visible = elm.attribute(ATTR_VISIBLE, "1").toInt();
  _locked = elm.attribute(ATTR_LOCKED, "0").toInt();

  QTransform tx;

  QDomElement tx_elm = elm.firstChildElement(TAG_TRANSFORM);
  if (!tx_elm.isNull())
  {
    double m11 = tx_elm.attribute("m11", "1").toDouble();
    double m12 = tx_elm.attribute("m12", "0").toDouble();
    double m21 = tx_elm.attribute("m21", "0").toDouble();
    double m22 = tx_elm.attribute("m22", "1").toDouble();
    double m13 = tx_elm.attribute("m13", "0").toDouble();
    double m31 = tx_elm.attribute("m31", "0").toDouble();
    double m32 = tx_elm.attribute("m32", "0").toDouble();
    double m23 = tx_elm.attribute("m23", "0").toDouble();
    double m33 = tx_elm.attribute("m33", "1").toDouble();
    tx.setMatrix(m11, m12,m13,m21,m22,m23,m31,m32,m33);
  }

  if (tx != _transform)
    setTransform(tx);

  return true;
}
