#ifndef COGLSTROKESPAINTER_H
#define COGLSTROKESPAINTER_H


#include <coglpainterdata.h>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QOffscreenSurface>
#include <QPainter>

class COGLStrokesPainter
{
public:
  enum Strip
  {
    TriangleStrip = 0,
    LinearStrip   = 1
  };

  void paint(QPainter*, COGLPainterData*, Strip strip);
};

#endif // COGLSTROKESPAINTER_H
