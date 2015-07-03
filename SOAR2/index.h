#ifndef INDEX_H
#define INDEX_H


/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@p-u-b-l-i-c---m-a-c-r-o-s-------------------------------------------------*/

#define INDEX_LINEAR                    0
#define INDEX_INTERLEAVED_QUADTREES     1
#define INDEX_EMBEDDED_QUADTREES        2

#define LINEAR_INDEX(i, j, m)   ((i) + (j) + ((j) << (m)))
#define LINEAR_SPLIT(i, j, k)   (((j) + (k)) / 2)
#define LINEAR_CHILD_L(i, j, k) LINEAR_SPLIT(i, j, k), j, i
#define LINEAR_CHILD_R(i, j, k) LINEAR_SPLIT(i, j, k), i, k
#define LINEAR_COUNT(n)         SQR((1 << ((n) / 2)) + 1)

#define IQ_SPLIT(i, j)          (j)
#define IQ_CHILD_L(i, j)        IQ_SPLIT(i, j), (4*(i)-7 + ((2*(i)+(j)+2) & 3))
#define IQ_CHILD_R(i, j)        IQ_SPLIT(i, j), (4*(i)-7 + ((2*(i)+(j)+3) & 3))
#define IQ_COUNT(n)             (4 + 5 * ((1 << (n)) - 1) / 3)

#ifndef INDEX
  #error no indexing scheme specified
#elif INDEX == INDEX_LINEAR
  /*
  ** Standard row-major (linear) matrix layout.
  */
  #define I_SW(m)               LINEAR_INDEX(0 << (m), 0 << (m), m)
  #define I_SE(m)               LINEAR_INDEX(1 << (m), 0 << (m), m)
  #define I_NE(m)               LINEAR_INDEX(1 << (m), 1 << (m), m)
  #define I_NW(m)               LINEAR_INDEX(0 << (m), 1 << (m), m)
  #define I_C(m)                LINEAR_INDEX(1 << ((m) - 1), 1 << ((m) - 1), m)
  #define ROOT_S(m)             I_C(m), I_SW(m), I_SE(m)
  #define ROOT_E(m)             I_C(m), I_SE(m), I_NE(m)
  #define ROOT_N(m)             I_C(m), I_NE(m), I_NW(m)
  #define ROOT_W(m)             I_C(m), I_NW(m), I_SW(m)
  #define TRIANGLE(i, j, k)     i, j, k
  #define SPLIT(i, j, k)        LINEAR_SPLIT(i, j, k)
  #define CHILD_L(i, j, k)      LINEAR_CHILD_L(i, j, k)
  #define CHILD_R(i, j, k)      LINEAR_CHILD_R(i, j, k)
#elif INDEX == INDEX_INTERLEAVED_QUADTREES
  /*
  ** Interleaved quadtrees (2/3 overhead).
  */
  #define I_SW(m)               0
  #define I_SE(m)               1
  #define I_NE(m)               2
  #define I_NW(m)               3
  #define I_C(m)                4
  #define I_W(m)                5
  #define I_S(m)                6
  #define I_E(m)                7
  #define I_N(m)                8
  #define ROOT_S(m)             I_C(m), I_S(m)
  #define ROOT_E(m)             I_C(m), I_E(m)
  #define ROOT_N(m)             I_C(m), I_N(m)
  #define ROOT_W(m)             I_C(m), I_W(m)
  #define TRIANGLE(i, j, k)     i, j
  #define SPLIT(i, j, k)        IQ_SPLIT(i, j)
  #define CHILD_L(i, j, k)      IQ_CHILD_L(i, j)
  #define CHILD_R(i, j, k)      IQ_CHILD_R(i, j)
#elif INDEX == INDEX_EMBEDDED_QUADTREES
  /*
  ** Embedded quadtrees (1/3 overhead).
  */
  #error embedded quadtrees not implemented yet
#else
  #error invalid indexing scheme
#endif


#endif
