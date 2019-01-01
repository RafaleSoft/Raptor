#ifndef GEOIO_H
#define GEOIO_H
#ifdef __cplusplus
extern "C" {
#endif

/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@p-u-b-l-i-c---p-r-o-t-o-t-y-p-e-s-----------------------------------------*/

extern VERTEX *
geo_read(
  VECTOR3f      box[2],         /* bounding box min, max */
  unsigned      *count,         /* number of vertices including gaps */
  unsigned      *levels,        /* number of refinement levels */
  const char    *filename,      /* path to GEO file */
  unsigned      indexing        /* indexing scheme */
);

extern BOOL
geo_write(
  const char            *filename,      /* path to GEO file */
  unsigned              indexing,       /* indexing scheme */
  const VERTEX          *vv,            /* 1D array of vertices */
  const VECTOR3f        box[2],         /* bounding box min, max */
  unsigned              count,          /* number of vertices including gaps */
  unsigned              levels          /* number of refinement levels */
);

#ifdef __cplusplus
}
#endif
#endif
