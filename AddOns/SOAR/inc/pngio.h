#ifndef PNGIO_H
#define PNGIO_H
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

extern void *
png_read(
  unsigned      *size_x,        /* image width */
  unsigned      *size_y,        /* image height */
  unsigned      *size_z,        /* number of channels */
  unsigned      *depth,         /* datum depth in bytes */
  const char    *filename,      /* path */
  BOOL          byteize         /* convert to 8-bit depth when set */
);

#ifdef __cplusplus
}
#endif
#endif
