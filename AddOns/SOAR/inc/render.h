#ifndef RENDER_H
#define RENDER_H
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

extern void
stats_render(
  FLOAT         tau_min,
  FLOAT         tau_max,
  unsigned      triangles,
  unsigned      fps
);

extern void
string_render(
  const char    *string,
  GLint         x,
  GLint         y
);

extern void
terrain_render_basic(
  const VERTEXbuffer    *buffer
);

extern void
terrain_render_lighted(
  const VERTEXbuffer    *buffer
);

extern void
view_volume_render(
  unsigned      size_x,
  unsigned      size_y,
  GLdouble      size
);

#ifdef __cplusplus
}
#endif
#endif
