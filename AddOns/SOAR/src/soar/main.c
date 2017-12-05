/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@i-m-p-o-r-t-e-d---h-e-a-d-e-r-s-------------------------------------------*/

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glut.h>
#include "common.h"
#include "index.h"
#include "terrain.h"
#include "refine.h"
#include "render.h"
#include "geoio.h"
#include "pngio.h"

/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#define BUFFER_CAPACITY 0x100000        /* default vertex buffer capacity */
#define BUFFER_FACTOR   8               /* inverse of maximum buffer load */
#define DEPTH_PRECISION 10              /* z-buffer bits to avoid aliasing */
#define DEPTH_RANGE_MIN 8               /* log of minimum far to near ratio */
#define MINIFY_MIN      0               /* minimum cull region minification */
#define MINIFY_MAX      8               /* maximum cull region minification */

#define VIEW_VOLUME_SIZE(m)\
  (1.0 - 0.75 * (GLdouble)(m) / MINIFY_MAX)

/*@p-r-i-v-a-t-e---t-y-p-e-s-------------------------------------------------*/

typedef struct {
  unsigned      size_x;                 /* window width in pixels */
  unsigned      size_y;                 /* window height in pixels */
  unsigned      minification;           /* cull region minification */
  FLOAT         fov_x;                  /* horizontal field of view */
  FLOAT         fov_y;                  /* vertical field of view */
  FLOAT         fov_pixel;              /* pixel width/height in degrees */
  FLOAT         near_z;                 /* distance to near plane */
  FLOAT         far_z;                  /* distance to far plane */
  VECTOR3f      axis_x;                 /* x axis in world coordinates */
  VECTOR3f      axis_y;                 /* y axis in world coordinates */
  VECTOR3f      axis_z;                 /* z axis in world coordinates */
  VECTOR3f      position;               /* viewpoint */
  VECTOR4f      plane[6];               /* view frustum plane equations */
} VIEW;

typedef struct {
  BOOL          freeze;                 /* disallow refinement when set */
  BOOL          refine;                 /* refine when set */
  unsigned      tolerance;              /* tolerance in tenths of pixels */
  FLOAT         tau;                    /* error tolerance in pixels */
  FLOAT         tau_min;                /* min error tolerance for morphing */
  FLOAT         tau_max;                /* max error tolerance for morphing */
  VDinfo        vd;                     /* view-dependent parameters */
} LODinfo;

typedef struct {
  BOOL          fill;                   /* fill polygons when set */
  BOOL          lighting;               /* light polygons when set */
  BOOL          stats;                  /* display statistics when set */
  BOOL          texture;                /* texture terrain when set */
  BOOL          wireframe;              /* wireframe polygons when set */
  GLbitfield    clear;                  /* bitplanes to clear */
  GLfloat       terrain_color[3];       /* foreground color */
  GLfloat       wireframe_color[3];     /* wireframe color */
  GLfloat       light[4];               /* light source direction */
} GLinfo;

typedef struct {
  int           button;                 /* GLUT mouse button */
  int           x;                      /* mouse horizontal position */
  int           y;                      /* mouse vertical position */
} MOUSE;

typedef struct {
  MOUSE         mouse;                  /* mouse information */
  FLOAT         speed;                  /* flight speed */
  FLOAT         quantum;                /* flight speed increment */
  VECTOR3f      tangent;                /* tangent for rotations */
} GUI;

/*@p-r-i-v-a-t-e---p-r-o-t-o-t-y-p-e-s---------------------------------------*/

static void
display_callback(
  void
);

static unsigned
fps_get(
  void
);

static void
gl_init(
  void
);

static void
gl_ortho_begin(
  unsigned      size_x,
  unsigned      size_y
);

static void
gl_ortho_end(
  void
);

static void
gl_update(
  void
);

static void
idle_callback(
  void
);

static void
keyboard_callback(
  unsigned char key,
  int           x,
  int           y
);

static void
lod_init(
  void
);

static void
lod_update(
  void
);

static void
motion_callback(
  int   x,
  int   y
);

static void
mouse_callback(
  int   button,
  int   state,
  int   x,
  int   y
);

static void
reshape_callback(
  int   width,
  int   height
);

static void
special_callback(
  int   key,
  int   x,
  int   y
);

static void
terrain_init(
  const char    *geofile,
  const char    *texfile
);

