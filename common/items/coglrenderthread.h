#ifndef CRENDERTHREAD_H
#define CRENDERTHREAD_H


#include "cobject.h"
#include "coglpainterdata.h"
#include "coglstrokesrender.h"

#include <QThread>

namespace sm
{
struct COGLRenderThreadTask
{
  StrokesTemplateData * tmpl_data;
  COGLPainterDataSharedPtr painter_data;
  //QList<KnifeRec> knives_recs;
};

class COGLRenderThread : public QThread
{
  Q_OBJECT

public:
  COGLRenderThread(QObject*);

  void init(const COGLStrokesRender&);

  void append(const COGLRenderThreadTask&);

  void remove(ObjId tmpl_id);

  void run();

  bool stopped();


  bool queueIsEmpty();

  int  queueCount();

  COGLRenderThreadTask queueTakeRenderTask();

  bool suspended();

  void suspend();

  void resume();

protected:
  QMutex _mutex;

  COGLStrokesRender _strokes_render;

  QList<COGLRenderThreadTask> _queue;

  bool _stopped=false;

  bool _suspended=false;

  void _remove_tmpl_data(ObjId tmpl_id);

signals:
  void strokesCreated(int, COGLPainterDataSharedPtr);

public slots:
  void stop();

};

}

#endif // CRENDERTHREAD_H
