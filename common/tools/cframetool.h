#ifndef VXFRAMETOOL_H
#define VXFRAMETOOL_H

#include "ctools.h"

#include <QAbstractGraphicsShapeItem>

using namespace sm;

class CFrameTool : public CAbstractTool
{
public:
  CFrameTool(QWidget * =nullptr, bool post_event_tool=true);
  ~CFrameTool() override {}

  void setGraphicsView(QGraphicsView*) override;

  void setPen(const QPen& pen)
  {
    _frame_pen = pen;
    _frame_pen.setCosmetic(true);
  }

  void setBrush(const QBrush& brush)
  {
    _frame_brush = brush;
  }

  void toolActivated() override;

  void toolDeactivated() override;

protected:
  QGraphicsItem * _rect_item=nullptr;
  QPointF _begin_pos;
  QPen _frame_pen;
  QBrush _frame_brush;

  bool _pressed;

  bool _move;

  bool eventFilter(QObject *, QEvent *) override;

  virtual bool frameAction(const QRectF&, QGraphicsSceneMouseEvent*) {return false;}

  virtual bool clickAction(QGraphicsSceneMouseEvent*) {return false;}

  virtual void sceneBeginDrag(QGraphicsSceneMouseEvent*) {}
  virtual void sceneDrag(QGraphicsSceneMouseEvent*) {}
  virtual void sceneEndDrag(QGraphicsSceneMouseEvent*) {}
  virtual void sceneMouseMove(QGraphicsSceneMouseEvent*) {}
  virtual bool keyPressed(QKeyEvent*) {return false;}
  virtual bool keyReleased(QKeyEvent*) {return false;}

  virtual void updateRectItem(const QRectF&);
  virtual QGraphicsItem * createFrameItem(const QRectF &);
  virtual void deleteFrameItem();

private:
  bool _post_event_tool;
  bool _post_event;

  void lockTool() override {}
  void unlockTool() override {}

};




#endif // VXFRAMETOOL_H
