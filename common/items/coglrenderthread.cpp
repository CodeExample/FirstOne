#include "coglrenderthread.h"
#include "coglstrokesrender.h"
#include "cprojecthandler.h"

#include <QDebug>

using namespace sm;

COGLRenderThread::COGLRenderThread(QObject*parent) : QThread(parent)
{
  hprj->registerThread(this);
  connect(hprj, SIGNAL(threadsStop()), this, SLOT(stop()));
}


void COGLRenderThread::stop()
{
  QMutexLocker l(&_mutex);
  _stopped = true;
}

void COGLRenderThread::init(const COGLStrokesRender &strokes_render)
{
  _strokes_render = strokes_render;
  _strokes_render.oglContext()->moveToThread(this);
}

bool COGLRenderThread::stopped()
{
  QMutexLocker l(&_mutex);
  return _stopped;
}

bool COGLRenderThread::suspended()
{
  QMutexLocker l(&_mutex);
  return _suspended;
}

void COGLRenderThread::suspend()
{
  QMutexLocker l(&_mutex);
  _suspended = true;
}

void COGLRenderThread::resume()
{
  QMutexLocker l(&_mutex);
  _suspended = false;
}




bool COGLRenderThread::queueIsEmpty()
{
  QMutexLocker l(&_mutex);
  return _queue.isEmpty();
}


int COGLRenderThread::queueCount()
{
  QMutexLocker l(&_mutex);
  return _queue.count();
}


COGLRenderThreadTask COGLRenderThread::queueTakeRenderTask()
{
  QMutexLocker l(&_mutex);
  return _queue.takeFirst();
}


void COGLRenderThread::remove(ObjId tmpl_id)
{
  QMutexLocker l(&_mutex);
  _remove_tmpl_data(tmpl_id);
}


void COGLRenderThread::_remove_tmpl_data(ObjId tmpl_id)
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


void COGLRenderThread::append(const COGLRenderThreadTask &redner_task)
{
  QMutexLocker l(&_mutex);
  _remove_tmpl_data(redner_task.tmpl_data->tmpl_id);
  _queue << redner_task;
}



void COGLRenderThread::run()
{
  while(!_stopped)
  {
    while(queueIsEmpty() && !stopped())
    {
      msleep(10);
    }

    while(suspended() && !stopped())
    {
      msleep(10);
    }

    COGLRenderThreadTask render_task = queueTakeRenderTask();
    msleep(1000);

    StrokesTemplateData * tmpl_data = render_task.tmpl_data;
    COGLPainterDataSharedPtr painter_data = render_task.painter_data;

    QMutexLocker l(&painter_data->mutex); Q_UNUSED(l);

    _strokes_render.setSourceImage(hprj->sourceImage());
    _strokes_render.setTemplateData(*tmpl_data);
    _strokes_render.setPainterData(painter_data);

    _strokes_render.createStrokeCL(QTransform(), tmpl_data->create_rect);
    _strokes_render.render();

    emit strokesCreated(tmpl_data->tmpl_id, _strokes_render.painterData());

    if (queueIsEmpty())
      break;
  }
}

