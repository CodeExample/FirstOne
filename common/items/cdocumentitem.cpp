#include "cdocumentitem.h"

#include "cprojecthandler.h"

#include <QPainter>

CDocumentItem::CDocumentItem(const CDocument &doc) : _document(doc)
{
  connect(hprj, SIGNAL(showSourceImage(bool)), this, SLOT(_on_show_source_image(bool)));
  connect(hprj, SIGNAL(layerActivated(CLayer)), this, SLOT(_layer_activated(CLayer)));
}



void CDocumentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->setPen(Qt::NoPen);
  painter->setBrush(Qt::white);
  painter->drawRect(_document.boundingRect());

  painter->setPen(Qt::red);
  painter->setBrush(Qt::NoBrush);

  if (_show_source_image)
  {
    CImage src_img;
    if (_document.activeLayer() && _document.activeLayer().sourceImage())
    {
      src_img = _document.activeLayer().sourceImage();
    } else
    if (_document.sourceImage())
    {
      src_img = _document.sourceImage();
    }
    if (src_img)
    {
      QTransform img_tx = src_img.transform();
      painter->drawImage(img_tx.mapRect(src_img.boundingRect()), src_img.image());
    }
  }
}



QRectF CDocumentItem::boundingRect() const
{
  return _document.boundingRect();
}


void CDocumentItem::_on_show_source_image(bool b)
{
  if (_show_source_image != b)
  {
    _show_source_image = b;
    update();
  }
}


void CDocumentItem::_layer_activated(CLayer)
{
  update();
}
