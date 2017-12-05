/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@i-m-p-o-r-t-e-d---h-e-a-d-e-r-s-------------------------------------------*/

#include <glut.h>
#include "common.h"
#include "index.h"
#include "terrain.h"
#include "refine.h"

/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#if WITH_STATS
  #define IF_STATS(x)           x
#else
  #define IF_STATS(x)
#endif

#define SPHERE_UNDECIDED        0x40u   /* initial visibility mask */
#define SPHERE_VISIBLE          0x7fu   /* guaranteed visible */

/*@p-r-i-v-a-t-e---p-r-o-t-o-t-y-p-e-s---------------------------------------*/

static void
mesh_refine(
  VERTEXbuffer  *buffer,
  const VDinfo  *vdp,
  unsigned      n,
  unsigned      m
);

static unsigned
sphere_visible(
  const VERTEX  *vp,
  const VDinfo  *vdp,
  unsigned      mask
);

static VERTEXid *
submesh_refine(
  VERTEXid      *vip,
  const VERTEX  *vv,
  const VDinfo  *vdp,
  TRIANGLE(
    VERTEXid    i,
    VERTEXid    j,
    VERTEXid    k
  ),
  unsigned      l
);

static VERTEXid *
submesh_refine_visible(
  VERTEXid      *vip,
  const VERTEX  *vv,
  const VDinfo  *vdp,
  TRIANGLE(
    VERTEXid    i,
    VERTEXid    j,
    VERTEXid    k
  ),
  unsigned      l,
  unsigned      m
);

static VERTEXid *
tstrip_index_append(
  VERTEXid      *vip,
  VERTEXid      v,
  BOOL          p
);

static VERTEXid *
tstrip_index_begin(
  VERTEXbuffer  *buffer,
  VERTEXid      v,
  BOOL          p
);

static void
tstrip_index_end(
  VERTEXbuffer  *buffer,
  VERTEXid      *vip,
  VERTEXid      v
);

static BOOL
vertex_active(
  const VERTEX  *vp,
  const VDinfo  *vdp
);

/*@i-m-p-o-r-t-e-d---s-o-u-r-c-e---f-i-l-e-s---------------------------------*/

/*
** For readability, the functions for geomorphing are placed in a separate
** source file morph.c.  For almost every function in refine.c, there is a
** corresponding similar function in morph.c.  Because both source files
** access the statically defined (i.e. private) function for view culling,
** however, they must be compiled as a single source file, which is the
** reason for including morph.c here.
*/

#include "morph.c"

/*@p-r-i-v-a-t-e---f-u-n-c-t-i-o-n-s-----------------------------------------*/

/*@mesh_refine...............................................................*/

static void
mesh_refine(
  VERTEXbuffer  *buffer,        /* triangle strip vertex buffer */
  const VDinfo  *vdp,           /* view-dependent parameters */
  unsigned      n,              /* maximum number of refinement levels */
  unsigned      m               /* initial visibility mask */
)
{
  const VERTEX  *vv = buffer->vertex;
  VERTEXid      *vip;

  /*
  ** Top-level function for constructing an indexed mesh.
  */

  vip = tstrip_index_begin(buffer, I_SW(n / 2), 1);
  vip = submesh_refine_visible(vip, vv, vdp, n - 1, ROOT_S(n / 2), m);
  vip = tstrip_index_append(vip, I_SE(n / 2), 0);
  vip = submesh_refine_visible(vip, vv, vdp, n - 1, ROOT_E(n / 2), m);
  vip = tstrip_index_append(vip, I_NE(n / 2), 0);
  vip = submesh_refine_visible(vip, vv, vdp, n - 1, ROOT_N(n / 2), m);
  vip = tstrip_index_append(vip, I_NW(n / 2), 0);
  vip = submesh_refine_visible(vip, vv, vdp, n - 1, ROOT_W(n / 2), m);
  tstrip_index_end(buffer, vip, I_SW(n / 2));
}

/*@sphere_visible............................................................*/

static unsigned
sphere_visible(
  const VERTEX  *vp,    /* vertex */
  const VDinfo  *vdp,   /* view-dependent parameters */
  unsigned      mask    /* visibility mask */
)
{
  const FLOAT   *p = vp->p, r = vp->r;
  FLOAT         d;
  unsigned      i;

  /*
  ** Compare the radius of the vertex's bounding sphere against the signed
  ** distance to each plane of the view volume.  If the sphere is completely
  ** on the exterior side of a plane, it is invisible.  Otherwise, if it is
  ** entirely on the interior side, then a flag is set for this plane, and no
  ** further tests are made between this plane and the vertex's descendants
  ** since the bounding spheres are nested.
  */

  for (i = 0; i != 6; i++)
    if (!(mask & (1u << i))) {
      d = DOT_PRODUCT(p, vdp->viewplane[i]) + vdp->viewplane[i][3];
      if (d > +r)
        return 0;               /* completely outside view volume */
      if (d < -r)
        mask |= 1u << i;        /* completely on interior side of view plane */
    }

  return mask;
}

/*@submesh_refine............................................................*/

