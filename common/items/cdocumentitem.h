#ifndef CDOCUMENTITEM_H
#define CDOCUMENTITEM_H

#include <QGraphicsObject>

#include "citems.h"
#include "cdocument.h"

using namespace sm;

class CDocumentItem : public QGraphicsObject
{
  Q_OBJECT

public:
  CDocumentItem(const CDocument&);

  int type() const override
  {
    return CItemsTypes::EngravingItem;
  }

  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

  QRectF boundingRect() const override;

private:
  CDocument _document;
  bool _show_source_image=true;

private slots:
  void _on_show_source_image(bool b);
  void _layer_activated(CLayer);
};

#endif // CDOCUMENTITEM_H
