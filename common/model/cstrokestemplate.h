#ifndef CSTROKESTEMPLATE_H
#define CSTROKESTEMPLATE_H


#include "cgraphics.h"
#include "cmodel.h"
#include "cmask.h"

#include <QImage>
namespace sm
{

struct StrokesTemplateData
{
  ObjId tmpl_id;
  QRectF bounding_rect;
  QRectF create_rect;

  float thickness=2;
  float thickness_base=0;
  float basrelief_rise=0;
  float threshold=0;
  float dotline_threshold=0;
  float white=0.1;
  float black=0.1;
  float interval=4;
  float line_position=0;       // 0..1
  float angle=0;               //-180..180
  float item_angle=0;          //-180..180
  uint smooth=5;
  int steps_on_px=6;

  bool is_knife = false;
  bool is_edged = false;
  bool is_cell = false;
  float cell_size = 1;


  QImage edges_img;
  QVector<float> edges_track;
  QVector<uint> edges_indexes;

  QString name;

  bool round_cups = true;

  bool is_changed=true;
  int data_version=0;

  QRectF mask_boundingRect;
  QTransform mask_transform;
  QPainterPath mask_path;

  void assignParams(StrokesTemplateData *d);
  void assignParams(const StrokesTemplateData &d);
  void assignEdges(const StrokesTemplateData &d);

  bool isEquivalent(StrokesTemplateData *d);
  bool isEquivalent(const StrokesTemplateData &d);

};


class StrokesTemplateImpl : public GraphicsImpl, public StrokesTemplateData
{
public:
  int type() const override
  {
    return Template;
  }

  StrokesTemplateImpl(const QString &name) : GraphicsImpl(name)
  {
    tmpl_id = getId();
  }

  bool iterate(IVisitor * v) override;

  QRectF boundingRect() const override
  {
    return bounding_rect;
  }

  virtual QPointF tangent(const QPointF&) const;
  void setBoundingRect(const QRectF&);
};


class CStrokesTemplate : public CGraphics
{
public:
  static CGraphics createInstance(const QString &name ="")
  {
    return CGraphics(new StrokesTemplateImpl(name));
  }

  CStrokesTemplate() : CGraphics() {}
  CStrokesTemplate(const CObject &object) : CGraphics(object) {}

  inline StrokesTemplateData * data() const
  {
    IMPL_D(StrokesTemplate);
    return d;
  }

  inline QPointF tangent(const QPointF &p) const
  {
    IMPL_D(StrokesTemplate);
    return d->tangent(p);
  }

  inline void setBoundingRect(const QRectF &r) const
  {
    IMPL_D(StrokesTemplate);
    d->setBoundingRect(r);
  }

  inline bool isChanged() const
  {
    return data()->is_changed;
  }

  inline void clearChanges() const
  {
    IMPL_D(StrokesTemplate);
    d->is_changed = false;
  }

};

}
#endif // CSTROKESTEMPLATE_H