static VERTEXid *
submesh_refine(
  VERTEXid      *vip,   /* pointer to end of triangle strip */
  const VERTEX  *vv,    /* vertex array */
  const VDinfo  *vdp,   /* view-dependent parameters */
  unsigned      l,      /* refinement level */
  TRIANGLE(
    VERTEXid    i,      /* triangle apex */
    VERTEXid    j,      /* supplemental vertex #1 */
    VERTEXid    k       /* supplemental vertex #2 */
  )
)
{
  const BOOL refine = (l != 0) && vertex_active(vv + SPLIT(i, j, k), vdp);

  /*
  ** Recursively refine the mesh.  Since the refinement condition is the
  ** same for both branches and can be somewhat expensive to evaluate,
  ** it is evaluated and tested *before* making the recursive calls.
  */

  if (refine)
    vip = submesh_refine(vip, vv, vdp, l - 1, CHILD_L(i, j, k));
  vip = tstrip_index_append(vip, i, l & 1);
  if (refine)
    vip = submesh_refine(vip, vv, vdp, l - 1, CHILD_R(i, j, k));

  return vip;
}

/*@submesh_refine_visible....................................................*/

static VERTEXid *
submesh_refine_visible(
  VERTEXid      *vip,   /* pointer to end of triangle strip */
  const VERTEX  *vv,    /* vertex array */
  const VDinfo  *vdp,   /* view-dependent parameters */
  unsigned      l,      /* refinement level */
  TRIANGLE(
    VERTEXid    i,      /* triangle apex */
    VERTEXid    j,      /* supplemental vertex #1 */
    VERTEXid    k       /* supplemental vertex #2 */
  ),
  unsigned      m       /* visibility mask */
)
{
  /*
  ** If the sphere is contained inside the view volume, then transition to
  ** submesh_refine() and make no further view culling tests.  Otherwise,
  ** continue culling.
  */

  if (m == SPHERE_VISIBLE)
    return submesh_refine(vip, vv, vdp, l, TRIANGLE(i, j, k));
  else {
    const BOOL refine = (l != 0) && vertex_active(vv + SPLIT(i, j, k), vdp) &&
                        (m = sphere_visible(vv + SPLIT(i, j, k), vdp, m));
    if (refine)
      vip = submesh_refine_visible(vip, vv, vdp, l - 1, CHILD_L(i, j, k), m);
    vip = tstrip_index_append(vip, i, l & 1);
    if (refine)
      vip = submesh_refine_visible(vip, vv, vdp, l - 1, CHILD_R(i, j, k), m);
    return vip;
  }
}

/*@tstrip_index_append.......................................................*/

static VERTEXid *
tstrip_index_append(
  VERTEXid      *vip,   /* pointer to end of triangle strip */
  VERTEXid      v,      /* index of vertex to append */
  BOOL          p       /* parity of vertex */
)
{
  const VERTEXid        tail = vip[-2], head = vip[-1], parity = vip[0];
  IF_STATS(
    const VERTEXid      triangles = vip[1];
  )

  /*
  ** Add vertex to end of triangle strip vertex buffer.
  */

  if (v != tail && v != head) {
    if ((VERTEXid)p == parity)
      *vip++ = tail;    /* turn corner; duplicate vertex */
    *vip++ = v;         /* append new vertex */
    *vip = (VERTEXid)p; /* store parity here for easy access */
    IF_STATS(vip[1] = triangles + 1;)
  }

  return vip;
}

/*@tstrip_index_begin........................................................*/

static VERTEXid *
tstrip_index_begin(
  VERTEXbuffer  *buffer,        /* triangle strip vertex buffer */
  VERTEXid      v,              /* first vertex in strip */
  BOOL          p               /* parity of first vertex */
)
{
  VERTEXid      *vip;

  /*
  ** Initialize triangle strip with two copies of the first vertex.
  ** The first copy should be skipped over during rendering.
  */

  vip = buffer->strip;
  *vip++ = v;
  *vip++ = v;
  *vip = (VERTEXid)p;   /* store parity here for easy access */
  IF_STATS(vip[1] = 0;) /* store triangle count here */

  return vip;
}

/*@tstrip_index_end..........................................................*/

static void
tstrip_index_end(
  VERTEXbuffer  *buffer,        /* triangle strip vertex buffer */
  VERTEXid      *vip,           /* pointer to end of vertex buffer */
  VERTEXid      v               /* last vertex in strip */
)
{
  *vip++ = v;
  IF_STATS(buffer->triangles = *vip;)
  buffer->count = (unsigned)(vip - (VERTEXid *)buffer->strip);
}

/*@vertex_active.............................................................*/

static BOOL
vertex_active(
  const VERTEX  *vp,    /* pointer to vertex */
  const VDinfo  *vdp    /* view-dependent parameters */
)
{
  return SQR(vdp->nu * vp->e + vp->r) > DISTANCE_SQR(vp->p, vdp->viewpoint);
}

/*@p-u-b-l-i-c---f-u-n-c-t-i-o-n-s-------------------------------------------*/

/*@mesh_construct............................................................*/

void
mesh_construct(
  VERTEXbuffer  *buffer,        /* triangle strip vertex buffer */
  const VDinfo  *vdp,           /* view-dependent parameters */
  unsigned      levels          /* maximum number of refinement levels */
)
{
  const unsigned mask = vdp->cull ? SPHERE_UNDECIDED : SPHERE_VISIBLE;

  /*
  ** Construct adaptive mesh as a single triangle strip.  NOTE: It is
  ** assumed that enough memory has been allocated for the vertex buffer.
  ** For performance reasons, no tests are made to prevent buffer overflows
  ** *during* stripping.  Instead, the buffer load should be checked
  ** periodically and the buffer must be resized as needed to accommodate
  ** even abrupt changes in mesh complexity.  This is not a failsafe
  ** approach, and crashes due to overflow are possible albeit unlikely.
  */

  if ((buffer->indexed = !vdp->morph))
    mesh_refine(buffer, vdp, levels, mask);
  else
    mesh_morph(buffer, vdp, levels, mask);
}
