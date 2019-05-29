#ifndef FLTRANSFORMFRAME_H
#define FLTRANSFORMFRAME_H

#include <QtGui>

#include "utils.h"


enum TransformFrameControl
{
  tfNoControl       = 0,

  tfTop             = 0x0001,
  tfVCenter         = 0x0002,
  tfBottom          = 0x0004,
  tfLeft            = 0x0010,
  tfHCenter         = 0x0020,
  tfRight           = 0x0040,

  tfTopLeft         = tfTop | tfLeft,
  tfTopHCenter      = tfTop | tfHCenter,
  tfTopRight        = tfTop | tfRight,

  tfBottomLeft      = tfBottom | tfLeft,
  tfBottomHCenter   = tfBottom | tfHCenter,
  tfBottomRight     = tfBottom | tfRight,

  tfVCenterLeft     = tfVCenter | tfLeft,
  tfVCenterRight    = tfVCenter | tfRight,
  tfCenter          = tfVCenter | tfHCenter,

  tfBody            = 0x0080,
  tfRotate          = 0x0100

};

using namespace  utils;

class TransformFrame
{
public:
  TransformFrame() {}

  explicit TransformFrame(const QRectF& r) :
    tl(r.topLeft()), tr(r.topRight()),
    bl(r.bottomLeft()), br(r.bottomRight()),
    cr(r.center())
  {
  }

  TransformFrame(const TransformFrame& tf) :
    tl(tf.tl), tr(tf.tr),
    bl(tf.bl), br(tf.br),
    cr(tf.cr),
    _initial_rect(tf._initial_rect),
    _canvas_to_shape_tx(tf._canvas_to_shape_tx),
    _shape_to_canvas_tx(tf._shape_to_canvas_tx)
  {
  }

  QPointF tl, tr, bl, br, cr;
  inline QPointF mt() const {return (tl + tr)/2;}
  inline QPointF mb() const {return (bl + br)/2;}
  inline QPointF ml() const {return (tl + bl)/2;}
  inline QPointF mr() const {return (tr + br)/2;}


  inline QPolygonF polygon() const
  {
    QPolygonF plgn;
    plgn << tl << tr << br << bl;
    return plgn;
  }

  inline TransformFrame getMapped(const QTransform& t) const
  {
    TransformFrame tf;
    tf.tl = t.map(tl);
    tf.tr = t.map(tr);
    tf.bl = t.map(bl);
    tf.br = t.map(br);
    tf.cr = t.map(cr);
    return tf;
  }

  inline double lh() const
  {
    return distancePointf(tl, bl);
  }

  inline double mh() const
  {
    return distancePointf(mt(), mb());
  }

  inline double rh() const
  {
    return distancePointf(tr, br);
  }

  inline double tw() const
  {
    return distancePointf(tl, tr);
  }

  inline double mw() const
  {
    return distancePointf(ml(), mr());
  }

  inline double bw() const
  {
    return distancePointf(br, bl);
  }

  inline void translate(const QPointF& o)
  {
    tl += o;
    tr += o;
    bl += o;
    br += o;
    cr += o;
  }

  inline QRectF boundingRect() const
  {
    return QRectF(tl, tr) | QRectF(bl, br);
  }

  void horizontalReflection();
  void verticalReflection();

  void flipHorizontal();
  void flipVertical();


  void rotate(double);

  void prepareTransformations(double scale);
  void prepareTransformations(TransformFrameControl control, const QPointF&, double scale);

  void setControlPos(const QPointF& position, bool reflection, bool proportional, bool fixed);
  void setControlPos(uint control, const QPointF& position, bool reflection, bool proportional, bool fixed);

  void setProjectionPos(const QPointF& position, bool reflection, bool proportional, bool fixed);
  void setProjectionPos(TransformFrameControl control, const QPointF& position, bool reflection, bool proportional, bool fixed);

  QPointF getControlPos(TransformFrameControl) const;

  bool getTransform(QTransform&) const;

  bool operator==(const TransformFrame&) const;

private:
  QPointF _tl, _tr, _bl, _br, _cr;
  QRectF _initial_rect;
  QPointF _initial_cr;
  QTransform _canvas_to_shape_tx;
  QTransform _shape_to_canvas_tx;
  QPointF _p0, _p1;
  double _vector_t;
  double _scale;
  TransformFrameControl _control;
  QPolygonF _polygon;

  void _position_controls_to(const QRectF& r, const QTransform&);

};



#endif // FLTRANSFORMFRAME_H
