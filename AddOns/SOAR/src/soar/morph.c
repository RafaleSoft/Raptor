/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

#include <windows.h>
#include <GL\GL.h>
#include "common.h"
#include "terrain.h"
#include "refine.h"
#include "index.h"


extern unsigned
sphere_visible(
  const VERTEX  *vp,
  const VDinfo  *vdp,
  unsigned      mask
);

/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#if WITH_STATS
  #define IF_STATS(x)           x
#else
  #define IF_STATS(x)
#endif

#define SPHERE_UNDECIDED        0x40u   /* initial visibility mask */
#define SPHERE_VISIBLE          0x7fu   /* guaranteed visible */


/*@p-r-i-v-a-t-e---t-y-p-e-s-------------------------------------------------*/

typedef struct {
  VECTOR3f      *strip;         /* pointer to end of triangle strip */
  VERTEXid      head;           /* id of most recent vertex */
  VERTEXid      tail;           /* id of second most recent vertex */
  BOOL          parity;         /* parity of most recent vertex */
  unsigned      triangles;      /* number of triangles in strip */
} VECTORstrip;

/*@p-r-i-v-a-t-e---p-r-o-t-o-t-y-p-e-s---------------------------------------*/

static void
mesh_morph(
  VERTEXbuffer  *buffer,
  const VDinfo  *vdp,
  unsigned      n,
  unsigned      m
);

static void
submesh_morph(
  VECTORstrip   *vsp,
  const VERTEX  *vv,
  const VDinfo  *vdp,
  unsigned      l,
  TRIANGLE(
    VERTEXid    i,
    VERTEXid    j,
    VERTEXid    k
  ),
  FLOAT         za,
  FLOAT         zl,
  FLOAT         zr
);

static void
submesh_morph_visible(
  VECTORstrip   *vsp,
  const VERTEX  *vv,
  const VDinfo  *vdp,
  unsigned      l,
  TRIANGLE(
    VERTEXid    i,
    VERTEXid    j,
    VERTEXid    k
  ),
  FLOAT         za,
  FLOAT         zl,
  FLOAT         zr,
  unsigned      m
);

static void
tstrip_vector_append(
  VECTORstrip   *vsp,
  const VERTEX  *vv,
  VERTEXid      v,
  BOOL          p,
  FLOAT         z
);

static VECTORstrip *
tstrip_vector_begin(
  VERTEXbuffer  *buffer,
  const VERTEX  *vv,
  VERTEXid      v,
  BOOL          p
);

static void
tstrip_vector_end(
  VERTEXbuffer  *buffer,
  VECTORstrip   *vsp,
  const VERTEX  *vv,
  VERTEXid      v
);

static BOOL
vertex_morph(
  FLOAT         *zmp,
  const VERTEX  *vp,
  const VDinfo  *vdp,
  FLOAT         zl,
  FLOAT         zr
);

/*@p-r-i-v-a-t-e---d-a-t-a---------------------------------------------------*/

static VECTORstrip      vector_strip;

/*@p-r-i-v-a-t-e---f-u-n-c-t-i-o-n-s-----------------------------------------*/

/*@mesh_morph................................................................*/

static void
mesh_morph(
  VERTEXbuffer  *buffer,        /* triangle strip vertex buffer */
  const VDinfo  *vdp,           /* view-dependent parameters */
  unsigned      n,              /* maximum number of refinement levels */
  unsigned      m               /* initial visibility mask */
)
{
  const VERTEX  *vv = buffer->vertex;
  const FLOAT   zc = vv[I_C(n / 2)].p[2],
                zsw = vv[I_SW(n / 2)].p[2],
                zse = vv[I_SE(n / 2)].p[2],
                zne = vv[I_NE(n / 2)].p[2],
                znw = vv[I_NW(n / 2)].p[2];
  VECTORstrip   *vsp;

  /*
  ** Top-level function for constructing a morphed mesh.
  */

  vsp = tstrip_vector_begin(buffer, vv, I_SW(n / 2), 1);
  submesh_morph_visible(vsp, vv, vdp, n - 1, ROOT_S(n / 2), zc, zsw, zse, m);
  tstrip_vector_append(vsp, vv, I_SE(n / 2), 0, zse);
  submesh_morph_visible(vsp, vv, vdp, n - 1, ROOT_E(n / 2), zc, zse, zne, m);
  tstrip_vector_append(vsp, vv, I_NE(n / 2), 0, zne);
  submesh_morph_visible(vsp, vv, vdp, n - 1, ROOT_N(n / 2), zc, zne, znw, m);
  tstrip_vector_append(vsp, vv, I_NW(n / 2), 0, znw);
  submesh_morph_visible(vsp, vv, vdp, n - 1, ROOT_W(n / 2), zc, znw, zsw, m);
  tstrip_vector_end(buffer, vsp, vv, I_SW(n / 2));
}

/*@submesh_morph.............................................................*/

static void
submesh_morph(
  VECTORstrip   *vsp,   /* pointer to triangle strip */
  const VERTEX  *vv,    /* vertex array */
  const VDinfo  *vdp,   /* view-dependent parameters */
  unsigned      l,      /* refinement level */
  TRIANGLE(
    VERTEXid    i,      /* triangle apex */
    VERTEXid    j,      /* supplemental vertex #1 */
    VERTEXid    k       /* supplemental vertex #2 */
  ),
  FLOAT         za,     /* elevation of apex */
  FLOAT         zl,     /* elevation of left corner */
  FLOAT         zr      /* elevation of right corner */
)
{
  FLOAT         zm;
  const BOOL    refine = (l != 0) &&
                         vertex_morph(&zm, vv + SPLIT(i, j, k), vdp, zl, zr);

  /*
  ** Recursively refine and morph the mesh.
  */

  if (refine)
    submesh_morph(vsp, vv, vdp, l - 1, CHILD_L(i, j, k), zm, zl, za);
  tstrip_vector_append(vsp, vv, i, l & 1, za);
  if (refine)
    submesh_morph(vsp, vv, vdp, l - 1, CHILD_R(i, j, k), zm, za, zr);
}


