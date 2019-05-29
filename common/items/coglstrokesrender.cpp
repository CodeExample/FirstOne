#include "coglstrokesrender.h"

#include "math.h"
#include "vintervals.h"
#include "cmask.h"
#include "cprojecthandler.h"
#include "utils.h"

#include <QFile>
#include <QDebug>
#include <QOffscreenSurface>

#define RENDER_DEBUG_INFO_

namespace sm
{

OGLStrokesRenderImpl::OGLStrokesRenderImpl()
{
}


OGLStrokesRenderImpl::OGLStrokesRenderImpl(const StrokesTemplateData &tmpl_data) : _tmpl_data(tmpl_data)
{
}


OGLStrokesRenderImpl::~OGLStrokesRenderImpl()
{
  if(_offscrnsf)
    delete _offscrnsf;

  if (_ogl_context)
    delete _ogl_context;
}


bool OGLStrokesRenderImpl::initContext(QOpenGLContext *owner_cntx)
{
  _offscrnsf = new QOffscreenSurface();
  _offscrnsf->create();
  if (!_offscrnsf->isValid()) {
    qDebug() << "Surface Failed";
    return false;
  }

  _ogl_context = new QOpenGLContext();
  _ogl_context->setShareContext(owner_cntx);
  _ogl_context->create();
  if (!_ogl_context->isValid()) {
    qDebug() << "Context Failed";
    return false;
  }
  if (!_ogl_context->areSharing(owner_cntx, _ogl_context)) {
    qDebug() << "Sharing Failed";
    return false;
  }
  return true;
}


void fillRequaredLines(const QRectF &rect, double angle, double interval, double line_pos,
                       QList<QLineF> &lines, double &length)
{
  double cos_a = cos(angle + M_PI_2);
  double sin_a = sin(angle + M_PI_2);

  bool inv_x = false;
  bool inv_y = false;
  if (cos_a < 0)
  {
    cos_a = fabs(cos_a);
    inv_x = true;
  }

  if (sin_a < 0)
  {
    sin_a = fabs(sin_a);
    inv_y = true;
  }

  if (fabs(cos_a) < 1e-5)
  {
    cos_a = 0;
    sin_a = sin_a > 0 ? 1 : -1;
  } else
  if (fabs(sin_a) < 1e-5)
  {
    cos_a = cos_a > 0 ? 1 : -1;
    sin_a = 0;
  }

  double w = rect.width();
  double h = rect.height();

  if (sin_a == 0)
  {
    double x0=interval*line_pos;
    while(x0 < w)
    {
      QLineF l(x0, 0, x0, h);
      lines << l;
      length += l.length();
      x0 += interval;
    }
  } else
  if (cos_a == 0)
  {
    double y0=interval*line_pos;
    while(y0 < h)
    {
      QLineF l(0, y0, w, y0);
      lines << l;
      length += l.length();
      y0 += interval;
    }

  } else {
    double dy = interval/sin_a;
    double dx = interval/cos_a;
    double dy_dx = dy/dx;
    double x0=dx*line_pos;
    while(fabs(x0) < w)
    {
      double lx = w - x0;
      double ly = lx*dy_dx;

      if (ly > h)
      {
        lx *= h/ly;
        ly = h;
      }

      QLineF l(inv_x ? w-x0 : x0,
               inv_y ? h : 0,
               inv_x ? w-(x0+lx) : x0+lx,
               inv_y ? h-ly : ly);

      lines.push_back(l);
      length += l.length();
      x0 += dx;
    }

    double dx_dy = dx/dy;

    double y0 = line_pos == 0 ? dy : dy*(1-line_pos);
    while(fabs(y0) < h)
    {
      double ly = h - y0;
      double lx = ly*dx_dy;

      if (lx > w)
      {
        ly *= w/lx;
        lx = w;
      }

      QLineF l(inv_x ? w : 0,
               inv_y ? h-y0 : y0,
               inv_x ? w-lx : lx,
               inv_y ? h-(y0+ly) : y0+ly);
      lines.push_front(l);
      length += l.length();
      y0 += dy;
    }
  }

}

bool OGLStrokesRenderImpl::createLinearTrack(
    cl_command_queue command_queue,
    cl_mem &mem_out_track,
    cl_mem &mem_out_normals,
    cl_mem &mem_indexes,
    const QList<QLineF> &lines,
    const cl_int steps_on_px,
    const size_t size)
{
  QTime t; t.restart();
  QVector<cl_float2> out_track(size);
  QVector<cl_float2> out_normals(size);
  QVector<uint> indexes(size);

  uint global_id=0;
  for(int line_ix=0; line_ix < lines.count(); ++line_ix)
  {
    const QLineF &line = lines.at(line_ix);

    double l=line.length()*steps_on_px + 1;
    int n=round(l);

    double d = 1./double(n);
    for(int i=0; i <= n+1; ++i)
    {
      double t=d*i;
      if (i <= n)
      {
        QPointF p = line.pointAt(t);
        float x = p.x();
        float y = p.y();
        out_track[global_id] = cl_float2{{x, y}};
        out_normals[global_id] = cl_float2{{1, 0}};
        indexes[global_id] = line_ix;
      } else {
        out_track[global_id] = cl_float2{{0, 0}};
        out_normals[global_id] = cl_float2{{0,0}};
        indexes[global_id] = 0xffffffff;
      }
      if (++global_id >= size)
        break;
    }
    if (global_id >= size)
      break;
  }
  QTransform tx;
  int h = size/width;
  tx.translate(width/2, h/2);
  tx.rotate(angle);
  tx.translate(-width/2, -h/2);
  QPolygonF plgn;
  for(uint global_id=0; global_id < size; ++global_id)
  {
    uint horz_cnt = width*steps_on_px + 1;

    uint row = global_id/horz_cnt;
    uint col = global_id%horz_cnt;

    float dx = 1/float(steps_on_px);
    float x = col*dx;
    float y = row*interval_px + line_position_px + sin(x/20);
    if (angle != 0)
    {
      QPointF p = tx.map(QPointF(x, y));
      x = p.x();
      y = p.y();
    }
    out_track[global_id] = cl_float2{{x, y}};
    if (global_id%horz_cnt < horz_cnt-1)
    {
      out_normals[global_id] = cl_float2{{1, 0}};
      indexes[global_id] = global_id/horz_cnt;
    } else {
      out_normals[global_id] = cl_float2{{0,0}};
      indexes[global_id] = 0xffffffff;
    }
    plgn << QPointF(x, y);
  }
  brect = plgn.boundingRect();

  {
    clEvent event;
    cl_int ret = clEnqueueWriteBuffer(command_queue, mem_out_track, CL_FALSE, 0,
                                      out_track.count()*sizeof(cl_float2), out_track.data(), 0, NULL, &event.event);

    LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer edges mem_above_edges...");

    clWaitForEvents(1, &event.event);
  }
  {
    clEvent event;
    cl_int ret = clEnqueueWriteBuffer(command_queue, mem_out_normals, CL_FALSE, 0,
                                      out_normals.count()*sizeof(cl_float2), out_normals.data(), 0, NULL, &event.event);

    LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer edges mem_above_edges...");

    clWaitForEvents(1, &event.event);
  }
  {
    clEvent event;
    cl_int ret = clEnqueueWriteBuffer(command_queue, mem_indexes, CL_FALSE, 0,
                                      indexes.count()*sizeof(cl_uint), indexes.data(), 0, NULL, &event.event);

    LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer edges mem_above_edges...");

    clWaitForEvents(1, &event.event);
  }

  return CL_SUCCESS;
}



float OCLStrokesRenderImpl::length(const QPointF &f)
{
  return hypot(f.x, f.y);
}


int OCLStrokesRenderImpl::intersect(QPointF t_pt1, QPointF t_pt2, QPointF l_pt1, QPointF l_pt2)
{
  const QPointF a = t_pt2 - t_pt1;
  const QPointF b = l_pt1 - l_pt2;
  const QPointF c = t_pt1 - l_pt1;

  const float denominator = a.y() * b.x() - a.x() * b.y();
  if (denominator == 0 || !isfinite(denominator))
    return NoIntersection;

  const float reciprocal = 1 / denominator;
  const float na = (b.y() * c.x() - b.x() * c.y()) * reciprocal;

  if (na < 0 || na > 1)
    return UnboundedIntersection;

  const float nb = (a.x() * c.y() - a.y() * c.x()) * reciprocal;
  if (nb < 0 || nb > 1)
    return UnboundedIntersection;

  return BoundedIntersection;
}

cl_float2 mapTx(QTransform tx, cl_float2 p)
{
  QPointF g = tx.map(QPointF(p.x, p.y));
  cl_float x = g.x();
  cl_float y = g.y();
  return cl_float2{{x,y}};
}

QPointF clToQ(cl_float2 p)
{
  return QPointF(p.x, p.y);
}


void applyEdgesToStrokes(const QVector<cl_float2> &smooth_track,
                                  QVector<cl_float2> &smooth_normals,
                                  QVector<cl_uint> &indexes,
                                  const QVector<cl_float2> &above_edges,
                                  const QVector<cl_uint> &edges_indexes,
                                  const cl_uint edges_size,
                                  const QImage &edges_map,
                                  const QTransform &scene_to_edges_tx,
                                  const QTransform &edges_to_scene_tx,
                                  const QTransform &track_to_scene_tx,
                                  const cl_float2 tl,
                                  const uint global_size)

{
  for(uint track_ix = 0; track_ix < global_size; ++track_ix)
  {

  if (track_ix == 0)
    continue;

  cl_float2 norm0 = smooth_normals[track_ix];
  cl_float2 norm1 = smooth_normals[track_ix-1];
  if (norm0.y == 0 || norm1.y == 0)
    continue;

  cl_float2 t0 = smooth_track[track_ix-1];
  cl_float2 t1 = smooth_track[track_ix];

  uint px0 = 0xffffffff;
  uint px1 = 0xffffffff;

  cl_float2 s0_xy = mapTx(track_to_scene_tx, t0);
  cl_float2 e0_xy = mapTx(scene_to_edges_tx, s0_xy);
  e0_xy.x += 5;
  e0_xy.y += 5;


  if (!edges_map.rect().contains(clToQ(e0_xy).toPoint() - clToQ(tl).toPoint()))
    continue;

  px0 = edges_map.pixel(clToQ(e0_xy).toPoint() - clToQ(tl).toPoint());

  cl_float2 s1_xy = mapTx(track_to_scene_tx, t1);
  cl_float2 e1_xy = mapTx(scene_to_edges_tx, s1_xy);
  e1_xy.x += 5;
  e1_xy.y += 5;
  if (!edges_map.rect().contains(clToQ(e1_xy).toPoint() - clToQ(tl).toPoint()))
    continue;

  px1 = edges_map.pixel(clToQ(e1_xy).toPoint() - clToQ(tl).toPoint());

 if (px0 == 0xffffffff && px1 == 0xffffffff)
    continue;

  uint px = px0 < px1 ? px0 : px1;
  uint s=15;
  uint edge_ix0 = px < s+1 ? 1 : px-s;
  uint edge_ix1 = px > edges_size-1-s ? edges_size-1 : px+s;

  for(uint edge_ix = edge_ix0; edge_ix <= edge_ix1; ++edge_ix)
  {
    cl_float2 e0_xy=mapTx(edges_to_scene_tx, cl_float2{{above_edges[edge_ix-1].x, above_edges[edge_ix-1].y}});
    cl_float2 e1_xy=mapTx(edges_to_scene_tx, cl_float2{{above_edges[edge_ix].x, above_edges[edge_ix].y}});

    if (!intersect(s0_xy, s1_xy, e0_xy, e1_xy))
      continue;

    uint edge_row_ix = 1 + edges_indexes[edge_ix];
    uint track_row_ix = 1 + indexes[track_ix];
    smooth_normals[track_ix].x = -(float)(edge_row_ix);
    smooth_normals[track_ix].y = -(float)(track_row_ix);
    break;
  }
  }
}


int OCLStrokesRenderImpl::createCells(clContext *context,
                                   clCommandQueue *command_queue,
                                   size_t global_size,
                                   size_t local_size,
                                   clProgram * program,
                                   const clMem &mem_track,
                                   const clMem &mem_normals,
                                   const clMem &mem_indexes,
                                   const QVector<cl_float2> &above_edges,
                                   const QVector<cl_uint> &above_indexes,
                                   QImage in_edges_map,
                                   const QPointF &tl,
                                   QVector<cl_uint8> &cells)
{
  if (above_edges.isEmpty())
    return CL_SUCCESS;

  cl_int ret;

  QTime t; t.start();
  {
    clMem mem_above_edges(clCreateBuffer(context->context, CL_MEM_READ_ONLY,
                                         above_edges.count()*sizeof(cl_float2), NULL, &ret));

    LOG_OCL_ERROR(ret, "Creating mem_above_edges failed");
    {
      clEvent event;
      cl_int ret = clEnqueueWriteBuffer(command_queue->queue, mem_above_edges.mem, CL_FALSE, 0,
                                        above_edges.count()*sizeof(cl_float2), above_edges.data(), 0, NULL, &event.event);

      LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer edges mem_above_edges...");

      clWaitForEvents(1, &event.event);
    }

    clMem mem_above_indexes(clCreateBuffer(context->context, CL_MEM_READ_ONLY,
                                           above_indexes.count()*sizeof(cl_uint), NULL, &ret));

    LOG_OCL_ERROR(ret, "Creating mem_above_indexes failed");
    {
      clEvent event;
      cl_int ret = clEnqueueWriteBuffer(command_queue->queue, mem_above_indexes.mem, CL_FALSE, 0,
                                        above_indexes.count()*sizeof(cl_uint), above_indexes.data(), 0, NULL, &event.event);

      LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer edges mem_above_edges...");

      clWaitForEvents(1, &event.event);
    }
    cl_image_format image_format;
    image_format.image_channel_data_type = CL_UNSIGNED_INT8;
    image_format.image_channel_order = CL_RGBA;

    clMem mem_edges_map(clCreateImage2D(
                          context->context,
                          CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                          &image_format,
                          in_edges_map.width(), in_edges_map.height(), in_edges_map.width()*4,
                          in_edges_map.scanLine(0),
                          &ret));
    LOG_OCL_ERROR(ret, "clCreateImage Failed");


    cl_uint edges_size = above_edges.size();
    {
      clKernel kernel(clCreateKernel(program->program, "applyEdgesToStrokes", &ret));
      cl_float tl_x  = tl.x();
      cl_float tl_y  = tl.y();
      cl_float2 cl_tl = cl_float2{{tl_x, tl_y}};
      clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_track.mem);
      clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_normals.mem);
      clSetKernelArg(kernel.kernel, 2, sizeof(cl_mem), &mem_indexes.mem);
      clSetKernelArg(kernel.kernel, 3, sizeof(cl_mem), &mem_above_edges.mem);
      clSetKernelArg(kernel.kernel, 4, sizeof(cl_mem), &mem_above_indexes.mem);
      clSetKernelArg(kernel.kernel, 5, sizeof(cl_uint), &edges_size);
      clSetKernelArg(kernel.kernel, 6, sizeof(cl_mem), &mem_edges_map.mem);
      clSetKernelArg(kernel.kernel, 7, sizeof(cl_float2), &cl_tl);

      clEvent event;
      ret=clEnqueueNDRangeKernel(command_queue->queue,
                                 kernel.kernel,
                                 1,
                                 NULL,
                                 &global_size,
                                 &local_size,
                                 0,
                                 NULL,
                                 &event.event);
      LOG_OCL_ERROR(ret, "clEnqueueNDRangeKernel 0xff11 Failed");
      clWaitForEvents(1, &event.event);
      TIME_LOG("applyEdgesToStrokes");
    }


