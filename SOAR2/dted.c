
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "common.h"
#include "terrain.h"
#include "index.h"

/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#define FLT_INF         ((float)HUGE_VAL)

#if INDEX == INDEX_INTERLEAVED_QUADTREES
static VERTEX   null_vertex = { { 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f };
#endif


void
dted_vertex_lod_compute(
  VERTEX        **vv,   /* 2D vertex array */
  unsigned      i,      /* column index */
  unsigned      j,      /* row index */
  int           di,     /* non-negative col offset to bisected edge endpoint */
  int           dj,     /* row offset to bisected edge endpoint */
  unsigned      n       /* one less array width/height (zero for leaves) */
)
{
  VERTEX        * const vp = &vv[j][i];
  const VERTEX  *cp;
  float         r;
  unsigned      k;

  /*
  ** Compute actual error and initialize radius to zero.  The error is
  ** simply the vertical difference between the vertex and the bisected
  ** edge, i.e. the error between two consecutive levels of refinement.
  ** This object-space error can be replaced with any measure of error,
  ** as long as the nesting step below is performed.
  */

  vp->e = fabsf(vp->p[2] - 0.5f * (vv[j-dj][i-di].p[2] + vv[j+dj][i+di].p[2]));
  vp->r = 0.0f;

  /*
  ** If the vertex is not a leaf node, ensure that the error and radius are
  ** nested using information from its four children.  Note that the offsets
  ** (+di, +dj) and (-di, -dj) from (i, j) initially get us to the two
  ** vertices of the bisected edge.  By "rotating" (di, dj) 45 degrees (in
  ** a topological sense), we arrive at one of the children of (i, j)
  ** (assuming we're not on a boundary).  Successive 90-degree rotations
  ** then allow us to visit all four children.
  */

  if (n) {
    /*
    ** di' = (di - dj) / 2
    ** dj' = (di + dj) / 2
    */
    dj = (di + dj) / 2;
    di -= dj;
    k = 4;
    do {
      /*
      ** Test whether child vertex exists.
      */
      if ((i != 0 || di >= 0) && (i != n || di <= 0) &&
          (j != 0 || dj >= 0) && (j != n || dj <= 0)) {
        /*
        ** Inflate error and radius as needed.
        */
        cp = &vv[j + dj][i + di];
		vp->e = (vp->e > cp->e ? vp->e : cp->e);
        r = DISTANCE(vp->p, cp->p) + cp->r;
		vp->r = (vp->r > r ? vp->r : r);
      }
      /*
      ** di' = -dj
      ** dj' = +di
      */
      dj += di;
      di -= dj;
      dj += di;
    } while (--k);
  }
}


void
dted_mesh_lod_compute(
  VERTEX        **vv,   /* 2D vertex array */
  unsigned      levels  /* number of refinement levels */
)
{
  const unsigned        n = 1 << (levels / 2), m = n / 2;
  unsigned              i, j, s;
  int                   a, b, c;

  /*
  ** Compute error and radius bottom-up, level-by-level.  This is done
  ** assuming a standard 2D matrix layout of the data, since this layout
  ** allows easy access to all four children of a vertex.  Depending on
  ** the choice of indexing scheme, the data may later have to be rearranged.
  */

  for (a = c = 1, b = 2, s = 0; (unsigned)a != n; a = c = b, b *= 2, s = n) {
    /*
    ** Process level in black quadtree.
    */
    for (j = a; j < n; j += b) {
      for (i = 0; i <= n; i += b) {
        dted_vertex_lod_compute(vv, i, j, 0, a, s);
        dted_vertex_lod_compute(vv, j, i, a, 0, s);
      }
    }
    /*
    ** Process level in white quadtree.
    */
    for (j = a; j < n; c = -c, j += b)
      for (i = a; i < n; c = -c, i += b)
        dted_vertex_lod_compute(vv, i, j, a, c, n);
  }

  /*
  ** Lock center and corner vertices.
  */
  vv[0][0].e = vv[n][0].e = vv[0][n].e = vv[n][n].e = vv[m][m].e = FLT_INF;
  vv[0][0].r = vv[n][0].r = vv[0][n].r = vv[n][n].r = vv[m][m].r = FLT_INF;
}


#if INDEX == INDEX_INTERLEAVED_QUADTREES
static void
submesh_reindex(
  VERTEX        *vvo,   /* rearranged data */
  const VERTEX  *vvi,   /* linearly indexed data */
  unsigned      level,  /* refinement level */
  VERTEXid      p,      /* DAG parent and triangle apex (quadtree index) */
  VERTEXid      c,      /* DAG child (quadtree index) */
  VERTEXid      i,      /* triangle apex (linear index) */
  VERTEXid      j,      /* base vertex #1 (linear index) */
  VERTEXid      k       /* base vertex #2 (linear index) */
)
{
  if (level) {
    /*
    ** Copy corresponding vertex from linear layout to quadtree layout.
    */
    vvo[p] = vvi[i];
    submesh_reindex(
      vvo, vvi, level - 1, IQ_CHILD_L(p, c), LINEAR_CHILD_L(i, j, k)
    );
    submesh_reindex(
      vvo, vvi, level - 1, IQ_CHILD_R(p, c), LINEAR_CHILD_R(i, j, k)
    );
  }
}
#endif


unsigned
mesh_reindex(
  VERTEX        **vv,   /* 2D vertex array */
  unsigned      levels  /* number of refinement levels */
)
{
  unsigned      count = 0;

#if INDEX == INDEX_LINEAR
  /*
  ** The data is already in linear order, so nothing needs to be done.
  */

  count = LINEAR_COUNT(levels);
#elif INDEX == INDEX_INTERLEAVED_QUADTREES
  VERTEX                *vp;
  const unsigned        l = levels / 2, n = 1 << l, m = n / 2;
  unsigned              i;

  /*
  ** Rearrange data to interleaved quadtree layout.  This is done by
  ** traversing the 2D linear array and the quadtree array in parallel in
  ** recursive refinement order, since this is the only (simple) way we
  ** know how to map quadtree array indices to actual vertex locations.
  ** First visit entire quadtree array to ensure that ghost vertices
  ** (holes) are initialized.
  */

  count = IQ_COUNT(levels);
  if (!(vp = malloc(count * sizeof(*vp)))) {
    fprintf(stderr, "mesh allocation failed\n");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i != count; i++)
    vp[i] = null_vertex;

  submesh_reindex(
    vp, *vv, levels, ROOT_S(l),
    LINEAR_INDEX(m, m, l), LINEAR_INDEX(0, 0, l), LINEAR_INDEX(n, 0, l)
  );
  submesh_reindex(
    vp, *vv, levels, ROOT_E(l),
    LINEAR_INDEX(m, m, l), LINEAR_INDEX(n, 0, l), LINEAR_INDEX(n, n, l)
  );
  submesh_reindex(
    vp, *vv, levels, ROOT_N(l),
    LINEAR_INDEX(m, m, l), LINEAR_INDEX(n, n, l), LINEAR_INDEX(0, n, l)
  );
  submesh_reindex(
    vp, *vv, levels, ROOT_W(l),
    LINEAR_INDEX(m, m, l), LINEAR_INDEX(0, n, l), LINEAR_INDEX(0, 0, l)
  );

  /*
  ** Copy corners.
  */

  vp[I_SW(l)] = vv[0][0];
  vp[I_SE(l)] = vv[0][n];
  vp[I_NE(l)] = vv[n][n];
  vp[I_NW(l)] = vv[n][0];

  free(*vv);
  *vv = vp;
#endif

  return count;
}

