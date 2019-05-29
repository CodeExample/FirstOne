#ifndef CRENDERTHREAD_H
#define CRENDERTHREAD_H


#include "cobject.h"
#include "coglpainterdata.h"
#include "coclstrokesrender.h"

#include <QThread>

namespace sm
{
struct COCLRenderThreadTask
{
  StrokesTemplateData * tmpl_data;
  COGLPainterDataSharedPtr painter_data;
  QList<KnifeRec> knives_recs;
};

class COCLRenderThread : public QThread
{
  Q_OBJECT

public:
  COCLRenderThread(QObject*);

  void init(const COCLStrokesRender&);

  void append(const COCLRenderThreadTask&);

  void remove(ObjId tmpl_id);

  void run();

  bool stopped();

  bool queueIsEmpty();

  int  queueCount();

  COCLRenderThreadTask queueTakeRenderTask();


protected:
  QMutex _mutex;

  COCLStrokesRender _strokes_render;

  QList<COCLRenderThreadTask> _queue;

  bool _stopped=false;

  void _remove_tmpl_data(ObjId tmpl_id);

signals:
  void knifeEdgesCreated(int, StrokesTemplateData*);
  void strokesCreated(int, COGLPainterDataSharedPtr);

public slots:
  void stop();

};

}

#endif // CRENDERTHREAD_H