    {
      QVector<cl_float2> smooth_normals(global_size);
      {
        clEvent event;
        ret=clEnqueueReadBuffer(command_queue->queue,
                                mem_normals.mem,
                                CL_TRUE,
                                0,
                                global_size*sizeof(cl_float2),
                                smooth_normals.data(),
                                0,
                                NULL,
                                &event.event);
        LOG_OCL_ERROR(ret, "clEnqueueNDRangeKernel smooth_normals Failed");
        clWaitForEvents(1, &event.event);
      }


      QVector<cl_float2> smooth_track(global_size);
      {
        clEvent event;
        ret=clEnqueueReadBuffer(command_queue->queue,
                                mem_track.mem,
                                CL_TRUE,
                                0,
                                global_size*sizeof(cl_float2),
                                smooth_track.data(),
                                0,
                                NULL,
                                &event.event);
        LOG_OCL_ERROR(ret, "clEnqueueNDRangeKernel smooth_track Failed");
        clWaitForEvents(1, &event.event);
      }
      QMap<quint64, cl_uint8> cells_map;
      for(int track_ix=0; track_ix < smooth_normals.count(); ++track_ix)
      {
        if (smooth_normals.at(track_ix).x < 0)
        {
          uint edge_row = -smooth_normals.at(track_ix).x - 1;
          uint track_row = -smooth_normals.at(track_ix).y - 1;

          quint64 cell0_id = track_row*0x1000000 + edge_row;
          quint64 cell1_id = track_row*0x1000000 +(edge_row+1);
          quint64 cell2_id = (track_row+1)*0x1000000 + edge_row;
          quint64 cell3_id = (track_row+1)*0x1000000 +(edge_row+1);
          quint64 cell4_id = (track_row-1)*0x1000000 + edge_row;
          quint64 cell5_id = (track_row-1)*0x1000000 +(edge_row+1);

          uint code_track_ix = 1+track_ix;
          cells_map[cell0_id].s0 = code_track_ix;
          cells_map[cell1_id].s1 = code_track_ix;
          cells_map[cell2_id].s2 = code_track_ix;
          cells_map[cell3_id].s3 = code_track_ix;
          cells_map[cell4_id].s4 = code_track_ix;
          cells_map[cell5_id].s5 = code_track_ix;
        }
      }

      cells = cells_map.values().toVector();
#ifdef RENDER_DEBUG_INFO
      qDebug() << " intersects.count()" << cells.count();
#endif
      TIME_LOG("intersects");
    }


