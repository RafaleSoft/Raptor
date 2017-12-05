#ifndef REFINE_H
#define REFINE_H


/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

#define SPHERE_UNDECIDED        0x40u   /* initial visibility mask */
#define SPHERE_VISIBLE          0x7fu   /* guaranteed visible */


/*@p-u-b-l-i-c---t-y-p-e-s---------------------------------------------------*/

typedef struct 
{
  float         nu;             /* inverse of error tolerance in radians */
  float         nu_min;         /* lower morph parameter */
  float         nu_max;         /* upper morph parameter */
  VECTOR3f      viewpoint;      /* viewpoint */
  VECTOR4f      viewplane[6];   /* view frustum plane equations */
} VDinfo;

typedef struct 
{
  unsigned      tolerance;              /* tolerance in tenths of pixels */
  float         tau;                    /* error tolerance in pixels */
  float         tau_min;                /* min error tolerance for morphing */
  float         tau_max;                /* max error tolerance for morphing */
  VDinfo        vd;                     /* view-dependent parameters */
} LODinfo;


#endif
