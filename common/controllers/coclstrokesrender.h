#ifndef CSTROKESRENDER_H
#define CSTROKESRENDER_H

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>

#include <QThread>
#include <QTime>

#include "citems.h"

#include "coglpainterdata.h"

#include "cstrokestemplate.h"

#include "ocl_utils.h"


class QOffscreenSurface;

namespace sm
{

struct KnifeRec
{
  QVector<cl_float2> above_edges;
  QVector<uint> above_indexes;
  QImage edges_map;
  QPointF edges_map_tl;
};

class StrokesTemplateImpl;

class OCLStrokesRenderImpl : public ObjectImpl
{
public:
  void TIME_RESTART()
  {
    _time.restart();
  }

  void TIME_LOG(const QString &txt);

  enum RenderResult
  {
    srerrOk = 0,
    srerrSize = 1,
    srerrCrateTrack = 2,
    srerrApplyEdges = 3,
    srerrLoadKernel = 4
  };

  cl_uint size=0x10000;

  OCLStrokesRenderImpl();

  OCLStrokesRenderImpl(const StrokesTemplateData &tmpl_data);

  ~OCLStrokesRenderImpl();

  bool initContext(QOpenGLContext*);

  int initKernel();

  bool createLinearTrack(
      cl_command_queue _ocl_command_queue,
      cl_mem &mem_out_track,
      cl_mem &mem_out_normals,
      cl_mem &mem_indexes,
      const QList<QLineF> &lines,
      const cl_int steps_on_px,
      const size_t size);


  int timeRendering;

  int size_wg = 256;

  float length(cl_float2 f);


  int intersect(QPointF t_pt1, QPointF t_pt2, QPointF l_pt1, QPointF l_pt2);


  int createCells(clContext *_ogl_context,
                  clCommandQueue *_ocl_command_queue,
                  size_t global_size,
                  size_t local_size,
                  clProgram * _ocl_program,
                  const clMem &mem_track,
                  const clMem &mem_normals,
                  const clMem &mem_indexes,
                  const QVector<cl_float2> &above_edges,
                  const QVector<cl_uint> &above_indexes,
                  QImage in_edges_map,
                  const QPointF &tl,
                  QVector<cl_uint8> &cells);

  int applyCells(clContext *context,
                 clCommandQueue *_ocl_command_queue,
                 size_t local_size,
                 clProgram * _ocl_program,
                 const clMem &mem_track,
                 const clMem &mem_normals,
                 const QVector<cl_uint8> &cells);


  int transformToMem(cl_command_queue _ocl_command_queue, const QTransform &tx, cl_mem &mem_tx);

  QTransform to_scene_tx;
  QList<KnifeRec> knives;
  QRectF rect;
  QOffscreenSurface * _offscrnsf=nullptr;

  cl_platform_id platform_id = NULL;
  cl_device_id device_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;

  CGLContextObj gl_ctx;
  CGLShareGroupObj gl_sharegroup;

  clContext * _ocl_context=nullptr;
  clCommandQueue * _ocl_command_queue=nullptr;
  clProgram * _ocl_program=nullptr;

  QOpenGLContext *_ogl_context=nullptr;

  void _init_painter_data();

  int createStrokeCL(const QTransform &in_to_scene_tx,
                     const QRectF &in_rect,
                     const QList<KnifeRec> &knives);

  int createStrokeCL(const QTransform &in_to_scene_tx,
                     const QRectF &in_rect)
  {
    return createStrokeCL(in_to_scene_tx, in_rect, QList<KnifeRec>());
  }

  int render();
  int _render();

  QTime _time;

  StrokesTemplateData _tmpl_data;

  COGLPainterData * _painter_data;

  QImage _image;

};


class COCLStrokesRender : public CObject
{
public:
  COCLStrokesRender() : CObject() {}
  COCLStrokesRender(const CObject &object) : CObject(object) {}

  COCLStrokesRender(const StrokesTemplateData &tmpl_data) : CObject(new OCLStrokesRenderImpl(tmpl_data)){}
  COCLStrokesRender(bool) : CObject(new OCLStrokesRenderImpl()){}


  inline int createStrokeCL(const QTransform &in_to_scene_tx,
                     const QRectF &in_rect,
                     const QList<KnifeRec> &knifes) const
  {
    IMPL_D(OCLStrokesRender);
    return d->createStrokeCL(in_to_scene_tx, in_rect, knifes);
  }

  inline int createStrokeCL(const QRectF &in_rect) const
  {
    IMPL_D(OCLStrokesRender);
    return d->createStrokeCL(QTransform(), in_rect);
  }

  inline int createStrokeCL(const QTransform &in_to_scene_tx,
                     const QRectF &in_rect) const
  {
    IMPL_D(OCLStrokesRender);
    return d->createStrokeCL(in_to_scene_tx, in_rect);
  }

  inline int render() const
  {
    IMPL_D(OCLStrokesRender);
    return d->render();
  }


  inline bool initContext(QOpenGLContext * ctx) const
  {
    IMPL_D(OCLStrokesRender);
    return d->initContext(ctx);
  }

  inline bool initKernel() const
  {
    IMPL_D(OCLStrokesRender);
    return d->initKernel();
  }


  inline QOpenGLContext * oglContext() const
  {
    IMPL_D(OCLStrokesRender);
    return d->_ogl_context;
  }

  inline COGLPainterDataSharedPtr painterData() const
  {
    IMPL_D(OCLStrokesRender);
    return d->_painter_data;
  }


  inline void setSourceImage(const QImage &image) const
  {
    IMPL_D(OCLStrokesRender);
    d->_image = image;
  }


  inline void setTemplateData(const StrokesTemplateData &tmpl_data) const
  {
    IMPL_D(OCLStrokesRender);
    d->_tmpl_data = tmpl_data;
  }

  inline void setPainterData(COGLPainterData * painter_data) const
  {
    IMPL_D(OCLStrokesRender);
    d->_painter_data = painter_data;
  }

  inline StrokesTemplateData &tmplData() const
  {
    IMPL_D(OCLStrokesRender);
    return d->_tmpl_data;
  }
};


}

#endif // CSTROKESRENDER_H
