#ifndef REFINE_H
#define REFINE_H
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

typedef struct {
  BOOL          cull;           /* perform view culling when set */
  BOOL          morph;          /* perform geomorphing when set */
  FLOAT         nu;             /* inverse of error tolerance in radians */
  FLOAT         nu_min;         /* lower morph parameter */
  FLOAT         nu_max;         /* upper morph parameter */
  VECTOR3f      viewpoint;      /* viewpoint */
  VECTOR4f      viewplane[6];   /* view frustum plane equations */
} VDinfo;

/*@p-u-b-l-i-c---p-r-o-t-o-t-y-p-e-s-----------------------------------------*/

extern void
mesh_construct(
  VERTEXbuffer  *buffer,        /* triangle strip vertex buffer */
  const VDinfo  *vdp,           /* view-dependent parameters */
  unsigned      levels          /* maximum number of refinement levels */
);

#ifdef __cplusplus
}
#endif
#endif