    {
      clKernel kernel(clCreateKernel(program->program, "clearCells", &ret));
      clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_normals.mem);

      clEvent event;
      ret=clEnqueueNDRangeKernel(command_queue->queue,
                                 kernel.kernel,
                                 1,
                                 NULL,
                                 &global_size,
                                 &local_size,
                                 0,
                                 NULL,
                                 &event.event);
      LOG_OCL_ERROR(ret, "clEnqueueNDRangeKernel clearCells Failed");
      clWaitForEvents(1, &event.event);
    }
  }

  return CL_SUCCESS;
}


int OCLStrokesRenderImpl::applyCells(clContext *context,
                                   clCommandQueue *command_queue,
                                   size_t local_size,
                                   clProgram * program,
                                   const clMem &mem_track,
                                   const clMem &mem_normals,
                                   const QVector<cl_uint8> &cells)
{

  cl_int ret;

  QTime t; t.start();

  if (cells.count() > 0)
  {
    clMem mem_cells_map(clCreateBuffer(context->context, CL_MEM_READ_ONLY,
                                       cells.count()*sizeof(cl_uint8), NULL, &ret));
    {
      clEvent event;
      cl_int ret = clEnqueueWriteBuffer(command_queue->queue, mem_cells_map.mem, CL_FALSE, 0,
                                        cells.count()*sizeof(cl_uint8), cells.data(), 0, NULL, &event.event);

      LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer mem_cells_map...");

      clWaitForEvents(1, &event.event);
    }

    uint cells_size = cells.size();
    size_t global_cells_size = size_wg*int((cells_size + size_wg-1)/size_wg);
    {
      clKernel kernel(clCreateKernel(program->program, "applyCellsToStrokes", &ret));
      clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_cells_map.mem);
      clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_track.mem);
      clSetKernelArg(kernel.kernel, 2, sizeof(cl_mem), &mem_normals.mem);
      clSetKernelArg(kernel.kernel, 3, sizeof(uint), &cells_size);

      clEvent event;
      ret = clEnqueueNDRangeKernel(command_queue->queue,
                                   kernel.kernel,
                                   1,
                                   NULL,
                                   &global_cells_size,
                                   &local_size,
                                   0,
                                   NULL,
                                   &event.event);
      LOG_OCL_ERROR(ret, "clEnqueueNDRangeKernel 0xff15 Failed");
      clWaitForEvents(1, &event.event);
    }
  }
  return CL_SUCCESS;
}



