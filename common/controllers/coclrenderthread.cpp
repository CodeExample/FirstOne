#include "coclrenderthread.h"
#include "coclstrokesrender.h"
#include "cprojecthandler.h"

#include <QDebug>

using namespace sm;

COCLRenderThread::COCLRenderThread(QObject*parent) : QThread(parent)
{
  hprj->registerThread(this);
  connect(hprj, SIGNAL(threadsStop()), this, SLOT(stop()));
}


void COCLRenderThread::stop()
{
  QMutexLocker l(&_mutex);
  _stopped = true;
}

void COCLRenderThread::init(const COCLStrokesRender &strokes_render)
{
  _strokes_render = strokes_render;
  _strokes_render.oglContext()->moveToThread(this);
}

bool COCLRenderThread::stopped()
{
  QMutexLocker l(&_mutex);
  return _stopped;
}

bool COCLRenderThread::queueIsEmpty()
{
  QMutexLocker l(&_mutex);
  return _queue.isEmpty();
}


int COCLRenderThread::queueCount()
{
  QMutexLocker l(&_mutex);
  return _queue.count();
}


COCLRenderThreadTask COCLRenderThread::queueTakeRenderTask()
{
  QMutexLocker l(&_mutex);
  return _queue.takeFirst();
}


void COCLRenderThread::remove(ObjId tmpl_id)
{
  QMutexLocker l(&_mutex);
  _remove_tmpl_data(tmpl_id);
}


void COCLRenderThread::_remove_tmpl_data(ObjId tmpl_id)
{
  for(int i=0; i < _queue.count(); ++i)
  {
    if (_queue.at(i).tmpl_data->tmpl_id == tmpl_id)
    {
      _queue.removeAt(i);
      break;
    }
  }
}


void COCLRenderThread::append(const COCLRenderThreadTask &redner_task)
{
  QMutexLocker l(&_mutex);
  _remove_tmpl_data(redner_task.tmpl_data->tmpl_id);
  _queue << redner_task;
}



void COCLRenderThread::run()
{
  while(!_stopped)
  {
    while(queueIsEmpty() && !stopped())
    {
      msleep(10);
    }

    COCLRenderThreadTask render_task = queueTakeRenderTask();

    StrokesTemplateData * tmpl_data = render_task.tmpl_data;
    COGLPainterDataSharedPtr painter_data = render_task.painter_data;

    QMutexLocker l(&painter_data->mutex); Q_UNUSED(l);

    _strokes_render.setSourceImage(hprj->sourceImage());
    _strokes_render.setTemplateData(*tmpl_data);
    _strokes_render.setPainterData(painter_data);

    _strokes_render.createStrokeCL(QTransform(), tmpl_data->create_rect, render_task.knives_recs);
    _strokes_render.render();

    if (tmpl_data->is_knife && !_strokes_render.tmplData().edges_indexes.isEmpty())
    {
      tmpl_data->assignEdges(_strokes_render.tmplData());
      emit knifeEdgesCreated(tmpl_data->tmpl_id, tmpl_data);
    }

    emit strokesCreated(tmpl_data->tmpl_id, _strokes_render.painterData());

    if (queueIsEmpty())
      break;
  }
}