static void
vertex_buffer_get(
  VERTEXbuffer  *buffer
);

static void
view_update(
  void
);

/*@p-r-i-v-a-t-e---d-a-t-a---------------------------------------------------*/

static TERRAIN          terrain;
static VERTEXbuffer     vertex_buffer;
static LODinfo          lod;
static VIEW             view;
static GLinfo           gl;
static GUI              gui;

/*@p-r-i-v-a-t-e---f-u-n-c-t-i-o-n-s-----------------------------------------*/

/*@display_callback..........................................................*/

static void
display_callback(
  void
)
{

  VERTEXbuffer  *buffer = &vertex_buffer;

  glClear(gl.clear);

  /*
  ** Refine mesh while z-buffer is being cleared.
  */

  vertex_buffer_get(buffer);

  /*
  ** Render terrain surface.
  */

  if (gl.fill) {
    glColor3fv(gl.terrain_color);
    if (gl.lighting)
      terrain_render_lighted(buffer);
    else
      terrain_render_basic(buffer);
  }

  /*
  ** Draw wireframe mesh.
  */

  if (gl.wireframe) {
    glColor3fv(gl.wireframe_color);
    if (gl.lighting)
      glDisable(GL_LIGHTING);
    if (gl.fill)
      glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    terrain_render_basic(buffer);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (gl.fill)
      glEnable(GL_POLYGON_OFFSET_FILL);
    if (gl.lighting)
      glEnable(GL_LIGHTING);
  }

  /*
  ** Display statistics.
  */

  if (gl.stats || (view.minification && !lod.freeze)) {
    gl_ortho_begin(view.size_x, view.size_y);
    if (view.minification && !lod.freeze)
      view_volume_render(
        view.size_x, view.size_y, VIEW_VOLUME_SIZE(view.minification)
      );
    if (gl.stats)
      stats_render(lod.tau_min, lod.tau_max, buffer->triangles, fps_get());
    gl_ortho_end();
  }

  glutSwapBuffers();
}

/*@fps_get...................................................................*/

static unsigned
fps_get(
  void
)
{
  static unsigned       fps = 0;
  static unsigned       frames = 0;
  static time_t         last = -1;
  time_t                t;
  double                seconds;

  frames++;
  t = time(NULL);
  seconds = difftime(t, last);
  if (seconds >= 1.0) {
    last = t;
    fps = (unsigned)floor(frames / seconds + 0.5);
    frames = 0;
  }

  return fps;
}

/*@gl_init...................................................................*/

static void
gl_init(
  void
)
{
  const GEOMETRY        *gp = &terrain.geometry;
  const TEXTURE         *tp = &terrain.texture;
  const GLfloat         zero[] = { 0.0f, 0.0f, 0.0f, 0.0f },
                        one[] = { 1.0f, 1.0f, 1.0f, 1.0f },
                        ambient[] = { 0.125f, 0.125f, 0.125f, 1.0f },
                        specular[] = { 0.25f, 0.25f, 0.25f, 1.0f };
  GLfloat               plane[4];

  gl.fill = TRUE;
  gl.lighting = TRUE;
  gl.stats = TRUE;
  gl.texture = FALSE;
  gl.wireframe = FALSE;
  gl.terrain_color[0] = gl.terrain_color[1] = gl.terrain_color[2] = 1.0f;
  gl.wireframe_color[0] = gl.wireframe_color[1] = gl.wireframe_color[2] = 0.0f;
  gl.light[0] = -1.0f;
  gl.light[1] = +1.0f;
  gl.light[2] = +1.0f;
  gl.light[3] = 0.0f;

  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHT0);
  glClearColor(0.5f, 0.625f, 0.75f, 0.0f);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_AMBIENT, one);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, one);
  glLightfv(GL_LIGHT0, GL_SPECULAR, one);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zero);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
  glShadeModel(GL_FLAT);

  if (tp->image) {
    gl.lighting = FALSE;
    gl.texture = TRUE;
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(
      GL_TEXTURE_2D, 0, 3,
      tp->size_x, tp->size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, tp->image
    );
    glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    plane[0] = 1.0f / (gp->box[1][0] - gp->box[0][0]);
    plane[1] = 0.0f;
    plane[2] = 0.0f;
    plane[3] = -gp->box[0][0] / (gp->box[1][0] - gp->box[0][0]);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, plane);
    plane[0] = 0.0f;
    plane[1] = 1.0f / (gp->box[1][1] - gp->box[0][1]);
    plane[2] = 0.0f;
    plane[3] = -gp->box[0][1] / (gp->box[1][1] - gp->box[0][1]);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, plane);
  }

  gluLookAt(
    view.position[0], view.position[1], view.position[2],
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  );
  gl_update();
}