int OCLStrokesRenderImpl::transformToMem(cl_command_queue command_queue, const QTransform &tx, cl_mem &mem_tx)
{
  cl_float cl_float_tx[6];

  cl_float_tx[0] = tx.m11();
  cl_float_tx[1] = tx.m21();
  cl_float_tx[2] = tx.dx();
  cl_float_tx[3] = tx.m12();
  cl_float_tx[4] = tx.m22();
  cl_float_tx[5] = tx.dy();
  clEvent event;
  int ret = clEnqueueWriteBuffer(command_queue, mem_tx, CL_FALSE, 0,
                                 6*sizeof(cl_float), &cl_float_tx, 0, NULL, &event.event);
  LOG_OCL_ERROR(ret, "transformToMem Failed");

  clWaitForEvents(1, &event.event);

  return CL_SUCCESS;
}

*/

int OGLStrokesRenderImpl::createStrokeCL(
    const QTransform &in_to_scene_tx,
    const QRectF &in_rect)
{
  to_scene_tx = in_to_scene_tx;
  rect = in_rect;

  return 0;
}



int OCLStrokesRenderImpl::initKernel()
{
  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  LOG_OCL_ERROR(ret, "clGetPlatformIDs Failed");

  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1,
                       &device_id, &ret_num_devices);
  LOG_OCL_ERROR(ret, "clGetDeviceIDs Failed");

  // Create an OpenCL context
  gl_ctx        = CGLGetCurrentContext();
  gl_sharegroup = CGLGetShareGroup(gl_ctx);

  cl_context_properties properties[] = {
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties) gl_sharegroup, 0};

  _ocl_context = new clContext(clCreateContext(properties,
                                               1,
                                               &device_id,
                                               [](const char* errinfo, const void* private_info, size_t cb, void* user_data) ->
                                               void {printf("Context error: ");
    printf(errinfo);
  }, nullptr, &ret));

  // Create a command queue
  _ocl_command_queue = new clCommandQueue(clCreateCommandQueue(_ocl_context->context, device_id, 0, &ret));

  QFile fileKernel(":/common/cl/scan2d.cl");
  if (!fileKernel.open(QIODevice::ReadOnly))
  {
    qDebug() << "Failed to load kernel.\n";
    return srerrLoadKernel;
  }

  QByteArray kernelSource_str = fileKernel.readAll();
  char * kernelSource_ptr = kernelSource_str.data();

  size_t source_size = kernelSource_str.length();
  fileKernel.close();

  _ocl_program = new clProgram(clCreateProgramWithSource(_ocl_context->context, 1, (const char **)&kernelSource_ptr,
                                                    (const size_t *)&source_size, &ret));
  ret = clBuildProgram(_ocl_program->program, 1, &device_id, NULL, NULL, NULL);
  if (ret != CL_SUCCESS)
  {
    char buffer[0x100000];
    clGetProgramBuildInfo(_ocl_program->program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
    qDebug() << "CL Compilation failed:\n" << buffer;
    _ogl_context->doneCurrent();
    LOG_OCL_ERROR(ret, "clBuildProgram Failed");
  }
  return CL_SUCCESS;
}


void OGLStrokesRenderImpl::_init_painter_data()
{
  _painter_data->stroke_first.clear();
  _painter_data->stroke_count.clear();
  _painter_data->round_cups_first.clear();
  _painter_data->round_cups_count.clear();

  _painter_data->cells_first.clear();
  _painter_data->cells_count.clear();
  _painter_data->round_cells_roundcups_first.clear();
  _painter_data->round_cells_roundcups_count.clear();

  _painter_data->centers_first.clear();
  _painter_data->centers_count.clear();
  _painter_data->round_centers_roundcups_first.clear();
  _painter_data->round_centers_roundcups_count.clear();
}


int OGLStrokesRenderImpl::render()
{
  _ogl_context->makeCurrent(_offscrnsf);

  int ret = _render();

  _ogl_context->doneCurrent();

  return ret;
}