/*@submesh_morph_visible.....................................................*/

static void
submesh_morph_visible(
  VECTORstrip   *vsp,   /* pointer to triangle strip */
  const VERTEX  *vv,    /* vertex array */
  const VDinfo  *vdp,   /* view-dependent parameters */
  unsigned      l,      /* refinement level */
  TRIANGLE(
    VERTEXid    i,      /* triangle apex */
    VERTEXid    j,      /* supplemental vertex #1 */
    VERTEXid    k       /* supplemental vertex #2 */
  ),
  FLOAT         za,     /* elevation of apex */
  FLOAT         zl,     /* elevation of left corner */
  FLOAT         zr,     /* elevation of right corner */
  unsigned      m       /* visibility mask */
)
{
  /*
  ** Recursively refine, morph, and cull the mesh.
  */

  if (m == SPHERE_VISIBLE)
    submesh_morph(vsp, vv, vdp, l, TRIANGLE(i, j, k), za, zl, zr);
  else {
    FLOAT       zm;
    const BOOL  refine = (l != 0) &&
                         vertex_morph(&zm, vv + SPLIT(i, j, k), vdp, zl, zr) &&
                         (m = sphere_visible(vv + SPLIT(i, j, k), vdp, m));
    if (refine)
      submesh_morph_visible(
        vsp, vv, vdp, l - 1, CHILD_L(i, j, k), zm, zl, za, m
      );
    tstrip_vector_append(vsp, vv, i, l & 1, za);
    if (refine)
      submesh_morph_visible(
        vsp, vv, vdp, l - 1, CHILD_R(i, j, k), zm, za, zr, m
      );
  }
}

/*@tstrip_vector_append......................................................*/

static void
tstrip_vector_append(
  VECTORstrip   *vsp,   /* pointer to triangle strip */
  const VERTEX  *vv,    /* array of vertices */
  VERTEXid      v,      /* index of vertex to append */
  BOOL          p,      /* parity of vertex */
  FLOAT         z       /* elevation of morphed vertex */
)
{
  if (v != vsp->tail && v != vsp->head) {
    VECTOR3f *vp = vsp->strip;
    if (p == vsp->parity) {
      COPY(vp[0], vp[-2]); /* turn corner; duplicate vertex */
      vp++;
    }
    else {
      vsp->parity = p;
      vsp->tail = vsp->head;
    }
    vsp->head = v;
    ASSIGN(*vp, vv[v].p[0], vv[v].p[1], z); /* append new vertex */
    vsp->strip = ++vp;
    IF_STATS(vsp->triangles++;)
  }
}

/*@tstrip_vector_begin.......................................................*/

static VECTORstrip *
tstrip_vector_begin(
  VERTEXbuffer  *buffer,        /* vertex buffer */
  const VERTEX  *vv,            /* array of vertices */
  VERTEXid      v,              /* index of first vertex in strip */
  BOOL          p               /* parity of first vertex */
)
{
  VECTORstrip   *vsp = &vector_strip;

  vsp->strip = buffer->strip;
  vsp->head = vsp->tail = v;
  vsp->parity = p;
  COPY(*vsp->strip, vv[v].p);
  vsp->strip++;
  COPY(*vsp->strip, vv[v].p);
  vsp->strip++;
  IF_STATS(vsp->triangles = 0;)

  return vsp;
}

/*@tstrip_vector_end.........................................................*/

static void
tstrip_vector_end(
  VERTEXbuffer  *buffer,        /* vertex buffer */
  VECTORstrip   *vsp,           /* pointer to triangle strip */
  const VERTEX  *vv,            /* array of vertices */
  VERTEXid      v               /* index of last vertex in strip */
)
{
  COPY(*vsp->strip, vv[v].p);
  vsp->strip++;
  IF_STATS(buffer->triangles = vsp->triangles;)
  buffer->count = (unsigned)(vsp->strip - (VECTOR3f *)buffer->strip);
}

/*@vertex_morph..............................................................*/

static BOOL
vertex_morph(
  FLOAT         *zmp,   /* pointer to morphed elevation */
  const VERTEX  *vp,    /* pointer to vertex */
  const VDinfo  *vdp,   /* view-dependent parameters */
  FLOAT         zl,     /* elevation of left endpoint of split edge */
  FLOAT         zr      /* elevation of right endpoint of split edge */
)
{
  FLOAT d, dmin, dmax;

  /*
  ** Compute the elevation of the morphed vertex.  The return value indicates
  ** whether the vertex is active or not.
  */

  d = DISTANCE_SQR(vp->p, vdp->viewpoint);
  dmax = SQR(vdp->nu_max * vp->e + vp->r);
  if (dmax > d) {
    dmin = SQR(vdp->nu_min * vp->e + vp->r);
    *zmp = dmin > d
             ? vp->p[2]
             : ((dmax - d) * vp->p[2] + (d - dmin) * 0.5f * (zl + zr)) /
               (dmax - dmin);
    return TRUE;
  }
  else
    return FALSE;
}
