/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@i-m-p-o-r-t-e-d---h-e-a-d-e-r-s-------------------------------------------*/

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glut.h>
#include "common.h"
#include "index.h"
#include "terrain.h"
#include "pngio.h"
#include "geoio.h"

/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#define LEVEL_MIN        2
#define LEVEL_MAX       (CHAR_BIT * sizeof(VERTEXid) - 2)
#define FLT_INF         ((FLOAT)HUGE_VAL)

/*@p-r-i-v-a-t-e---p-r-o-t-o-t-y-p-e-s---------------------------------------*/

static VERTEX **
mesh_construct(
  VECTOR3f              box[2], /* bounding box min, max */
  void                  *image, /* height field data */
  unsigned              size_x, /* height field width */
  unsigned              size_y, /* height field height */
  unsigned              depth,  /* height field depth */
  unsigned              n,      /* grid width/height */
  const VECTOR3f        res     /* xyz resolution */
);

static void
mesh_lod_compute(
  VERTEX        **vv,   /* 2D vertex array */
  unsigned      levels  /* number of refinement levels */
);

static unsigned
mesh_reindex(
  VERTEX        **vv,   /* 2D vertex array */
  unsigned      levels  /* number of refinement levels */
);

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
);
#endif

static void
vertex_lod_compute(
  VERTEX        **vv,   /* 2D vertex array */
  unsigned      i,      /* column index */
  unsigned      j,      /* row index */
  int           di,     /* non-negative col offset to bisected edge endpoint */
  int           dj,     /* row offset to bisected edge endpoint */
  unsigned      n       /* array width/height (zero for leaves) */
);

/*@p-r-i-v-a-t-e---d-a-t-a---------------------------------------------------*/

#if INDEX == INDEX_INTERLEAVED_QUADTREES
static VERTEX   null_vertex = { { 0.0f, 0.0f, 0.0f}, 0.0f, 0.0f };
#endif

/*@p-r-i-v-a-t-e---f-u-n-c-t-i-o-n-s-----------------------------------------*/

/*@mesh_construct............................................................*/

static VERTEX **
mesh_construct(
  VECTOR3f              box[2], /* bounding box min, max */
  void                  *image, /* height field data */
  unsigned              size_x, /* height field width */
  unsigned              size_y, /* height field height */
  unsigned              depth,  /* height field depth */
  unsigned              n,      /* grid width/height */
  const VECTOR3f        res     /* xyz resolution */
)
{
  VERTEX                **vv;
  unsigned char         *cp = image;
  unsigned short        *sp = image;
  unsigned              i, j;

  /*
  ** Compute vertex positions and bounding box from height field and grid
  ** resolution.
  */

  if (!(vv = malloc(n * sizeof(*vv))) ||
      !(*vv = malloc(SQR(n) * sizeof(**vv)))) {
    fprintf(stderr, "mesh allocation failed\n");
    exit(EXIT_FAILURE);
  }
  for (j = 1; j != n; j++)
    vv[j] = vv[j - 1] + n;

  box[0][0] = -res[0] * 0.5f * (size_x - 1);
  box[0][1] = -res[1] * 0.5f * (size_y - 1);
  box[0][2] = +FLT_MAX;
  box[1][0] = +res[0] * 0.5f * (size_x - 1);
  box[1][1] = +res[1] * 0.5f * (size_y - 1);
  box[1][2] = -FLT_MAX;

  for (j = 0; j != size_y; j++)
    for (i = 0; i != size_x; i++) {
      vv[j][i].p[0] = res[0] * (i - 0.5f * (size_x - 1));
      vv[j][i].p[1] = res[1] * (j - 0.5f * (size_y - 1));
      vv[j][i].p[2] = res[2] * (depth == 1 ? (FLOAT)*cp++ : (FLOAT)*sp++);
      if (vv[j][i].p[2] < box[0][2])
        box[0][2] = vv[j][i].p[2];
      else if (vv[j][i].p[2] > box[1][2])
        box[1][2] = vv[j][i].p[2];
    }

  return vv;
}

/*@mesh_lod_compute..........................................................*/

static void
mesh_lod_compute(
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
        vertex_lod_compute(vv, i, j, 0, a, s);
        vertex_lod_compute(vv, j, i, a, 0, s);
      }
    }
    /*
    ** Process level in white quadtree.
    */
    for (j = a; j < n; c = -c, j += b)
      for (i = a; i < n; c = -c, i += b)
        vertex_lod_compute(vv, i, j, a, c, n);
  }

  /*
  ** Lock center and corner vertices.
  */
  vv[0][0].e = vv[n][0].e = vv[0][n].e = vv[n][n].e = vv[m][m].e = FLT_INF;
  vv[0][0].r = vv[n][0].r = vv[0][n].r = vv[n][n].r = vv[m][m].r = FLT_INF;
}

