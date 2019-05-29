#include "coglstrokespainter.h"


void COGLStrokesPainter::paint(QPainter*painter, COGLPainterData * pd, Strip strip)
{
  //strip = LinearStrip;
  painter->beginNativePainting();
  {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    QColor c = 0xff000000;
    glColor3f(c.redF(), c.greenF(), c.blueF());
    glLineWidth(0);
    {
      glEnableClientState( GL_VERTEX_ARRAY );
      glBindBuffer(GL_ARRAY_BUFFER, pd->stroke_vbo_id);
      glVertexPointer(3, GL_FLOAT, 0, 0);

      if (strip == TriangleStrip)
        glMultiDrawArrays(GL_TRIANGLE_STRIP, pd->stroke_first.data(),
                          pd->stroke_count.data(), pd->stroke_first.size());
      else
        glMultiDrawArrays(GL_LINE_STRIP, pd->stroke_first.data(),
                          pd->stroke_count.data(), pd->stroke_first.size());

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (pd->round_cups_vbo_id > 0)
    {
      QColor c = 0xff000000;
      glColor3f(c.redF(), c.greenF(), c.blueF());
      glLineWidth(0);
      glEnableClientState( GL_VERTEX_ARRAY );
      glBindBuffer(GL_ARRAY_BUFFER, pd->round_cups_vbo_id);
      glVertexPointer(3, GL_FLOAT, 0, 0);

      if (strip == TriangleStrip)
        glMultiDrawArrays(GL_TRIANGLE_STRIP, pd->round_cups_first.data(),
                          pd->round_cups_count.data(), pd->round_cups_first.size());
      else
        glMultiDrawArrays(GL_LINE_STRIP, pd->round_cups_first.data(),
                          pd->round_cups_count.data(), pd->round_cups_first.size());

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (pd->cells_vbo_id > 0)
    {
      QColor c = 0xff000000;
      glColor3f(c.redF(), c.greenF(), c.blueF());
      glLineWidth(0);
      glEnableClientState( GL_VERTEX_ARRAY );
      glBindBuffer(GL_ARRAY_BUFFER, pd->cells_vbo_id);
      glVertexPointer(3, GL_FLOAT, 0, 0);

      if (strip == TriangleStrip)
        glMultiDrawArrays(GL_TRIANGLE_STRIP, pd->cells_first.data(),
                          pd->cells_count.data(), pd->cells_count.size());
      else
        glMultiDrawArrays(GL_LINE_STRIP, pd->cells_first.data(),
                          pd->cells_count.data(), pd->cells_count.size());

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (pd->cells_roundcups_vbo_id > 0)
    {
      QColor c = 0xff000000;
      glColor3f(c.redF(), c.greenF(), c.blueF());
      glLineWidth(0);
      glEnableClientState( GL_VERTEX_ARRAY );
      glBindBuffer(GL_ARRAY_BUFFER, pd->cells_roundcups_vbo_id);
      glVertexPointer(3, GL_FLOAT, 0, 0);

      if (strip == TriangleStrip)
        glMultiDrawArrays(GL_TRIANGLE_STRIP, pd->round_cells_roundcups_first.data(),
                          pd->round_cells_roundcups_count.data(), pd->round_cells_roundcups_count.size());
      else
        glMultiDrawArrays(GL_LINE_STRIP, pd->round_cells_roundcups_first.data(),
                          pd->round_cells_roundcups_count.data(), pd->round_cells_roundcups_count.size());

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (pd->centers_vbo_id > 0)
    {
      QColor c = 0xff000000;
      glColor3f(c.redF(), c.greenF(), c.blueF());
      glLineWidth(0);
      glEnableClientState( GL_VERTEX_ARRAY );
      glBindBuffer(GL_ARRAY_BUFFER, pd->centers_vbo_id);
      glVertexPointer(3, GL_FLOAT, 0, 0);

      if (strip == TriangleStrip)
        glMultiDrawArrays(GL_TRIANGLE_STRIP, pd->centers_first.data(),
                          pd->centers_count.data(), pd->centers_count.size());
      else
        glMultiDrawArrays(GL_LINE_STRIP, pd->centers_first.data(),
                          pd->centers_count.data(), pd->centers_count.size());

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (pd->centers_roundcups_vbo_id > 0)
    {
      QColor c = 0xff000000;
      glColor3f(c.redF(), c.greenF(), c.blueF());
      glLineWidth(0);
      glEnableClientState( GL_VERTEX_ARRAY );
      glBindBuffer(GL_ARRAY_BUFFER, pd->centers_roundcups_vbo_id);
      glVertexPointer(3, GL_FLOAT, 0, 0);

      if (strip == TriangleStrip)
        glMultiDrawArrays(GL_TRIANGLE_STRIP, pd->round_centers_roundcups_first.data(),
                          pd->round_centers_roundcups_count.data(), pd->round_centers_roundcups_count.size());
      else
        glMultiDrawArrays(GL_LINE_STRIP, pd->round_centers_roundcups_first.data(),
                          pd->round_centers_roundcups_count.data(), pd->round_centers_roundcups_count.size());

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableClientState(GL_VERTEX_ARRAY);
    }

    painter->endNativePainting();
  }
}
