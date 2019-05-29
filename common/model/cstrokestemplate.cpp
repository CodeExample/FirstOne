#include "cstrokestemplate.h"

using namespace sm;

bool StrokesTemplateImpl::iterate(IVisitor * v)
{
  return v->visitTo(this);
}
/*
void StrokesTemplateImpl::addMask(const QString &mask_name)
{
  if (!masks.contains(mask_name))
  {
    masks.append(mask_name);
    ++data_version;
  }
}
*/



QPointF StrokesTemplateImpl::tangent(const QPointF&) const
{
  QLineF l(0,0,1,0);
  l.setAngle(angle);
  return l.p2();
}


void StrokesTemplateImpl::setBoundingRect(const QRectF &r)
{
  bounding_rect =
      create_rect = r;

  is_changed = true;
}


void StrokesTemplateData::assignParams(StrokesTemplateData *d)
{
  assignParams(*d);
}


void StrokesTemplateData::assignParams(const StrokesTemplateData &d)
{
  bounding_rect = d.bounding_rect;
  create_rect = d.create_rect;

  thickness = d.thickness;
  thickness_base = d.thickness_base;
  basrelief_rise = d.basrelief_rise;
  threshold = d.threshold;
  dotline_threshold = d.dotline_threshold;
  white = d.white;
  black = d.black;
  interval = d.interval;
  line_position = d.line_position;
  angle = d.angle;
  item_angle = d.item_angle;
  smooth = d.smooth;
  steps_on_px = d.steps_on_px;

  is_knife = d.is_knife;
  is_edged = d.is_edged;
  is_cell = d.is_cell;
  cell_size = d.cell_size;

  name = d.name;

  round_cups = d.round_cups;
}


void StrokesTemplateData::assignEdges(const StrokesTemplateData &d)
{
  edges_img = d.edges_img;
  edges_indexes = d.edges_indexes;
  edges_track = d.edges_track;

}


bool StrokesTemplateData::isEquivalent(StrokesTemplateData *d)
{
  return isEquivalent(*d);
}


bool StrokesTemplateData::isEquivalent(const StrokesTemplateData &d)
{
  return bounding_rect == d.bounding_rect &&
      create_rect == d.create_rect &&
      thickness == d.thickness &&
      thickness_base == d.thickness_base &&
      basrelief_rise == d.basrelief_rise &&
      threshold == d.threshold &&
      dotline_threshold == d.dotline_threshold &&
      white == d.white &&
      black == d.black &&
      interval == d.interval &&
      line_position == d.line_position &&
      angle == d.angle &&
      item_angle == d.item_angle &&
      smooth == d.smooth &&
      steps_on_px == d.steps_on_px &&
      is_knife == d.is_knife &&
      is_edged == d.is_edged &&
      is_cell == d.is_cell &&
      cell_size == d.cell_size &&
      name == d.name &&
      round_cups == d.round_cups;
}
