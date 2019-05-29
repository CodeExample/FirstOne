#include "coglpainterthread.h"

#include "coglstrokespainter.h"
#include "cprojecthandler.h"

#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QPainter>

#include <QDebug>

using namespace  sm;

COGLPainterThread::COGLPainterThread(QObject * parent) : QThread(parent)
{
  _strip = 0;
  hprj->registerThread(this);
  connect(hprj, SIGNAL(threadsStop()), this, SLOT(stop()));
}


bool COGLPainterThread::initContext(QOpenGLContext *owner_cntx)
{
  _offscrnsf = new QOffscreenSurface();
  _offscrnsf->create();
  if (!_offscrnsf->isValid()) {
    qDebug() << "Surface Failed";
    return false;
  }

  _context = new QOpenGLContext();
  _context->setShareContext(owner_cntx);
  _context->create();
  if (!_context->isValid()) {
    qDebug() << "Context Failed";
    return false;
  }
  if (!_context->areSharing(owner_cntx, _context)) {
    qDebug() << "Sharing Failed";
    return false;
  }
  _context->moveToThread(this);
  return true;
}


void COGLPainterThread::releaseContext()
{
  delete _offscrnsf;
  delete _context;
}


void COGLPainterThread::run()
{
  QTime t; t.start();

  while(!stopped() && !queueIsEmpty())
  {
    _render_painting();

    if (t.elapsed() > 500 || queueIsEmpty())
    {
      hprj->scene()->update();
      t.restart();
    }
  }
}


void COGLPainterThread::stop()
{
  QMutexLocker l(&_mutex);
  _stop = true;
}


bool COGLPainterThread::queueIsEmpty()
{
  QMutexLocker l(&_mutex);
  return _task_map.isEmpty();
}


int COGLPainterThread::queueCount()
{
  QMutexLocker l(&_mutex);
  return _task_map.count();
}


void COGLPainterThread::append(ObjId tmpl_id, const PainterThreadTask &task)
{
  QMutexLocker l(&_mutex);
  KeyPair key(tmpl_id, task.viewport);
  _task_map.insert(key, task);
}


void COGLPainterThread::remove(ObjId tmpl_id)
{
  QMutexLocker l(&_mutex);
  for(KeyPair key : _task_map.keys())
  {
    if (key.first == tmpl_id)
      _task_map.remove(key);
  }
}


PainterThreadTask COGLPainterThread::takeTask()
{
  QMutexLocker l(&_mutex);
  KeyPair key = _task_map.firstKey();
  return _task_map.take(key);
}


void COGLPainterThread::_render_painting()
{
  if (_task_map.isEmpty())
    return;

  PainterThreadTask task = takeTask();
  QOpenGLWidget * view = static_cast<QOpenGLWidget*>(task.viewport);

  if (!initContext(view->context()))
    return;

  COGLPainterData * painter_data = task.painter_data;

  QTransform buff_tx = task.item_to_pixmap_tx;
  QRect buff_rect = task.item_view_r;

  _context->makeCurrent(_offscrnsf);

  QOpenGLFramebufferObjectFormat fboFormat;
  fboFormat.setSamples(8);
  fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

  QOpenGLFramebufferObject fbo(buff_rect.size(), fboFormat);

  bool is_ready = false;

  if (fbo.bind())
  {
    QMutexLocker l(&painter_data->mutex); Q_UNUSED(l);

    QOpenGLPaintDevice device(buff_rect.size());
    QPainter painter;
    painter.begin(&device);
    {
      painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
      painter.setTransform(buff_tx);
      COGLStrokesPainter sp;
      sp.paint(&painter, painter_data, COGLStrokesPainter::TriangleStrip);
      painter.resetTransform();
    }
    painter.end();
    fbo.release();

    task.pixmap = fbo.toImage();
    is_ready = true;

  }
  _context->doneCurrent();

  releaseContext();

  if (is_ready)
    emit ready(painter_data->tmpl_id, task);

}




