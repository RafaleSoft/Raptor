#ifndef COMMON_H
#define COMMON_H
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

/*@p-u-b-l-i-c---m-a-c-r-o-s-------------------------------------------------*/

/*
** Macros for floating point math (if not in math.h).
*/

#if !WITH_FLOAT_MATH
  #define atanf(x)      ((FLOAT)atan((double)(x)))
  #define fabsf(x)      ((FLOAT)fabs((double)(x)))
  #define floorf(x)     ((FLOAT)floor((double)(x)))
  #define powf(x, y)    ((FLOAT)pow((double)(x), (double)(y)))
  #define sqrtf(x)      ((FLOAT)sqrt((double)(x)))
  #define tanf(x)       ((FLOAT)tan((double)(x)))
#endif

/*
** Boolean macros.
*/

#undef FALSE
#define FALSE   0
#undef TRUE
#define TRUE    1

/*
** Miscellaneous math macros.
*/

#define DEG2RAD(x)      ((3.1415927f / 180.0f) * (x))
#define RAD2DEG(x)      ((180.0f / 3.1415927f) * (x))
#define ISPOW2(x)       (!((x) & ((x) - 1)) && !!(x))
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define SQR(x)          ((x) * (x))

/*
** Operations on 3-vectors.
*/

#define ASSIGN(v, x, y, z)\
  ((void)((v)[0] = (x), (v)[1] = (y), (v)[2] = (z)))

#define COPY(x, y)\
  ((void)((x)[0] = (y)[0], (x)[1] = (y)[1], (x)[2] = (y)[2]))

#define CROSS_PRODUCT(x, y, z)\
  ((void)((x)[0] = (y)[1]*(z)[2] - (y)[2]*(z)[1],\
          (x)[1] = (y)[2]*(z)[0] - (y)[0]*(z)[2],\
          (x)[2] = (y)[0]*(z)[1] - (y)[1]*(z)[0]))

#define DISTANCE(p, q)\
  (sqrtf(DISTANCE_SQR(p, q)))

#define DISTANCE_SQR(p, q)\
  (SQR((p)[0]-(q)[0]) + SQR((p)[1]-(q)[1]) + SQR((p)[2]-(q)[2]))

#define DOT_PRODUCT(x, y)\
  ((x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2])

/*@p-u-b-l-i-c---t-y-p-e-s---------------------------------------------------*/

#ifndef BOOL_DEFINED
#define BOOL_DEFINED
typedef int             BOOL;
#endif

typedef GLfloat         FLOAT;
typedef FLOAT           VECTOR3f[3];
typedef FLOAT           VECTOR4f[4];

#ifdef __cplusplus
}
#endif
#endif
