#include "crendermanager.h"

#include "cprojecthandler.h"
#include "cstrokestemplate.h"

using namespace sm;

namespace sm
{
  struct StrokesTemplateData;
}

void CRenderManager::unregisterItem(ObjId tmpl_id)
{
  if (render_thread)
    render_thread->remove(tmpl_id);

  if (painter_thread)
    painter_thread->remove(tmpl_id);
}


int CRenderManager::queueRenderCount() const
{
  return render_thread ? render_thread->queueCount() : 0;
}


int CRenderManager::queuePainterCount() const
{
  return painter_thread ? painter_thread->queueCount() : 0;
}


void CRenderManager::append(const COGLRenderThreadTask &render_task)
{
qDebug() << "append" << render_task.painter_data->tmpl_id;
  QMutexLocker l(&mutex);
  if (!render_thread)
  {
    COGLStrokesRender strokes_render(true);
    strokes_render.initContext(hprj->context());

    render_thread = new COGLRenderThread(this);
    render_thread->init(strokes_render);

    connect(render_thread, SIGNAL(strokesCreated(int, COGLPainterDataSharedPtr)), this,
            SLOT(_strokes_created(int, COGLPainterDataSharedPtr)), Qt::BlockingQueuedConnection);

    connect(render_thread, SIGNAL(finished()), this,
            SLOT(_render_finished()), Qt::BlockingQueuedConnection);

    render_thread->start();
  }
  render_thread->append(render_task);
}


void CRenderManager::append(ObjId tmpl_id, const PainterThreadTask &painter_task)
{
  QMutexLocker l(&mutex);
  if (!painter_thread)
  {
    painter_thread = new COGLPainterThread(this);
    painter_thread->start();

    connect(painter_thread, SIGNAL(ready(int, const PainterThreadTask&)), this,
            SLOT(_painter_ready(int, const PainterThreadTask&)), Qt::BlockingQueuedConnection);

    connect(painter_thread, SIGNAL(finished()), this,
            SLOT(_painter_finished()), Qt::BlockingQueuedConnection);

    painter_thread->start();
  }
  painter_thread->append(tmpl_id, painter_task);
}


void CRenderManager::_render_finished()
{
  render_thread->deleteLater();
  render_thread = 0;
}

void  CRenderManager::_painter_finished()
{
  painter_thread->deleteLater();
  painter_thread = 0;
}


void CRenderManager::_strokes_created(int tmpl_id, COGLPainterDataSharedPtr painter_data)
{
  emit strokesCreated(tmpl_id, painter_data);
}


void CRenderManager::_painter_ready(int tmpl_id, const PainterThreadTask &task)
{
  emit painterReady(tmpl_id, task);
}


void CRenderManager::_knife_edges_created(int , StrokesTemplateData * )
{
//  emit knifeEdgesCreated(tmpl_id, tmpl_data);
}


CRenderManager * CRenderManager::_instance=nullptr;
