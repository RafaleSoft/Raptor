/*
** SOAR Terrain Engine v1.11.
** Peter Lindstrom and Valerio Pascucci.
** UCRL-CODE-2002-023.
** Copyright (c) 2002 University of California, LLNL.
** See "LICENSE" for details.
*/

/*@i-m-p-o-r-t-e-d---h-e-a-d-e-r-s-------------------------------------------*/
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL\gl.h>


#include "common.h"
#include "terrain.h"


/*@p-r-i-v-a-t-e---m-a-c-r-o-s-----------------------------------------------*/

#define NORMAL(v, p0, p1, p2)\
  (void)(\
    (v)[0] = ((p1)[1] - (p0)[1]) * ((p2)[2] - (p0)[2]) -\
             ((p1)[2] - (p0)[2]) * ((p2)[1] - (p0)[1]),\
    (v)[1] = ((p1)[2] - (p0)[2]) * ((p2)[0] - (p0)[0]) -\
             ((p1)[0] - (p0)[0]) * ((p2)[2] - (p0)[2]),\
    (v)[2] = ((p1)[0] - (p0)[0]) * ((p2)[1] - (p0)[1]) -\
             ((p1)[1] - (p0)[1]) * ((p2)[0] - (p0)[0])\
  )

/*@p-r-i-v-a-t-e---p-r-o-t-o-t-y-p-e-s---------------------------------------*/

static void
tstrip_index_render_lighted(
  const VERTEX          *vv,
  const VERTEXid        *vip,
  const VERTEXid        * const end
);


/*@p-r-i-v-a-t-e---f-u-n-c-t-i-o-n-s-----------------------------------------*/

/*@tstrip_index_render_lighted...............................................*/
static float *pNormals = NULL;
static unsigned int *indexes = NULL;

static void
tstrip_index_render_lighted(
  const VERTEX          *vv,
  const VERTEXid        *vip,
  const VERTEXid        * const end
)
{
  const GLfloat         *p0, *p1, *p2;
  bool                  flip = false;
  unsigned int			indexPos = 0;
  unsigned int			ip0 = 0, ip1 = 0, ip2 = 0;
  unsigned int			j=0;
  float					n =0;
  const VERTEXid*		begin = vip; 

  memset(pNormals,0,4*30000*sizeof(float));

  p0 = p1 = vv[*vip].p;
  ip0 = ip1 = *vip;
  do {

    ip2 = *vip;
	indexPos++;

    //
    // If triangle is not degenerate, compute its normal.
    //
    if ((p2 = vv[*vip++].p) != p0) 
	{
      VECTOR3f v;
      NORMAL(v, flip ? p1 : p0, flip ? p0 : p1, p2);

	  v[2] *= 5.0f;
	  n = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	  if (n != 0)
	  {
		v[0] /= n;
		v[1] /= n;
		v[2] /= n;

		pNormals[4*ip0] += v[0];
		pNormals[4*ip0+1] += v[1];
		pNormals[4*ip0+2] += v[2];
		pNormals[4*ip0+3] += 1.0f;
		pNormals[4*ip1] += v[0];
		pNormals[4*ip1+1] += v[1];
		pNormals[4*ip1+2] += v[2];
		pNormals[4*ip1+3] += 1.0f;
		pNormals[4*ip2] += v[0];
		pNormals[4*ip2+1] += v[1];
		pNormals[4*ip2+2] += v[2];
		pNormals[4*ip2+3] += 1.0f;
	  }
    }

    flip = !flip;
    p0 = p1;	ip0 = ip1;
    p1 = p2;	ip1 = ip2;
  } while (vip != end);

  for (j=0;j<indexPos;j++)
  {
	  float val = 0;
	  ip0 = begin[j];
	  val = pNormals[4*ip0+3];
	  if (val > 1.0f)
	  {
		  val = 1.0f / val;
		  pNormals[4*ip0+0] *= val;
		  pNormals[4*ip0+1] *= val;
		  pNormals[4*ip0+2] *= val;
		  pNormals[4*ip0+3] = 1.0f;
	  }
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,sizeof(VERTEX),vv);
  glNormalPointer(GL_FLOAT,4*sizeof(float),pNormals);
  glDrawElements( GL_TRIANGLE_STRIP, indexPos, GL_UNSIGNED_INT, begin);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

/*@p-u-b-l-i-c---f-u-n-c-t-i-o-n-s-------------------------------------------*/

/*@terrain_render_lighted....................................................*/

void
terrain_render_lighted(const VERTEXbuffer *buffer)
{
  if (pNormals == NULL)
	  pNormals = (float*)calloc(4*40000,sizeof(float));
 
  tstrip_index_render_lighted(
      buffer->vertex,
      (VERTEXid *)buffer->strip + 1,
      (VERTEXid *)buffer->strip + buffer->count
    );
}

void
tstrip_vector_render_lighted(	const VECTOR3f        *vp,
								const VECTOR3f        * const end)
{
  const GLfloat         *p0, *p1, *p2;
  VECTOR3f              v;
  bool                  flip = false;

  if (pNormals == NULL)
	  pNormals = (float*)calloc(4*40000,sizeof(float));
 

  glBegin(GL_TRIANGLE_STRIP);

  unsigned int			ip0 = 0, ip1 = 0, ip2 = 0;
  float					n =0;

  p1 = p2 = *vp;
  do 
  {
    p0 = p1;
    p1 = p2;
    p2 = *vp++;
    NORMAL(v, flip ? p1 : p0, flip ? p0 : p1, p2);

	v[2] *= 5.0f;
    n = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (n != 0)
    {
	  v[0] /= n;
	  v[1] /= n;
	  v[2] /= n;

	  pNormals[4*ip0] += v[0];
	  pNormals[4*ip0+1] += v[1];
	  pNormals[4*ip0+2] += v[2];
	  pNormals[4*ip0+3] += 1.0f;
	  pNormals[4*ip1] += v[0];
	  pNormals[4*ip1+1] += v[1];
	  pNormals[4*ip1+2] += v[2];
	  pNormals[4*ip1+3] += 1.0f;
	  pNormals[4*ip2] += v[0];
	  pNormals[4*ip2+1] += v[1];
	  pNormals[4*ip2+2] += v[2];
	  pNormals[4*ip2+3] += 1.0f;
    }

	glNormal3fv(v);

	ip0 = ip1;
	ip1 = ip2;
	ip2++;

    glVertex3fv(p2);
    flip = !flip;
  } while (vp != end);

  for (unsigned int j=0;j<ip2;j++)
  {
	  float val = 0;
	  val = pNormals[4*j+3];
	  if (val > 1.0f)
	  {
		  val = 1.0f / val;
		  pNormals[4*j+0] *= val;
		  pNormals[4*j+1] *= val;
		  pNormals[4*j+2] *= val;
		  pNormals[4*j+3] = 1.0f;
	  }
  }

  glEnd();
}