/*@gl_ortho_begin............................................................*/

static void
gl_ortho_begin(
  unsigned      size_x,
  unsigned      size_y
)
{
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0.375f, 0.375f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, size_x, 0, size_y);
  glMatrixMode(GL_MODELVIEW);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
}

/*@gl_ortho_end..............................................................*/

static void
gl_ortho_end(
  void
)
{
  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/*@gl_update.................................................................*/

static void
gl_update(
  void
)
{
  if (gl.wireframe) {
    if (gl.fill) {
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1.0f, 4.0f);
    }
    else {
      glDisable(GL_POLYGON_OFFSET_FILL);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
    }
  }
  else
    glDisable(GL_POLYGON_OFFSET_FILL);

  gl.clear = GL_COLOR_BUFFER_BIT;

  if (gl.fill) {
    gl.clear |= GL_DEPTH_BUFFER_BIT;
    glEnable(GL_DEPTH_TEST);
    if (gl.texture) {
      glEnable(GL_TEXTURE_2D);
      gl.terrain_color[0] = gl.terrain_color[1] = gl.terrain_color[2] = 1.0f;
    }
    else {
      glDisable(GL_TEXTURE_2D);
      gl.terrain_color[0] = 0.875f;
      gl.terrain_color[1] = 0.625f;
      gl.terrain_color[2] = 0.500f;
    }
    if (gl.lighting) {
      glEnable(GL_LIGHTING);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gl.terrain_color);
    }
    else
      glDisable(GL_LIGHTING);
  }

  glutPostRedisplay();
}

/*@gui_init..................................................................*/

static void
gui_init(
  void
)
{
  gui.speed = 0.0f;
  gui.quantum = view.far_z / 65536.0f;
}

/*@idle_callback.............................................................*/

static void
idle_callback(
  void
)
{
  if (fabsf(gui.speed) > 0.0f) {
    glTranslatef(
      -gui.speed * view.axis_z[0],
      -gui.speed * view.axis_z[1],
      -gui.speed * view.axis_z[2]
    );
    view_update();
    lod_update();
  }
  else
    glutIdleFunc(NULL);
}

/*@keyboard_callback.........................................................*/

/*ARGSUSED1*/
static void
keyboard_callback(
  unsigned char key,
  int           x,
  int           y
)
{
  switch (key) {
    case 0x1b: /* escape key */
      exit(EXIT_SUCCESS);
      break;
    case 'c':
      lod.vd.cull = !lod.vd.cull;
      lod_update();
      break;
    case 'f':
      gl.fill = !gl.fill;
      gl_update();
      break;
    case 'l':
      gl.lighting = !gl.lighting;
      gl_update();
      break;
    case 'm':
      lod.vd.morph = !lod.vd.morph;
      lod_update();
      break;
    case 'r':
      lod.freeze = !lod.freeze;
      lod_update();
      break;
    case 's':
      gl.stats = !gl.stats;
      gl_update();
      break;
    case 't':
      if (terrain.texture.image) {
        gl.texture = !gl.texture;
        gl_update();
      }
      break;
    case 'w':
      gl.wireframe = !gl.wireframe;
      gl_update();
      break;
  }
}

/*@lod_init..................................................................*/

static void
lod_init(
  void
)
{
  lod.freeze = FALSE;
  lod.refine = TRUE;
  lod.tolerance = 40;
  lod.tau = lod.tau_min = lod.tau_max = 0.1f * lod.tolerance;
  lod.vd.cull = TRUE;
  lod.vd.morph = TRUE;
  vertex_buffer.indexed = !lod.vd.morph;
  vertex_buffer.count = 0;
  vertex_buffer.capacity = BUFFER_CAPACITY;
  vertex_buffer.triangles = 0;
  if (!(vertex_buffer.strip =
          malloc(vertex_buffer.capacity * sizeof(VECTOR3f)))) {
    fprintf(stderr, "vertex buffer allocation failed\n");
    exit(EXIT_FAILURE);
  }
  vertex_buffer.vertex = NULL;
}