int OCLStrokesRenderImpl::_render()
{
  cl_int ret=0;

  TIME_RESTART();

  cl_float interval_px = _tmpl_data.interval;
  cl_float line_position = _tmpl_data.line_position;
  cl_float angle = _tmpl_data.angle;
  cl_int steps_on_px = _tmpl_data.steps_on_px;

  QList<QLineF> lines;
  double lines_l;

  if (!_tmpl_data.mask_path.isEmpty())
  {
    rect = _tmpl_data.mask_transform.mapRect(_tmpl_data.mask_boundingRect);
  }

  if (rect.width() < 2)
    rect.setWidth(2);
  if (rect.height() < 2)
    rect.setHeight(2);

  fillRequaredLines(rect, M_PI*angle/180., interval_px, line_position, lines, lines_l);

  for(QLineF &l : lines)
  {
    l.translate(rect.left(), rect.top());
  }

  StrokesTemplateData * tmpl_data = &_tmpl_data;

  tmpl_data->bounding_rect = rect;

  size = lines_l*steps_on_px + lines.count()*2 + 1;
  size = size_wg*int((size + size_wg-1)/size_wg);

  cl_uint vbo_buff_stroke_size=3*size*2;

  _init_painter_data();

  size_t global_size = size;
  size_t local_size = size_wg;

  //
  QTime debug_time; debug_time.restart();

  // Number of work-groups
  if (size % local_size) {
    fprintf(stderr,"(Size of Array) not divisible by (Size of WorkGroup)\n");
    return srerrSize;
  }

  clMem mem_in_track(clCreateBuffer(_ocl_context->context, CL_MEM_READ_WRITE,
                                    size*sizeof(cl_float2), NULL, &ret));
  LOG_OCL_ERROR(ret, "Creating mem_in_track failed");

  clMem mem_stroke_track(clCreateBuffer(_ocl_context->context, CL_MEM_READ_WRITE,
                                        size*sizeof(cl_float2), NULL, &ret));
  LOG_OCL_ERROR(ret, "Creating mem_out_track failed");

  clMem mem_indexes(clCreateBuffer(_ocl_context->context, CL_MEM_READ_WRITE,
                                   size*sizeof(cl_uint), NULL, &ret));
  LOG_OCL_ERROR(ret, "Creating mem_indexes failed");

  clMem mem_stroke_normals(clCreateBuffer(_ocl_context->context, CL_MEM_READ_WRITE,
                                          size*sizeof(cl_float2), NULL, &ret));
  LOG_OCL_ERROR(ret, "Creating mem_stroke_normals failed");

  clMem mem_smooth_track(clCreateBuffer(_ocl_context->context, CL_MEM_READ_WRITE,
                                        size*sizeof(cl_float2), NULL, &ret));
  LOG_OCL_ERROR(ret, "Creating mem_smooth_track failed");

  clMem mem_smooth_normals(clCreateBuffer(_ocl_context->context, CL_MEM_READ_WRITE,
                                          size*sizeof(cl_float2), NULL, &ret));
  LOG_OCL_ERROR(ret, "Creating mem_smooth_normals failed");

  clMem mem_transform(clCreateBuffer(_ocl_context->context, CL_MEM_READ_ONLY,
                                     6*sizeof(cl_float), NULL, &ret));
  LOG_OCL_ERROR(ret, "Creating mem_transform failed");

  // transform
  transformToMem(_ocl_command_queue->queue, to_scene_tx, mem_transform.mem);


  //
  // create source strokes
  //
  ret = createLinearTrack(_ocl_command_queue->queue,
                         mem_in_track.mem,
                         mem_stroke_normals.mem,
                         mem_indexes.mem,
                         lines, steps_on_px, size);
  LOG_OCL_ERROR(ret, "createLinearTrack failed");

  //
  // createStroke2d
  //

  // correct data for filled transform

  {
    QImage src_image = _image;
    size_t src_w = _image.width();
    size_t src_h = _image.height();

    if (!_tmpl_data.mask_path.isEmpty())
    {
      QImage masked_image = src_image;
      masked_image.fill(0xffffffff);

      QPainterPath clip_path = _tmpl_data.mask_transform.map(_tmpl_data.mask_path);

      QPainter painter(&masked_image);
      painter.setClipPath(clip_path);
      painter.drawImage(0,0, src_image);
      src_image = masked_image;
    }

    void * img_buf = src_image.scanLine(0);

    cl_image_format image_format;
    image_format.image_channel_data_type = CL_UNSIGNED_INT8;
    image_format.image_channel_order = CL_RGBA;

    clMem mem_image(clCreateImage2D(
                      _ocl_context->context,
                      CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                      &image_format,
                      src_w, src_h, src_w*4,
                      img_buf,
                      &ret));
    LOG_OCL_ERROR(ret, "clCreateImage Failed");
    //
    clKernel kernel(clCreateKernel(_ocl_program->program, "createStroke2d", &ret));

    clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_in_track.mem);
    clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_stroke_track.mem);
    clSetKernelArg(kernel.kernel, 2, sizeof(cl_mem), &mem_stroke_normals.mem);
    clSetKernelArg(kernel.kernel, 3, sizeof(cl_mem), &mem_image.mem);
    clSetKernelArg(kernel.kernel, 4, sizeof(cl_mem), &mem_transform.mem);
    clSetKernelArg(kernel.kernel, 5, sizeof(cl_uint), &size);
    clSetKernelArg(kernel.kernel, 6, sizeof(cl_float), &tmpl_data->thickness);
    clSetKernelArg(kernel.kernel, 7, sizeof(cl_float), &tmpl_data->basrelief_rise);
    clSetKernelArg(kernel.kernel, 8, sizeof(cl_float), &tmpl_data->threshold);
    clSetKernelArg(kernel.kernel, 9, sizeof(cl_float), &tmpl_data->thickness_base);

    clEvent event;
    ret=clEnqueueNDRangeKernel(_ocl_command_queue->queue,
                               kernel.kernel,
                               1,
                               NULL,
                               &global_size,
                               &local_size,
                               0,
                               NULL,
                               &event.event);
    LOG_OCL_ERROR(ret, "createStroke2d failed");

    clWaitForEvents(1, &event.event);
  }
  //
  // breakBasrelief
  //
  {
    clKernel kernel(clCreateKernel(_ocl_program->program, "breakBasrelief", &ret));
    clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_stroke_normals.mem);
    clSetKernelArg(kernel.kernel, 1, sizeof(cl_uint), &size);
    clEvent event;
    ret=clEnqueueNDRangeKernel(_ocl_command_queue->queue,
                               kernel.kernel,
                               1,
                               NULL,
                               &global_size,
                               &local_size,
                               0,
                               NULL,
                               &event.event);
    LOG_OCL_ERROR(ret, "createStroke2d failed");
    clWaitForEvents(1, &event.event);
  }
  //
  // apply mask
  //
  QImage mask_image;
  if (!tmpl_data->_masks.isEmpty())
  {
    clMem mem_mask_transform(clCreateBuffer(_ocl_context->context, CL_MEM_READ_ONLY,
                                       6*sizeof(cl_float), NULL, &ret));
    LOG_OCL_ERROR(ret, "Creating mem_mask_transform failed");

    CMask mask = hprj->mask(tmpl_data->_masks.first());

    QTransform mask_tx;
    mask_tx.translate(-mask.boundingRect().left(), -mask.boundingRect().top());
    transformToMem(command_queue->queue, mask_tx, mem_mask_transform.mem);

    cl_image_format image_format;
    image_format.image_channel_data_type = CL_UNSIGNED_INT8;
    image_format.image_channel_order = CL_RGBA;
    mask_image = mask.image();
    clMem mem_masks(clCreateImage2D(
                      _ocl_context->context,
                      CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                      &image_format,
                      mask_image.width(), mask_image.height(), mask_image.width()*4,
                      mask_image.scanLine(0),
                      &ret));
    LOG_OCL_ERROR(ret, "clCreateImage Failed");


    clKernel kernel(clCreateKernel(program->program, "applyMask", &ret));

    clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_stroke_track.mem);
    clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_stroke_normals.mem);
    clSetKernelArg(kernel.kernel, 2, sizeof(cl_mem), &mem_masks.mem);
    clSetKernelArg(kernel.kernel, 3, sizeof(cl_mem), &mem_mask_transform.mem);
    clSetKernelArg(kernel.kernel, 4, sizeof(cl_int), &steps_on_px);
    clSetKernelArg(kernel.kernel, 5, sizeof(cl_uint), &size);

    clEvent event;
    ret=clEnqueueNDRangeKernel(command_queue->queue,
                               kernel.kernel,
                               1,
                               NULL,
                               &global_size,
                               &local_size,
                               0,
                               NULL,
                               &event.event);
    LOG_OCL_ERROR(ret, "applyMasks failed");
    clWaitForEvents(1, &event.event);
  }

  //
  // smoothStroke
  //
  {
    clKernel kernel(clCreateKernel(_ocl_program->program, "smoothStroke", &ret));

    clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_stroke_track.mem);
    clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_stroke_normals.mem);
    clSetKernelArg(kernel.kernel, 2, sizeof(cl_mem), &mem_smooth_track.mem);
    clSetKernelArg(kernel.kernel, 3, sizeof(cl_mem), &mem_smooth_normals.mem);
    clSetKernelArg(kernel.kernel, 4, sizeof(cl_uint), &size);
    clSetKernelArg(kernel.kernel, 5, sizeof(cl_uint), &tmpl_data->smooth);

    clEvent event;
    ret=clEnqueueNDRangeKernel(_ocl_command_queue->queue,
                               kernel.kernel,
                               1,
                               NULL,
                               &global_size,
                               &local_size,
                               0,
                               NULL,
                               &event.event);
    LOG_OCL_ERROR(ret, "smoothStroke failed");
    clWaitForEvents(1, &event.event);
  }

  //
  // Fetch intervals
  //
  vIntervals<cl_uint> i_dotted;
  QVector<cl_float2> smooth_normals(size);
  {
    {
      clEvent event;
      ret=clEnqueueReadBuffer(_ocl_command_queue->queue,
                              mem_smooth_normals.mem,
                              CL_TRUE,
                              0,
                              size*sizeof(cl_float2),
                              smooth_normals.data(),
                              0,
                              NULL,
                              &event.event);
      LOG_OCL_ERROR(ret, "fetchIntervals failed");
      clWaitForEvents(1, &event.event);
    }

    for(uint norm_i=0; norm_i < global_size; ++norm_i)
    {
      float d = length(smooth_normals.at(norm_i));
      if (d > tmpl_data->dotline_threshold)
        continue;

      int norm_i0 = norm_i;
      while(++norm_i < global_size)
      {
        float f = length(smooth_normals.at(norm_i));
        if (f > tmpl_data->dotline_threshold)
          break;
      }
      i_dotted += vInterval<cl_uint>(norm_i0, norm_i);
    }
#ifdef RENDER_DEBUG_INFO
    qDebug() << "    fetch intervals:" << debug_time.elapsed(); debug_time.restart();
#endif
  }

  //
  // Edges generate
  //
  if (tmpl_data->is_knife)
  {
    QRectF brect = tmpl_data->bounding_rect;
    QPointF tl = brect.topLeft();

    QVector<cl_float2> smooth_track(size);
    {
      clEvent event;
      ret=clEnqueueReadBuffer(_ocl_command_queue->queue,
                              mem_smooth_track.mem,
                              CL_TRUE,
                              0,
                              size*sizeof(cl_float2),
                              smooth_track.data(),
                              0,
                              NULL,
                              &event.event);
      LOG_OCL_ERROR(ret, "edgesGenerate failed");
      clWaitForEvents(1, &event.event);
    }



    QVector<cl_uint> track_indexes(size);
    {
      clEvent event;
      ret=clEnqueueReadBuffer(_ocl_command_queue->queue,
                              mem_indexes.mem,
                              CL_TRUE,
                              0,
                              size*sizeof(cl_uint),
                              track_indexes.data(),
                              0,
                              NULL,
                              &event.event);
      LOG_OCL_ERROR(ret, "trackIndexes failed");
      clWaitForEvents(1, &event.event);
    }

    QTime t; t.restart();
    tmpl_data->edges_img = QImage(brect.width() + 10,
                                  brect.height() + 10, QImage::Format_ARGB32);

    tmpl_data->edges_img.fill(0xffffffff);

    for(int i=0; i < smooth_normals.count(); ++i)
    {
      if (smooth_normals[i].y == 0)
        continue;

      cl_float2 cl_p0 = smooth_track[i];

      QPoint p0(round(cl_p0.x - tl.x()) + 5,
                round(cl_p0.y - tl.y()) + 5);

      if (tmpl_data->edges_img.pixel(p0) == 0xffffffff)
        tmpl_data->edges_img.setPixel(p0, i);

      if (p0.x() > 0 && p0.y() > 0 &&
          tmpl_data->edges_img.pixel(p0-QPoint(1,1)) == 0xffffffff)
        tmpl_data->edges_img.setPixel(p0-QPoint(1,1), i);

      if (p0.y() > 0 && tmpl_data->edges_img.pixel(p0-QPoint(0,1)) == 0xffffffff)
        tmpl_data->edges_img.setPixel(p0-QPoint(0,1), i);

      if (p0.x() > 0 && tmpl_data->edges_img.pixel(p0-QPoint(1,0)) == 0xffffffff)
        tmpl_data->edges_img.setPixel(p0-QPoint(1,0), i);
    }

    tmpl_data->edges_track = smooth_track;
    tmpl_data->edges_indexes = track_indexes;

#ifdef RENDER_DEBUG_INFO
    qDebug() << "    create edges: " << t.elapsed();
#endif
  }

  //
  // Apply edges
  //
  vIntervals<uint> i_edged;
  if (tmpl_data->is_edged && knives.count() > 0)
  {
    QList<QVector<cl_uint8> > cells_list;
    for(const KnifeRec &kr : knives)
    {
      QVector<cl_uint8> cells;
      if (createCells(_ocl_context, _ocl_command_queue,
                      global_size, local_size,
                      _ocl_program,
                      mem_smooth_track, mem_smooth_normals, mem_indexes,
                      kr.above_edges, kr.above_indexes, kr.edges_map,
                      kr.edges_map_tl, cells) != CL_SUCCESS)
        return srerrApplyEdges;
      cells_list << cells;
    }

    if (tmpl_data->is_cell)
    {
      for(QVector<cl_uint8> cells : cells_list)
      {
        if (applyCells(_ocl_context, _ocl_command_queue,
                       local_size,
                       _ocl_program,
                       mem_smooth_track, mem_smooth_normals,
                       cells) != CL_SUCCESS)
          return srerrApplyEdges;
      }
    }

    for(QVector<cl_uint8> cells : cells_list)
    {
      for(cl_uint8 cell : cells)
      {
        uint i0 = cell.s0;
        uint i1 = cell.s1;
        if (i0 && i1)
        {
          i_edged += vInterval<uint>(i0, i1);
        }
      }
    }
  }

  //
  // Dot lines
  //
  if (!i_edged.isEmpty())
  {
    QVector<cl_uint> intervals = i_edged.pairs;
    cl_int interval_i = intervals.count();
    size_t intrvals_size_t=local_size*int((interval_i + local_size-1)/local_size);
    clMem mem_in_intervals(clCreateBuffer(_ocl_context->context, CL_MEM_READ_ONLY,
                                          intrvals_size_t*sizeof(cl_uint), NULL, &ret));
    int intrvals_size = intrvals_size_t;
    while(intervals.size() < intrvals_size)
    {
      intervals << 0;
    }

    {
      clEvent event;
      ret = clEnqueueWriteBuffer(_ocl_command_queue->queue, mem_in_intervals.mem, CL_FALSE, 0,
                                 intrvals_size_t*sizeof(cl_uint), intervals.data(), 0, NULL, &event.event);
      LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer mem_in_intervals...");
      clWaitForEvents(1, &event.event);
    }
    //
    // GPU applyDottedLine
    //
    {
      cl_float cell_size = tmpl_data->cell_size;
      clKernel kernel(clCreateKernel(_ocl_program->program, "applyCellsDottedLine", &ret));
      clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_in_intervals.mem);
      clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_smooth_normals.mem);
      clSetKernelArg(kernel.kernel, 2, sizeof(cl_float), &tmpl_data->dotline_threshold);
      clSetKernelArg(kernel.kernel, 3, sizeof(cl_float), &cell_size);


      // Execute kernel code
      clEvent event;
      ret = clEnqueueNDRangeKernel(_ocl_command_queue->queue,
                                   kernel.kernel,
                                   1,
                                   NULL,
                                   &intrvals_size_t,
                                   &local_size,
                                   0,
                                   NULL,
                                   &event.event);
      LOG_OCL_ERROR(ret, "Error in clEnqueueNDRangeKernel applyCellsDottedLine");
      clWaitForEvents(1, &event.event);
    }
  }

  i_dotted -= i_edged;

  if (!i_dotted.isEmpty() && tmpl_data->dotline_threshold > 0)
  {
    QVector<cl_uint> intervals = i_dotted.pairs;
    cl_int interval_i = intervals.count();
    size_t intrvals_size_t=local_size*int((interval_i + local_size-1)/local_size);
    clMem mem_in_intervals(clCreateBuffer(_ocl_context->context, CL_MEM_READ_ONLY,
                                          intrvals_size_t*sizeof(cl_uint), NULL, &ret));
    int intrvals_size = intrvals_size_t;
    while(intervals.size() < intrvals_size)
    {
      intervals << 0;
    }

    {
      clEvent event;
      ret = clEnqueueWriteBuffer(_ocl_command_queue->queue, mem_in_intervals.mem, CL_FALSE, 0,
                                 intrvals_size_t*sizeof(cl_uint), intervals.data(), 0, NULL, &event.event);
      LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer mem_in_intervals...");
      clWaitForEvents(1, &event.event);
    }
    //
    // GPU applyDottedLine
    //
    {
      Bicubic bic = tmpl_data->bic;
      cl_float white = tmpl_data->white;//*intv_sc;
      cl_float black = tmpl_data->black;//*intv_sc;

      clKernel kernel(clCreateKernel(_ocl_program->program, "applyDottedLine", &ret));

      clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_in_intervals.mem);
      clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_smooth_normals.mem);
      clSetKernelArg(kernel.kernel, 2, sizeof(cl_float), &tmpl_data->dotline_threshold);
      clSetKernelArg(kernel.kernel, 3, sizeof(cl_float), &white);
      clSetKernelArg(kernel.kernel, 4, sizeof(cl_float), &black);
      clSetKernelArg(kernel.kernel, 5, sizeof(cl_int), &steps_on_px);
      clSetKernelArg(kernel.kernel, 6, sizeof(Bicubic), &bic);
      // Execute kernel code
      clEvent event;
      ret = clEnqueueNDRangeKernel(_ocl_command_queue->queue,
                                   kernel.kernel,
                                   1,
                                   NULL,
                                   &intrvals_size_t,
                                   &local_size,
                                   0,
                                   NULL,
                                   &event.event);
      LOG_OCL_ERROR(ret, "Error in clEnqueueNDRangeKernel applyDottedLine");
      clWaitForEvents(1, &event.event);
    }
  }

  //
  // GPU applyRoundCups
  //
  if (tmpl_data->round_cups)
  {
    int dot_interval_size=0;
    QVector<cl_uint> dot_intervals;
    {
      QVector<cl_float2> smooth_normals(size);
      clEvent event;
      ret = clEnqueueReadBuffer(_ocl_command_queue->queue,
                                mem_smooth_normals.mem,
                                CL_TRUE,
                                0,
                                size*sizeof(cl_float2),
                                smooth_normals.data(),
                                0,
                                NULL,
                                &event.event);
      LOG_OCL_ERROR(ret, "clEnqueueReadBuffer 0xff21 Failed");
      clWaitForEvents(1, &event.event);

      {
        bool prev_is_null=true;
        GLint first_ix=0;
        for(int i=0; i < size; ++i)
        {
          GLint gl_i = i;
          cl_float2 n = gl_i < size ? smooth_normals[gl_i] : cl_float2{{0,0}};
          bool is_null = fabs(n.x) <= 0.01 && fabs(n.y) <= 0.01;

          if ((prev_is_null &&  is_null) ||
              (!prev_is_null && !is_null))
          {
            prev_is_null = is_null;
            continue;
          }

          cl_uint cl_i=i;
          dot_intervals << (is_null ? (cl_i-1) : cl_i);

          if (prev_is_null && !is_null)
          {
            first_ix = gl_i;
            prev_is_null = false;
            continue;
          }
          first_ix = 0;
          prev_is_null = true;
        }
      }
      dot_interval_size = dot_intervals.size();
    }
#ifdef RENDER_DEBUG_INFO
    qDebug() << "    createBufferDescriptors:" << debug_time.elapsed(); debug_time.restart();
#endif

    if (dot_interval_size > 1)
    {

      size_t dot_intrvals_size_t=local_size*int((dot_interval_size+local_size-1)/local_size);
      clMem mem_in_dot_intervals(clCreateBuffer(_ocl_context->context, CL_MEM_READ_ONLY,
                                                dot_intrvals_size_t*sizeof(cl_uint), NULL, &ret));

      {
        clEvent event;
        ret = clEnqueueWriteBuffer(_ocl_command_queue->queue, mem_in_dot_intervals.mem, CL_FALSE, 0,
                                   dot_intrvals_size_t*sizeof(cl_uint), dot_intervals.data(), 0, NULL, &event.event);
        LOG_OCL_ERROR(ret, "clEnqueueWriteBuffer dot_intervals...");
        clWaitForEvents(1, &event.event);
      }

      cl_uint vbo_buff_round_cups_size = (dot_interval_size + 1)*37*12;

      if (_painter_data->round_cups_vbo_id == 0)
      {
        glGenBuffers(1, &_painter_data->round_cups_vbo_id);
      }
      glBindBuffer(GL_ARRAY_BUFFER, _painter_data->round_cups_vbo_id);
      glBufferData(GL_ARRAY_BUFFER, vbo_buff_round_cups_size*sizeof(cl_float), 0, GL_STATIC_DRAW);

      clMem mem_vbo_buff_round_cups(clCreateFromGLBuffer(_ocl_context->context, CL_MEM_READ_WRITE, _painter_data->round_cups_vbo_id, &ret));
      LOG_OCL_ERROR(ret, "Creating Stroke GL VBO to CL Buffer failed");

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      {
        clKernel kernel(clCreateKernel(_ocl_program->program, "createRoundCups", &ret));
        clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_in_dot_intervals.mem);
        clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_smooth_track.mem);
        clSetKernelArg(kernel.kernel, 2, sizeof(cl_mem), &mem_smooth_normals.mem);
        clSetKernelArg(kernel.kernel, 3, sizeof(cl_mem), &mem_vbo_buff_round_cups.mem);
        clSetKernelArg(kernel.kernel, 4, sizeof(cl_float), &tmpl_data->thickness);
        clSetKernelArg(kernel.kernel, 5, sizeof(cl_float), &tmpl_data->thickness_base);
        clSetKernelArg(kernel.kernel, 6, sizeof(cl_uint), &dot_interval_size);

        clEvent event;
        ret = clEnqueueNDRangeKernel(_ocl_command_queue->queue,
                                     kernel.kernel,
                                     1,
                                     NULL,
                                     &dot_intrvals_size_t,
                                     &local_size,
                                     0,
                                     NULL,
                                     &event.event);
        LOG_OCL_ERROR(ret, "clEnqueueNDRangeKernel 0xff22 failed");
        clWaitForEvents(1, &event.event);
      }
