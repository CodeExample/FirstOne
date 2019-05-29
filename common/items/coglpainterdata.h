#ifndef COGLPAINTERDATA_H
#define COGLPAINTERDATA_H

#include "cmodel.h"

#include <QtOpenGL>

using namespace sm;

struct COGLPainterData
{
public:
  QMutex mutex;

  ObjId tmpl_id=0;

  GLuint stroke_vbo_id=0;
  GLuint round_cups_vbo_id=0;

  GLuint cells_vbo_id=0;
  GLuint cells_roundcups_vbo_id=0;

  GLuint centers_vbo_id=0;
  GLuint centers_roundcups_vbo_id=0;

  QVector<GLint> stroke_first;
  QVector<GLsizei> stroke_count;
  QVector<GLint> round_cups_first;
  QVector<GLsizei> round_cups_count;

  QVector<GLint> cells_first;
  QVector<GLsizei> cells_count;
  QVector<GLint> round_cells_roundcups_first;
  QVector<GLsizei> round_cells_roundcups_count;

  QVector<GLint> centers_first;
  QVector<GLsizei> centers_count;
  QVector<GLint> round_centers_roundcups_first;
  QVector<GLsizei> round_centers_roundcups_count;

  void assign(const COGLPainterData &data)
  {
    stroke_vbo_id = data.stroke_vbo_id;
    round_cups_vbo_id = data.round_cups_vbo_id;

    cells_vbo_id = data.cells_vbo_id;
    cells_roundcups_vbo_id = data.cells_roundcups_vbo_id;

    centers_vbo_id = data.centers_vbo_id;
    centers_roundcups_vbo_id = data.centers_roundcups_vbo_id;

    stroke_first = data.stroke_first;
    stroke_count = data.stroke_count;
    round_cups_first = data.round_cups_first;
    round_cups_count = data.round_cups_count;

    cells_first = data.cells_first;
    cells_count = data.cells_count;
    round_cells_roundcups_first = data.round_cells_roundcups_first;
    round_cells_roundcups_count = data.round_cells_roundcups_count;

    centers_first = data.centers_first;
    centers_count = data.centers_count;
    round_centers_roundcups_first = data.round_centers_roundcups_first;
    round_centers_roundcups_count = data.round_centers_roundcups_count;
  }

  void clear()
  {
    stroke_first.clear();
    stroke_count.clear();
    round_cups_first.clear();
    round_cups_count.clear();

    cells_first.clear();
    cells_count.clear();
    round_cells_roundcups_first.clear();
    round_cells_roundcups_count.clear();

    centers_first.clear();
    centers_count.clear();
    round_centers_roundcups_first.clear();
    round_centers_roundcups_count.clear();

    if (stroke_vbo_id)
      glDeleteBuffers(1, &stroke_vbo_id);
    if (round_cups_vbo_id)
      glDeleteBuffers(1, &round_cups_vbo_id);

    if (cells_vbo_id)
      glDeleteBuffers(1, &cells_vbo_id);
    if (cells_roundcups_vbo_id)
      glDeleteBuffers(1, &cells_roundcups_vbo_id);

    if (centers_vbo_id)
      glDeleteBuffers(1, &centers_vbo_id);
    if (centers_roundcups_vbo_id)
      glDeleteBuffers(1, &centers_roundcups_vbo_id);

    stroke_vbo_id=0;
    round_cups_vbo_id=0;

    cells_vbo_id=0;
    cells_roundcups_vbo_id=0;

    centers_vbo_id=0;
    centers_roundcups_vbo_id=0;
  }

};


typedef COGLPainterData * COGLPainterDataSharedPtr;

#endif // COGLPAINTERDATA_H