/*@lod_update................................................................*/

static void
lod_update(
  void
)
{
  FLOAT         kappa;
  unsigned      i, j;

  if (lod.freeze)
    lod.tau = lod.tau_min = lod.tau_max = -1.0f;
  else {
    lod.refine = TRUE;
    lod.tau = 0.1f * lod.tolerance;
    lod.tau_min = lod.tau;
    lod.tau_max = lod.vd.morph ? (3.0f / 2.0f) * lod.tau_min : lod.tau_min;
    kappa = (lod.tau / view.size_x) * DEG2RAD(view.fov_x);
    lod.vd.nu = kappa > 0.0f ? 1.0f / kappa : FLT_MAX;
    lod.vd.nu_min = (2.0f / 3.0f) * lod.vd.nu;
    lod.vd.nu_max = lod.vd.nu;
    COPY(lod.vd.viewpoint, view.position);
    for (i = 0; i != 6; i++)
      for (j = 0; j != 4; j++)
        lod.vd.viewplane[i][j] = view.plane[i][j];
  }

  glutPostRedisplay();
}

/*@menu_callback.............................................................*/

static void
menu_callback(
  int   value
)
{
  if (value < 0)
    special_callback(-value, -1, -1);
  else
    keyboard_callback((unsigned char)value, -1, -1);
}

/*@menu_init.................................................................*/

static void
menu_init(
  void
)
{
  int   lod, render;

  render = glutCreateMenu(menu_callback);
  glutAddMenuEntry("f - toggle polygon fill", 'f');
  glutAddMenuEntry("l - toggle lighting", 'l');
  glutAddMenuEntry("s - toggle statistics", 's');
  if (terrain.texture.image)
    glutAddMenuEntry("t - toggle texturing", 't');
  glutAddMenuEntry("w - toggle wireframe", 'w');
  lod = glutCreateMenu(menu_callback);
  glutAddMenuEntry("DN - decrease tolerance", -GLUT_KEY_DOWN);
  glutAddMenuEntry("UP - increase tolerance", -GLUT_KEY_UP);
  glutAddMenuEntry("LT - minify cull region", -GLUT_KEY_LEFT);
  glutAddMenuEntry("RT - magnify cull region", -GLUT_KEY_RIGHT);
  glutAddMenuEntry("c - toggle view culling", 'c');
  glutAddMenuEntry("m - toggle geomorphing", 'm');
  glutAddMenuEntry("r - toggle refinement", 'r');
  glutCreateMenu(menu_callback);
  glutAddSubMenu("rendering", render);
  glutAddSubMenu("level of detail", lod);
  glutAddMenuEntry("ESC - exit", 0x1b);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*@motion_callback...........................................................*/

static void
motion_callback(
  int   x,
  int   y
)
{
  VECTOR3f      axis, a, t;
  FLOAT         angle;

  x -= gui.mouse.x;
  gui.mouse.x += x;
  y -= gui.mouse.y;
  gui.mouse.y += y;

  switch (gui.mouse.button) {
    case GLUT_LEFT_BUTTON:
      /*
      ** Change flight speed.
      */
      gui.speed += y * gui.quantum;
      glutIdleFunc(idle_callback);
      break;
    case GLUT_MIDDLE_BUTTON:
      /*
      ** Perform rotation.
      */
      t[0] = x;
      t[1] = y;
      t[2] = 0.0f;
      CROSS_PRODUCT(a, t, gui.tangent);
      axis[0] = view.axis_x[0]*a[0] + view.axis_y[0]*a[1] + view.axis_z[0]*a[2];
      axis[1] = view.axis_x[1]*a[0] + view.axis_y[1]*a[1] + view.axis_z[1]*a[2];
      axis[2] = view.axis_x[2]*a[0] + view.axis_y[2]*a[1] + view.axis_z[2]*a[2];
      angle = view.fov_pixel * sqrtf(SQR((FLOAT)x) + SQR((FLOAT)y));
      glTranslatef(+view.position[0], +view.position[1], +view.position[2]);
      glRotatef(angle, axis[0], axis[1], axis[2]);
      glTranslatef(-view.position[0], -view.position[1], -view.position[2]);
      view_update();
      if (lod.vd.cull)
        lod_update();
      break;
  }
}

/*@mouse_callback............................................................*/

static void
mouse_callback(
  int   button,
  int   state,
  int   x,
  int   y
)
{
  FLOAT linf, l2, nx, ny;

  gui.mouse.x = x;
  gui.mouse.y = y;
  if (state == GLUT_DOWN)
    switch (gui.mouse.button = button) {
      case GLUT_LEFT_BUTTON:
        gui.speed = 0.0f;
        break;
      case GLUT_MIDDLE_BUTTON:
        /*
        ** Compute tangent vector to rotation vector.  The other tangent
        ** is given by the mouse motion in the projection plane.
        */
        nx = 2.0f * (x + 0.5f) / view.size_x - 1.0f;
        ny = 2.0f * (y + 0.5f) / view.size_y - 1.0f;
        if ((l2 = sqrtf(SQR(nx) + SQR(ny))) > 0.0f) {
          linf = MAX(fabsf(nx), fabsf(ny));
          gui.tangent[0] = nx * linf / l2;
          gui.tangent[1] = ny * linf / l2;
          gui.tangent[2] = -sqrtf(1.0f - SQR(linf));
        }
        else {
          gui.tangent[0] = 0.0f;
          gui.tangent[1] = 0.0f;
          gui.tangent[2] = -1.0f;
        }
        break;
    }
}

/*@reshape_callback..........................................................*/

static void
reshape_callback(
  int   width,
  int   height
)
{
  view.size_x = width;
  view.size_y = height;
  view.fov_y =
    RAD2DEG(2 * atanf(height * tanf(DEG2RAD(view.fov_x / 2)) / width));
  view.fov_pixel = view.fov_x / width;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(view.fov_y, (GLdouble)width / height, view.near_z, view.far_z);
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, width, height);
  view_update();
  lod_update();
}