#ifdef RENDER_DEBUG_INFO
      qDebug() << "    createRoundCups:" << debug_time.elapsed(); debug_time.restart();
#endif

      _painter_data->round_cups_first = QVector<GLint>(dot_interval_size);
      _painter_data->round_cups_count = QVector<GLsizei>(dot_interval_size);

      for(int i=0; i < dot_interval_size; ++i)
      {
        cl_int cl_i = i;
        _painter_data->round_cups_first[i] = cl_i*37*2;
        _painter_data->round_cups_count[i] = 37*2;
      }
#ifdef RENDER_DEBUG_INFO
      qDebug() << "    createRoundCups finalized.";
#endif
    }
  }
  //
  // GPU createVBOBuffer2d
  //
  {
    bool prev_is_null=true;
    GLint first_ix=0;
    for(int i=0; i < size+1; ++i)
    {
      GLint gl_i = i;
      cl_float2 n = gl_i < size ? smooth_normals[gl_i] : cl_float2{{0,0}};
      bool is_null = n.x == 0 && n.y == 0;

      if ((prev_is_null &&  is_null) ||
          (!prev_is_null && !is_null))
      {
        prev_is_null = is_null;
        continue;
      }

      if (prev_is_null && !is_null)
      {
        first_ix = gl_i;
        prev_is_null = false;
        continue;
      }
      _painter_data->stroke_first << first_ix*2;
      _painter_data->stroke_count << (gl_i-first_ix-1)*2;
      first_ix = 0;
      prev_is_null = true;
    }
  }
  {
    clKernel kernel(clCreateKernel(_ocl_program->program, "createVBOBuffer2d", &ret));
    if (_painter_data->stroke_vbo_id == 0)
    {
      glGenBuffers(1, &_painter_data->stroke_vbo_id);
    }
    glBindBuffer(GL_ARRAY_BUFFER, _painter_data->stroke_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vbo_buff_stroke_size*sizeof(GLfloat), 0, GL_STATIC_DRAW);

    clMem mem_vbo_buff_stroke(clCreateFromGLBuffer(_ocl_context->context,
                                                   CL_MEM_READ_WRITE, _painter_data->stroke_vbo_id, &ret));
    LOG_OCL_ERROR(ret, "Creating Stroke GL VBO to CL Buffer failed");

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    clSetKernelArg(kernel.kernel, 0, sizeof(cl_mem), &mem_smooth_track.mem);
    clSetKernelArg(kernel.kernel, 1, sizeof(cl_mem), &mem_smooth_normals.mem);
    clSetKernelArg(kernel.kernel, 2, sizeof(cl_mem), &mem_vbo_buff_stroke.mem);
    clSetKernelArg(kernel.kernel, 3, sizeof(cl_float), &tmpl_data->thickness);
    clSetKernelArg(kernel.kernel, 4, sizeof(cl_float), &tmpl_data->thickness_base);
    clSetKernelArg(kernel.kernel, 5, sizeof(cl_uint), &size);

    clEvent event;
    ret = clEnqueueNDRangeKernel(_ocl_command_queue->queue,
                                 kernel.kernel,
                                 1,
                                 NULL,
                                 &global_size,
                                 &local_size,
                                 0,
                                 NULL,
                                 &event.event);
    LOG_OCL_ERROR(ret, "clEnqueueNDRangeKernel 0xff23 failed");
    clWaitForEvents(1, &event.event);
  }
#ifdef RENDER_DEBUG_INFO
  qDebug() << "create createVBOBuffer2d:" << debug_time.elapsed();
#endif
  //
  // release
  //
  timeRendering = _time.elapsed();

  TIME_LOG("Create Stroke CL finished.");

#ifdef RENDER_DEBUG_INFO
  qDebug() << "render finished.";
#endif
  return CL_SUCCESS;
}
#endif

void OGLStrokesRenderImpl::TIME_LOG(const QString &)
{
#ifdef RENDER_DEBUG_INFO
  qDebug() << QString("%1: %2").arg(txt).arg(_time.elapsed()); _time.restart();
#endif
}


