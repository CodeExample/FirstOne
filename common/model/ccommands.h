#ifndef CCOMMANDS_H
#define CCOMMANDS_H


#include "clayer.h"
#include "cstrokestemplate.h"
#include "cmask.h"

#include <QUndoCommand>


namespace sm
{

class commAddToLayer : public QUndoCommand
{
public:
  commAddToLayer(const QString&, const CLayer &dest_layer, const CGraphics &object, bool activate=false);
  commAddToLayer(const QString&, const CGraphics&, bool activate=false);

  void undo();
  void redo();

private:
  CLayer _dest_layer;
  CGraphics _object;
  bool _activate=false;
  CLayer _active_layer;
};


class commEditMask : public QUndoCommand
{
public:
  commEditMask(const QString&, const CMask&, const QPainterPath &);

  void undo();
  void redo();

private:
  CMask _mask;
  CStrokesTemplate _tmpl;
  QPainterPath _old_path, _new_path;
};



class commEditParams : public QUndoCommand
{
public:
  commEditParams(const QString&, const CStrokesTemplate&, const StrokesTemplateData&, QUndoCommand*parent=0);

  void undo();
  void redo();

private:
  CStrokesTemplate _tmpl;
  StrokesTemplateData _old_params, _new_params;
  bool _initialized=false;
};


class commSetTransform : public QUndoCommand
{
public:
  typedef QPair<CGraphics, QTransform> CommPair;

  commSetTransform(const QString&, const QList<CommPair> &old_tx, const QList<CommPair> &new_tx);

  void undo();
  void redo();

private:
  QList<CommPair> _old_tx_list, _new_tx_list;
  bool _initialized=false;
};



class commSaveState : public QUndoCommand
{
public:
  typedef QPair<CGraphics, QDomElement> CommPair;

  commSaveState(const QString&, const QList<CGraphics>&);

  void undo();
  void redo();

  void updateStates();

private:
  QList<CommPair> _old_states, _new_states;
  bool _initialized=false;
  QList<CGraphics> _objects;
};


class commGroup : public QUndoCommand
{
public:
  static bool validate(const QList<CGraphics>&);

  commGroup(const QString&, const QList<CGraphics>&);

  void undo();
  void redo();

private:
  QList<CGraphics> _selection;
  CGraphics _owner;
  CGraphics _group;
};


class commUngroup : public QUndoCommand
{
public:
  static bool validate(const QList<CGraphics>&);

  commUngroup(const QString&, const CGraphics&);

  void undo();
  void redo();

private:
  QList<CGraphics> _selection;
  CGraphics _owner;
  CGraphics _group;
};





}
#endif // CCOMMANDS_H