/*@special_callback..........................................................*/

/*ARGSUSED1*/
static void
special_callback(
  int   key,
  int   x,
  int   y
)
{
  switch (key) {
    case GLUT_KEY_DOWN:
      if (lod.tolerance)
        lod.tolerance--;
      lod_update();
      break;
    case GLUT_KEY_UP:
      lod.tolerance++;
      lod_update();
      break;
    case GLUT_KEY_LEFT:
      if (view.minification != MINIFY_MAX) {
        view.minification++;
        view_update();
        if (lod.vd.cull)
          lod_update();
      }
      break;
    case GLUT_KEY_RIGHT:
      if (view.minification != MINIFY_MIN) {
        view.minification--;
        view_update();
        if (lod.vd.cull)
          lod_update();
      }
      break;
  }
}

/*@terrain_init..............................................................*/

static void
terrain_init(
  const char    *geofile,       /* terrain geometry file */
  const char    *texfile        /* terrain texture file */
)
{
  GEOMETRY      *gp = &terrain.geometry;
  TEXTURE       *tp = &terrain.texture;

  if (!(gp->vertex =
          geo_read(gp->box, &gp->vertices, &gp->levels, geofile, INDEX))) {
    fprintf(stderr, "can't read terrain geometry file '%s'\n", geofile);
    exit(EXIT_FAILURE);
  }

  if (texfile) {
    if (!(tp->image =
            png_read(
              &tp->size_x, &tp->size_y, &tp->size_z, NULL, texfile, TRUE
            )))
      fprintf(stderr, "can't read terrain texture file '%s'\n", texfile);
    else if (!ISPOW2(tp->size_x) || !ISPOW2(tp->size_y)) {
      fprintf(stderr, "texture dimensions must be powers of two\n");
      free(tp->image);
      tp->image = NULL;
    }
    else if (tp->size_z != 3) {
      fprintf(stderr, "texture is not RGB\n");
      free(tp->image);
      tp->image = NULL;
    }
  }
  else
    tp->image = NULL;
}

/*@vertex_buffer_get.........................................................*/

static void
vertex_buffer_get(
  VERTEXbuffer  *buffer
)
{
  if (lod.refine) {
    lod.refine = FALSE;
    /*
    ** Double the size of the vertex buffer if it becomes too full.
    */
    if (BUFFER_FACTOR * buffer->count > buffer->capacity) {
      free(buffer->strip);
      while (BUFFER_FACTOR * buffer->count > buffer->capacity)
        buffer->capacity *= 2;
      if (!(buffer->strip = malloc(buffer->capacity * sizeof(VECTOR3f)))) {
        fprintf(stderr, "vertex buffer reallocation failed\n");
        exit(EXIT_FAILURE);
      }
    }
    /*
    ** Perform view-dependent refinement on mesh.
    */
    buffer->vertex = terrain.geometry.vertex;
    mesh_construct(buffer, &lod.vd, terrain.geometry.levels);
  }
}

