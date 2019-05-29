#ifndef CRENDERMANAGER_H
#define CRENDERMANAGER_H

#include "coglpainterthread.h"
#include "coglrenderthread.h"

#include <QObject>

namespace sm
{
  struct StrokesTemplateData;
}

using namespace sm;


class CRenderManager : public QObject
{
  Q_OBJECT

public:

  static CRenderManager * instance()
  {
    if (!_instance)
      _instance = new CRenderManager();

    return _instance;
  }


  void unregisterItem(ObjId tmpl_id);

  void append(const COGLRenderThreadTask &render_task);

  void append(ObjId tmpl_id, const PainterThreadTask &painter_task);

  int queueRenderCount() const;
  int queuePainterCount() const;

private:
  static CRenderManager * _instance;
  static uint _counter_id;

  QMutex mutex;
  COGLRenderThread * render_thread=0;

  COGLPainterThread * painter_thread=0;

  CRenderManager()
  {
  }

  struct PaintingData
  {
    QImage buff_img;
    QTransform buff_tx;
    QRect buff_rect;
  };

  QMap<uint, PaintingData> _painting_data;

private slots:
  void _render_finished();

  void _painter_finished();

  void _strokes_created(int tmpl_id, COGLPainterDataSharedPtr painter_data);

  void _painter_ready(int tmpl_id, const PainterThreadTask &task);

  void _knife_edges_created(int tmpl_id, StrokesTemplateData*);

signals:
  void strokesCreated(int, COGLPainterDataSharedPtr);

  void painterReady(int, const PainterThreadTask &task);

  //void knifeEdgesCreated(int tmpl_id, StrokesTemplateData*);

};





#endif // CRENDERMANAGER_H
