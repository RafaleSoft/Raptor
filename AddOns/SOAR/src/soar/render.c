/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@i-m-p-o-r-t-e-d---h-e-a-d-e-r-s-------------------------------------------*/

#include <stdio.h>
#include <glut.h>
#include "common.h"
#include "terrain.h"
#include "render.h"

/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#define NORMAL(v, p0, p1, p2)\
  (void)(\
    (v)[0] = ((p1)[1] - (p0)[1]) * ((p2)[2] - (p0)[2]) -\
             ((p1)[2] - (p0)[2]) * ((p2)[1] - (p0)[1]),\
    (v)[1] = ((p1)[2] - (p0)[2]) * ((p2)[0] - (p0)[0]) -\
             ((p1)[0] - (p0)[0]) * ((p2)[2] - (p0)[2]),\
    (v)[2] = ((p1)[0] - (p0)[0]) * ((p2)[1] - (p0)[1]) -\
             ((p1)[1] - (p0)[1]) * ((p2)[0] - (p0)[0])\
  )

/*@p-r-i-v-a-t-e---p-r-o-t-o-t-y-p-e-s---------------------------------------*/

static void
tstrip_index_render_basic(
  const VERTEX          *vv,
  const VERTEXid        *vip,
  const VERTEXid        * const end
);

static void
tstrip_index_render_lighted(
  const VERTEX          *vv,
  const VERTEXid        *vip,
  const VERTEXid        * const end
);

static void
tstrip_vector_render_basic(
  const VECTOR3f        *vp,
  const VECTOR3f        * const end
);

static void
tstrip_vector_render_lighted(
  const VECTOR3f        *vp,
  const VECTOR3f        * const end
);

/*@p-r-i-v-a-t-e---f-u-n-c-t-i-o-n-s-----------------------------------------*/

/*@tstrip_index_render_basic.................................................*/

static void
tstrip_index_render_basic(
  const VERTEX          *vv,
  const VERTEXid        *vip,
  const VERTEXid        * const end
)
{
  do
    glVertex3fv(vv[*vip++].p);
  while (vip != end);
}

/*@tstrip_index_render_lighted...............................................*/

static void
tstrip_index_render_lighted(
  const VERTEX          *vv,
  const VERTEXid        *vip,
  const VERTEXid        * const end
)
{
  const GLfloat         *p0, *p1, *p2;
  BOOL                  flip = FALSE;

  p0 = p1 = vv[*vip].p;
  do {
    /*
    ** If triangle is not degenerate, compute its normal.
    */
    if ((p2 = vv[*vip++].p) != p0) {
      VECTOR3f v;
      NORMAL(v, flip ? p1 : p0, flip ? p0 : p1, p2);
      glNormal3fv(v);
    }
    glVertex3fv(p2);
    flip = !flip;
    p0 = p1;
    p1 = p2;
  } while (vip != end);
}

/*@tstrip_vector_render_basic................................................*/

static void
tstrip_vector_render_basic(
  const VECTOR3f        *vp,
  const VECTOR3f        * const end
)
{
  do
    glVertex3fv(*vp++);
  while (vp != end);
}

/*@tstrip_vector_render_lighted..............................................*/

static void
tstrip_vector_render_lighted(
  const VECTOR3f        *vp,
  const VECTOR3f        * const end
)
{
  const GLfloat         *p0, *p1, *p2;
  VECTOR3f              v;
  BOOL                  flip = FALSE;

  p1 = p2 = *vp;
  do {
    p0 = p1;
    p1 = p2;
    p2 = *vp++;
    NORMAL(v, flip ? p1 : p0, flip ? p0 : p1, p2);
    glNormal3fv(v);
    glVertex3fv(p2);
    flip = !flip;
  } while (vp != end);
}

/*@p-u-b-l-i-c---f-u-n-c-t-i-o-n-s-------------------------------------------*/

/*@stats_render..............................................................*/

void
stats_render(
  FLOAT         tau_min,
  FLOAT         tau_max,
  unsigned      triangles,
  unsigned      fps
)
{
  static char   string[0x100];
  GLint         y = 4;

  if (tau_min >= 0.0f) {
    if (tau_max > tau_min)
      sprintf(string, "tau = %.1f - %.1f", tau_min, tau_max);
    else
      sprintf(string, "tau = %.1f", tau_min);
    string_render(string, 4, y);
    y += 16;
  }

  if (triangles) {
    sprintf(string, "tri = %u", triangles);
    string_render(string, 4, y);
    y += 16;
  }

  if (fps) {
    sprintf(string, "fps = %u", fps);
    string_render(string, 4, y);
    y += 16;
  }
}

/*@string_render.............................................................*/

void
string_render(
  const char    *string,
  GLint         x,
  GLint         y
)
{
  unsigned      i;

  glColor3f(0.0f, 0.0f, 0.0f);
  glRasterPos2i(x + 1, y - 1);
  for (i = 0; string[i]; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
  glColor3f(1.0f, 1.0f, 1.0f);
  glRasterPos2i(x, y);
  for (i = 0; string[i]; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
}

/*@terrain_render_basic......................................................*/

void
terrain_render_basic(
  const VERTEXbuffer    *buffer
)
{
  glBegin(GL_TRIANGLE_STRIP);
  if (buffer->indexed)
    tstrip_index_render_basic(
      buffer->vertex,
      (VERTEXid *)buffer->strip + 1,
      (VERTEXid *)buffer->strip + buffer->count
    );
  else
    tstrip_vector_render_basic(
      (const VECTOR3f *)buffer->strip + 1,
      (const VECTOR3f *)buffer->strip + buffer->count
    );
  glEnd();
}

/*@terrain_render_lighted....................................................*/

void
terrain_render_lighted(
  const VERTEXbuffer    *buffer
)
{
  glBegin(GL_TRIANGLE_STRIP);
  if (buffer->indexed)
    tstrip_index_render_lighted(
      buffer->vertex,
      (VERTEXid *)buffer->strip + 1,
      (VERTEXid *)buffer->strip + buffer->count
    );
  else
    tstrip_vector_render_lighted(
      (const VECTOR3f *)buffer->strip + 1,
      (const VECTOR3f *)buffer->strip + buffer->count
    );
  glEnd();
}

/*@view_volume_render........................................................*/

void
view_volume_render(
  unsigned      size_x,
  unsigned      size_y,
  GLdouble      size
)
{
  GLdouble      min_x, min_y, max_x, max_y;

  min_x = 0.5 * (1.0 - size) * size_x;
  min_y = 0.5 * (1.0 - size) * size_y;
  max_x = 0.5 * (1.0 + size) * size_x;
  max_y = 0.5 * (1.0 + size) * size_y;

  glPushAttrib(GL_LINE_BIT);
  glLineWidth(2.0f);
  glColor3f(1.0f/3.0f, 1.0f/3.0f, 2.0f/3.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2d(min_x, min_y);
  glVertex2d(max_x, min_y);
  glVertex2d(max_x, max_y);
  glVertex2d(min_x, max_y);
  glEnd();
  glPopAttrib();
}
