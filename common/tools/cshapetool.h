#ifndef CSHAPETOOL_H
#define CSHAPETOOL_H


#include "cframetool.h"
#include "cgraphics.h"

using namespace sm;

class CShapeTool : public CFrameTool
{
public:
  enum ShapeType
  {
    Rectangle,
    Ellipse
  };

  CShapeTool(ShapeType, QWidget * =nullptr);

  int toolType() const override
  {
    switch(_shape_type)
    {
    case Rectangle:
      return ToolRectangle;
    case Ellipse:
      return ToolEllipse;
    }
    return 0;
  }


  bool frameAction(const QRectF &scene_rect, QGraphicsSceneMouseEvent *) override;

  bool clickAction(QGraphicsSceneMouseEvent *) override;

  void toolActivated() override;

  void updateRectItem(const QRectF&) override;

  QGraphicsItem * createFrameItem(const QRectF &) override;

  void deleteFrameItem() override;

private:
  ShapeType _shape_type;

  CGraphics createTmpl(const QRectF&);
};





#endif // CSHAPETOOL_H
