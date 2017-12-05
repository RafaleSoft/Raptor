#ifndef TERRAIN_H
#define TERRAIN_H


#include "System/Raptor.h"

RAPTOR_NAMESPACE

/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@p-u-b-l-i-c---t-y-p-e-s---------------------------------------------------*/

typedef unsigned VERTEXid;

typedef struct VERTEX_t
{
  GL_COORD_VERTEX	p;
  float				e;              /* error */
  float				r;              /* bounding sphere radius */
} VERTEX;

typedef struct VERTEXbuffer_t
{
  unsigned int  count;          /* number of vertices in buffer */
  unsigned int  triangles;      /* number of triangles in buffer */
  void          *strip;         /* triangle strip vertex buffer */
  const VERTEX  *vertex;        /* 1D array of vertices */
} VERTEXbuffer;

typedef struct VECTORstrip_t
{
  VECTOR3f      *strip;         /* pointer to end of triangle strip */
  VERTEXid      head;           /* id of most recent vertex */
  VERTEXid      tail;           /* id of second most recent vertex */
  bool          parity;         /* parity of most recent vertex */
  unsigned int  triangles;      /* number of triangles in strip */
} VECTORstrip;

typedef struct GEOMETRY_t
{
  bool			morph;
  VERTEXid      vertices;       /* number of vertices */
  unsigned int	levels;         /* number of levels in terrain DAG */
  unsigned int	width;
  unsigned int	height;
} GEOMETRY;


#endif
