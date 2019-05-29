#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QImage>
#include <QThread>

#include "cobject.h"
#include "citems.h"
#include "coglpainterdata.h"

namespace sm
{
struct PainterThreadTask
{
  QWidget * viewport;
  COGLPainterData * painter_data;
  QTransform item_to_pixmap_tx;
  QRectF pixmap_rect_on_item;
  QRect item_view_r;
  QImage pixmap;
};


class COGLPainterThread : public QThread
{
  Q_OBJECT

public:
  COGLPainterThread(QObject*parent=nullptr);

  bool initContext(QOpenGLContext*);
  void releaseContext();

  QColor _color;

  int _strip=0;


  bool stopped()
  {
    QMutexLocker l(&_mutex);
    return _stop;
  }

  void append(ObjId, const PainterThreadTask&);

  void remove(ObjId tmpl_id);

  bool queueIsEmpty();

  int queueCount();

  PainterThreadTask takeTask();


signals:
  void ready(int, PainterThreadTask);

protected:
  void run();

private:
  QOpenGLContext *_context=0;
  QOffscreenSurface *_offscrnsf=0;

  QMutex _mutex;

  typedef QPair<ObjId, QWidget*> KeyPair;
  QList<KeyPair> _queue;
  QMap<KeyPair, PainterThreadTask> _task_map;

  bool _stop=false;

  void _render_painting();

public slots:
  void stop();

};

}
#endif // RENDERTHREAD_H