/*@mesh_reindex..............................................................*/

/*ARGSUSED1*/
static unsigned
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

/*@submesh_reindex...........................................................*/

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

/*@vertex_lod_compute........................................................*/

static void
vertex_lod_compute(
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
  FLOAT         r;
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
        vp->e = MAX(vp->e, cp->e);
        r = DISTANCE(vp->p, cp->p) + cp->r;
        vp->r = MAX(vp->r, r);
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

/*@m-a-i-n---f-u-n-c-t-i-o-n-------------------------------------------------*/

int
main(
  int   argc,
  char  *argv[]
)
{
  const char    *infile, *outfile;
  void          *image;
  VERTEX        **vv;
  VECTOR3f      box[2], resolution;
  size_t        ram;
  unsigned      count, i, j, levels, n, size_x, size_y, size_z, depth;

  fprintf(stderr, "\nSOAR Data Set Builder v1.11\n");
  fprintf(stderr, "Peter Lindstrom and Valerio Pascucci\n");
  fprintf(stderr, "UCRL-CODE-2002-023\n");
  fprintf(stderr, "Copyright (c) 2002 University of California, LLNL\n\n");

  if (argc != 6) {
    fprintf(
      stderr,
      "Usage: %s <x-res> <y-res> <z-res> <png file> <geo file>\n",
      argv[0]
    );
    exit(EXIT_FAILURE);
  }

  if (sscanf(argv[1], "%f", resolution + 0) != 1 ||
      sscanf(argv[2], "%f", resolution + 1) != 1 ||
      sscanf(argv[3], "%f", resolution + 2) != 1) {
    fprintf(stderr, "invalid resolution\n");
    exit(EXIT_FAILURE);
  }

  infile = argv[4];
  outfile = argv[5];

  /*
  ** Read PNG file.
  */

  if (!(image = png_read(&size_x, &size_y, &size_z, &depth, infile, FALSE))) {
    fprintf(stderr, "can't read PNG image '%s'\n", infile);
    exit(EXIT_FAILURE);
  }
  if (size_z != 1) {
    fprintf(stderr, "PNG image has more than one channel\n");
    exit(EXIT_FAILURE);
  }

  /*
  ** Compute number of refinement levels.
  */

  for (levels = 0; (1u << levels) + 1 < MAX(size_x, size_y); levels++);
  n = (1u << levels) + 1;
  levels *= 2;
  if (levels < LEVEL_MIN) {
    fprintf(stderr, "height field too small\n");
    exit(EXIT_FAILURE);
  }
  else if (levels > LEVEL_MAX) {
    fprintf(stderr, "height field too large\n");
    exit(EXIT_FAILURE);
  }

  ram = sizeof(VERTEX) * LINEAR_COUNT(levels);
#if INDEX == INDEX_INTERLEAVED_QUADTREES
  ram += sizeof(VERTEX) * IQ_COUNT(levels);
#endif
  fprintf(
    stderr,
    "estimated memory requirements: %u MB\n",
    (unsigned)((0xfffffu + ram) / 0x100000u)
  );

  /*
  ** Initialize vertex positions.
  */

  vv = mesh_construct(box, image, size_x, size_y, depth, n, resolution);

  /*
  ** Duplicate vertices outside height field if necessary.
  */

  if (size_x < n || size_y < n) {
    for (j = 0; j != size_y; j++)
      for (i = size_x; i != n; i++)
        COPY(vv[j][i].p, vv[j][size_x - 1].p);
    for (j = size_y; j != n; j++)
      for (i = 0; i != size_x; i++)
        COPY(vv[j][i].p, vv[size_y - 1][i].p);
    for (j = size_y; j != n; j++)
      for (i = size_x; i != n; i++)
        COPY(vv[j][i].p, vv[size_y - 1][size_x - 1].p);
  }

  /*
  ** Compute error and radius.
  */

  mesh_lod_compute(vv, levels);

  /*
  ** Change data layout if necessary.
  */

  count = mesh_reindex(vv, levels);

  /*
  ** Write GEO file.
  */

  if (!geo_write(outfile, INDEX, *vv, (void *)box, count, levels)) {
    fprintf(stderr, "error writing '%s'\n", outfile);
    exit(EXIT_FAILURE);
  }

  return 0;
}
