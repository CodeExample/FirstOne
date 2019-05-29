#include "cselectiontool.h"

CSelectionTool::CSelectionTool() : CFrameTool()
{
}


void CSelectionTool::toolActivated()
{
  _view->setCursor(Qt::ArrowCursor);
}


bool CSelectionTool::frameAction(const QRectF &, QGraphicsSceneMouseEvent * )
{
  if (~event->modifiers() & Qt::ShiftModifier)
  {
    vxDocument document = vxEnvironment::activeDocument();
    document.unselectAll();
  }

  if (event->modifiers() & Qt::ControlModifier)
  {
    _select_in_frame(scene_rect, event->modifiers() & Qt::ShiftModifier);
  } else {
    _select_intersect_frame(scene_rect, event->modifiers() & Qt::ShiftModifier);
  }

  return true;
}


void CSelectionTool::_select_in_frame(const QRectF &, bool)
{
  vxDocument document = vxEnvironment::activeDocument();

  QList<vxGraphicsObject> objects = document.getObjects();

  QSet<GroupIndex> to_select_groups;
  QSet<GroupIndex> to_unselect_groups;

  for(QList<vxGraphicsObject>::iterator iter=objects.begin(); iter!=objects.end(); ++iter)
  {
    vxGraphicsObject object = *iter;

    if (object.isLocked() || !object.isVisible())
      continue;

    QGraphicsItem * item = object.graphicsItem();

    QRectF item_rect = item->mapRectToScene(item->boundingRect());

    if (scene_rect.contains(item_rect))
    {
      object.setSelected(true);
      if (object.isGroupEnabled())
        to_select_groups.insert(object.groupIndex());

    } else
    if (!keep_selection)
    {
      object.setSelected(false);
      if (object.isGroupEnabled())
        to_unselect_groups.insert(object.groupIndex());
    }
  }

  document.selectGroups(to_select_groups);
  document.unselectGroups(to_unselect_groups);

  vxEnvironment::notifySelectionChanged();
}


void CSelectionTool::_select_intersect_frame(const QRectF &, bool)
{
  vxDocument document = vxEnvironment::activeDocument();

  QList<vxGraphicsObject> objects = document.getObjects();

  QSet<GroupIndex> to_select_groups;
  QSet<GroupIndex> to_unselect_groups;

  for(QList<vxGraphicsObject>::iterator iter=objects.begin(); iter!=objects.end(); ++iter)
  {
    vxGraphicsObject object = *iter;

    if (object.isLocked() || !object.isVisible())
      continue;

    QGraphicsItem * item = object.graphicsItem();

    QRectF item_rect = item->type() == BigImageItem
        ? static_cast<vxBigImageItem*>(item)->croppingRect()
        : item->boundingRect();

    item_rect = item->mapRectToScene(item_rect);

    if (scene_rect.intersects(item_rect))
    {
      object.setSelected(true);
      if (object.isGroupEnabled())
        to_select_groups.insert(object.groupIndex());

    } else
    if (!keep_selection)
    {
      object.setSelected(false);
      if (object.isGroupEnabled())
        to_unselect_groups.insert(object.groupIndex());
    }
  }

  document.selectGroups(to_select_groups);
  document.unselectGroups(to_unselect_groups);

  vxEnvironment::notifySelectionChanged();
}


bool CSelectionTool::clickAction(QGraphicsSceneMouseEvent *)
{
  vxProject * project = vxEnvironment::activeProject();

  if (!project)
    return false;

  double resolution = project->viewResolution(_view);

  vxDocument document = vxEnvironment::activeDocument();

  QPointF scene_pos = event->scenePos();

  bool keep_selection = event->modifiers() & Qt::ShiftModifier;

  if (!keep_selection)
  {
    document.unselectAll();
  }

  QSet<GroupIndex> to_select_groups;
  QSet<GroupIndex> to_unselect_groups;

  QList<vxGraphicsObject> objects = document.getObjects(true);

  if (!keep_selection)
  {
    foreach(const vxGraphics &object, objects)
    {
      if (object.isSelected())
        object.setSelected(false);

      if (!vxGraphicsObject::isGraphicsObject(object))
        objects.removeOne(object);
    }
  }

  bool was_selected = false;
  vxGraphicsObject hit_object;

  for(vxGraphicsObject object : objects)
  {
    if (object.isLocked() || !object.isVisible())
      continue;

    if (object.objectType() > 0xA)
      continue;

    if (object.isHit(scene_pos, resolution) && !was_selected)
    {
      was_selected = true;
      bool s = !object.isSelected();
      object.setSelected(s);
      if (s)
        hit_object = object;
      if (object.isGroupEnabled())
      {
        if (s)
          to_select_groups.insert(object.groupIndex());
        else
          to_unselect_groups.insert(object.groupIndex());
      }
    }
  }

  if (hit_object)
  {
    if (hit_object.owner().objectType() == Layer)
    {
      document.setActiveLayer(hit_object.owner());
    }
  }

  document.selectGroups(to_select_groups);
  document.unselectGroups(to_unselect_groups);

  vxEnvironment::notifySelectionChanged();

  return true;
}