/*@view_init.................................................................*/

static void
view_init(
  void
)
{
  const GEOMETRY        *gp = &terrain.geometry;
  GLint                 depth;

  glGetIntegerv(GL_DEPTH_BITS, &depth);
  depth = MAX(DEPTH_RANGE_MIN, depth - DEPTH_PRECISION);

  view.minification = MINIFY_MIN;
  view.fov_x = 45.0f;
  view.far_z = 2.0f * DISTANCE(gp->box[0], gp->box[1]);
  view.near_z = view.far_z / powf(2.0f, depth);
  view.position[0] = 0.5f * (gp->box[0][0] + gp->box[1][0]);
  view.position[1] = 0.5f * (gp->box[0][1] + gp->box[1][1]);
  view.position[2] = 2.0f * gp->box[1][2] - gp->box[0][2];
}

/*@view_update...............................................................*/

static void
view_update(
  void
)
{
  GLdouble      d, l, plane[4], mp[16], mv[16], mpv[4][4];
  unsigned      i, j, k;

  if (gl.lighting)
    glLightfv(GL_LIGHT0, GL_POSITION, gl.light);

  /*
  ** Compute viewpoint and orientation.
  */

  glGetDoublev(GL_MODELVIEW_MATRIX, mv);
  for (i = 0; i != 3; i++) {
    view.axis_x[i] = (FLOAT)mv[4 * i + 0];
    view.axis_y[i] = (FLOAT)mv[4 * i + 1];
    view.axis_z[i] = (FLOAT)mv[4 * i + 2];
    for (j = 0, view.position[i] = 0.0f; j != 3; j++)
      view.position[i] -= (FLOAT)(mv[4 * i + j] * mv[12 + j]);
  }

  /*
  ** Multiply modelview and projection matrices.
  */

  glGetDoublev(GL_PROJECTION_MATRIX, mp);
  for (i = 0; i != 4; i++)
    for (j = 0; j != 4; j++)
      for (k = 0, mpv[i][j] = 0.0; k != 4; k++)
        mpv[i][j] += mp[4 * k + i] * mv[4 * j + k];

  /*
  ** Compute view frustum planes.
  */

  for (i = 0; i != 6; i++) {
    d = i < 4 && view.minification ? VIEW_VOLUME_SIZE(view.minification) : 1.0;
    k = i / 2;
    for (j = 0; j != 4; j++)
      plane[j] = (i & 1 ? +mpv[k][j] : -mpv[k][j]) - d * mpv[3][j];
    l = sqrt(SQR(plane[0]) + SQR(plane[1]) + SQR(plane[2]));
    for (j = 0; j != 4; j++)
      view.plane[i][j] = (FLOAT)(plane[j] / l);
  }

  glutPostRedisplay();
}

/*@m-a-i-n---f-u-n-c-t-i-o-n-------------------------------------------------*/

int
main(
  int   argc,
  char  *argv[]
)
{
  fprintf(stderr, "\nSOAR Terrain Engine v1.11\n");
  fprintf(stderr, "Peter Lindstrom and Valerio Pascucci\n");
  fprintf(stderr, "UCRL-CODE-2002-023\n");
  fprintf(stderr, "Copyright (c) 2002 University of California, LLNL\n\n");

  glutInitWindowSize(640, 480);
  glutInit(&argc, argv);

  switch (argc) {
    case 2:
      terrain_init(argv[1], NULL);
      break;
    case 3:
      terrain_init(argv[2], argv[1]);
      break;
    default:
      fprintf(stderr, "Usage: %s [texture file] <geometry file>\n", argv[0]);
      exit(EXIT_FAILURE);
  }

  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("SOAR");
  glutDisplayFunc(display_callback);
  glutReshapeFunc(reshape_callback);
  glutKeyboardFunc(keyboard_callback);
  glutSpecialFunc(special_callback);
  glutMouseFunc(mouse_callback);
  glutMotionFunc(motion_callback);

  lod_init();
  view_init();
  gl_init();
  gui_init();
  menu_init();

  glutMainLoop();

  return 0;
}
