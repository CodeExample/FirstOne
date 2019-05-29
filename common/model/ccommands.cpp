#include "ccommands.h"
#include "cgroup.h"
#include "cprojecthandler.h"

using namespace sm;

//
// commAddToLayer
//
commAddToLayer::commAddToLayer(const QString &caption, const CLayer &dest_layer, const CGraphics &o, bool activate) : QUndoCommand(caption),
  _dest_layer(dest_layer), _object(o), _activate(activate)
{
}


commAddToLayer::commAddToLayer(const QString &caption, const CGraphics &o, bool activate) : QUndoCommand(caption),
 _object(o), _activate(activate)
{
}


void commAddToLayer::undo()
{
  if (_activate)
  {
    if (_active_layer)
    {
      hprj->document().setActiveLayer(_active_layer);
    }
  }
  _dest_layer.remove(_object);

  emit hprj->objectRemoved(_object, _dest_layer);
}


void commAddToLayer::redo()
{
  if (!_dest_layer)
  {
    _dest_layer = hprj->document().activeLayer();
  }

  _dest_layer.add(_object);

  if (_activate)
  {
    if (_object.type() == Layer)
    {
      _active_layer = hprj->document().activeLayer();
      hprj->document().setActiveLayer(_object);
    }
  }

  hprj->select(_object, false);

  emit hprj->objectAdded(_object, _dest_layer);
}


//
// commEditMask
//
commEditMask::commEditMask(const QString &caption, const CMask &mask, const QPainterPath &path) :
  QUndoCommand(caption), _mask(mask)
{
  _old_path = _mask.path();
  _new_path = path;
}

void commEditMask::undo()
{
  _mask.setPath(_old_path);
  if (_tmpl)
    _tmpl.setBoundingRect(_old_path.boundingRect());
}


void commEditMask::redo()
{
  _mask.setPath(_new_path);
  if (_tmpl)
    _tmpl.setBoundingRect(_new_path.boundingRect());
}


//
// commEditParams
//
commEditParams::commEditParams(const QString &caption, const CStrokesTemplate &tmpl,
                               const StrokesTemplateData &old_params, QUndoCommand*parent) :
  QUndoCommand(caption, parent), _tmpl(tmpl), _old_params(old_params)
{
}


void commEditParams::undo()
{
  _tmpl.data()->assignParams(_old_params);
  _tmpl.data()->is_changed = true;
  _tmpl.notifyDataChanged();
}


void commEditParams::redo()
{
  if (!_initialized)
  {
    _new_params = *_tmpl.data();
    _initialized = true;
  } else {
    _tmpl.data()->assignParams(_new_params);
    _tmpl.data()->is_changed = true;
    _tmpl.notifyDataChanged();
  }
}


//
// commSetTransform
//
commSetTransform::commSetTransform(const QString &caption, const QList<CommPair> &old_tx_list, const QList<CommPair> &new_tx_list) :
  QUndoCommand(caption), _old_tx_list(old_tx_list), _new_tx_list(new_tx_list)
{
}

void commSetTransform::undo()
{
  for(const CommPair &pair : _old_tx_list)
  {
    CGraphics obj = pair.first;
    obj.setTransform(pair.second);
  }
}

void commSetTransform::redo()
{
  if (!_initialized)
  {
    _initialized = true;
    return;
  }
  for(const CommPair &pair : _new_tx_list)
  {
    CGraphics obj = pair.first;
    obj.setTransform(pair.second);
  }
}

//
// commSetState
//
commSaveState::commSaveState(const QString &caption, const QList<CGraphics> &objects) :
  QUndoCommand(caption), _objects(objects)
{
  QDomDocument doc("doc");
  for(const CGraphics &obj : objects)
  {
    QDomElement elm = doc.createElement("object");
    obj.write(elm, doc, false);

    _old_states << CommPair(obj, elm);
  }
}

void commSaveState::undo()
{
  for(const CommPair &pair : _old_states)
  {
    CGraphics obj = pair.first;
    obj.read(pair.second);
  }
}


void commSaveState::updateStates()
{
  QDomDocument doc("doc");
  _new_states.clear();
  for(const CGraphics &obj : _objects)
  {
    QDomElement elm = doc.createElement("object");
    obj.write(elm, doc, false);

    _new_states << CommPair(obj, elm);
  }
}


void commSaveState::redo()
{
  if (_initialized)
  {
    for(const CommPair &pair : _new_states)
    {
      CGraphics obj = pair.first;
      obj.read(pair.second);
    }
    return;
  }
  _initialized = true;

  updateStates();
}


//
// commGroup
//
bool commGroup::validate(const QList<CGraphics> &selection)
{
  if (selection.count() < 2)
    return false;

  CGraphics owner = selection.first().owner();
  for(const CGraphics &object : selection)
  {
    if (object.owner() != owner)
      return false;
  }
  return true;
}


commGroup::commGroup(const QString &text, const QList<CGraphics> &selection) : QUndoCommand(text),
  _selection(selection)
{
}


void commGroup::undo()
{
  hprj->unselectAll();

  _owner.remove(_group);
  emit hprj->objectRemoved(_group, _owner);

  for(const CGraphics &object : _selection)
  {
    _group.remove(object);
    emit hprj->objectRemoved(object, _group);
    _owner.add(object);
    emit hprj->objectAdded(object, _owner);
  }
  hprj->select(_selection);
}


void commGroup::redo()
{
  if (!_owner)
  {
    _owner = _selection.first().owner();
    _group = CGroup::createInstance();
  }

  for(const CGraphics &object : _selection)
  {
    _owner.remove(object);
    emit hprj->objectRemoved(object, _owner);
    _group.add(object);
    emit hprj->objectAdded(object, _group);
  }

  _owner.add(_group);
  emit hprj->objectAdded(_group, _owner);

  hprj->select(_group);
}


//
// commUngroup
//
bool commUngroup::validate(const QList<CGraphics> &selection)
{
  if (selection.count() != 1)
    return false;

  return selection.first().type() == Group;
}


commUngroup::commUngroup(const QString &text, const CGraphics &group) : QUndoCommand(text),
  _group(group)
{
  _selection = _group.objects();
  _owner = _group.owner();
}


void commUngroup::undo()
{
  QTransform group_tx = _group.transform().inverted();
  for(const CGraphics &object : _selection)
  {
    _owner.remove(object);
    emit hprj->objectRemoved(object, _owner);
    object.setTransform(object.transform()*group_tx);
    _group.add(object);
    emit hprj->objectAdded(object, _group);
  }

  _owner.add(_group);
  emit hprj->objectAdded(_group, _owner);

  hprj->select(_group);
}


void commUngroup::redo()
{
  hprj->unselectAll();

  _owner.remove(_group);
  emit hprj->objectRemoved(_group, _owner);

  QTransform group_tx = _group.transform();

  for(const CGraphics &object : _selection)
  {
    _group.remove(object);
    emit hprj->objectRemoved(object, _group);
    object.setTransform(object.transform()*group_tx);
    _owner.add(object);
    emit hprj->objectAdded(object, _owner);
  }
  hprj->select(_selection);
}

