#ifndef TERRAIN_H
#define TERRAIN_H
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

/*@p-u-b-l-i-c---t-y-p-e-s---------------------------------------------------*/

typedef unsigned VERTEXid;

typedef struct {
  VECTOR3f      p;              /* vertex position */
  FLOAT         e;              /* error */
  FLOAT         r;              /* bounding sphere radius */
} VERTEX;

typedef struct {
  BOOL          indexed;        /* indexed triangle strip when set */
  unsigned      count;          /* number of vertices in buffer */
  unsigned      capacity;       /* buffer capacity in number of vertices */
  unsigned      triangles;      /* number of triangles in buffer */
  void          *strip;         /* triangle strip vertex buffer */
  const VERTEX  *vertex;        /* 1D array of vertices */
} VERTEXbuffer;

typedef struct {
  VERTEX        *vertex;        /* 1D array of vertices */
  VERTEXid      vertices;       /* number of vertices */
  unsigned      levels;         /* number of levels in terrain DAG */
  VECTOR3f      box[2];         /* bounding box min, max */
} GEOMETRY;

typedef struct {
  void          *image;         /* texture image data */
  unsigned      size_x;         /* image width */
  unsigned      size_y;         /* image height */
  unsigned      size_z;         /* image depth */
} TEXTURE;

typedef struct {
  GEOMETRY      geometry;       /* terrain geometry */
  TEXTURE       texture;        /* terrain texture */
} TERRAIN;

#ifdef __cplusplus
}
#endif
#endif
