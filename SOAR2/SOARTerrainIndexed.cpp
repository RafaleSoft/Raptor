#include "StdAfx.h"
#include "index.h"
#include "SOARTerrainIndexed.h"

CSOARTerrainIndexed::CSOARTerrainIndexed(void)
{
}

CSOARTerrainIndexed::~CSOARTerrainIndexed(void)
{
}

CSOARTerrainBase::VERTEXid * CSOARTerrainIndexed::tstrip_index_append(
  VERTEXid      *vip,   /* pointer to end of triangle strip */
  VERTEXid      v,      /* index of vertex to append */
  bool          p       /* parity of vertex */
)
{
  const VERTEXid        tail = vip[-2], head = vip[-1], parity = vip[0];

  /*
  ** Add vertex to end of triangle strip vertex buffer.
  */
  if (v != tail && v != head) 
  {
    if ((VERTEXid)p == parity)
      *vip++ = tail;    /* turn corner; duplicate vertex */

    *vip++ = v;         /* append new vertex */
    *vip = (VERTEXid)p; /* store parity here for easy access */
	vertex_buffer.triangles++;
  }

  return vip;
}


CSOARTerrainBase::VERTEXid *CSOARTerrainIndexed::submesh_refine(
  VERTEXid      *vip,   /* pointer to end of triangle strip */
  unsigned      l,      /* refinement level */
  VERTEXid    i,      /* triangle apex */
  VERTEXid    j,      /* supplemental vertex #1 */
  VERTEXid    k)       /* supplemental vertex #2 */
{
  const VERTEX  *vp = *m_pVertex + SPLIT(i, j, k);
  const bool vertex_active = SQR(lod.vd.nu * vp->e + vp->r) > DISTANCE_SQR(vp->p, lod.vd.viewpoint);

  const bool refine = ((l != 0) && vertex_active);

  /*
  ** Recursively refine the mesh.  Since the refinement condition is the
  ** same for both branches and can be somewhat expensive to evaluate,
  ** it is evaluated and tested *before* making the recursive calls.
  */

  if (refine)
    vip = submesh_refine(vip, l - 1, CHILD_L(i, j, k));
  vip = tstrip_index_append(vip, i, l & 1);
  if (refine)
    vip = submesh_refine(vip, l - 1, CHILD_R(i, j, k));

  return vip;
}


void CSOARTerrainIndexed::mesh_refine()
{
  unsigned int n = terrain.levels;

  vertex_buffer.triangles = 0;

  VERTEXid *vip = vertex_buffer.strip;
  *vip++ = I_SW(n / 2);
  *vip++ = I_SW(n / 2);
  *vip = 1;

  vip = submesh_refine(vip, n - 1, ROOT_S(n / 2));
  vip = tstrip_index_append(vip, I_SE(n / 2), false);
  vip = submesh_refine(vip, n - 1, ROOT_E(n / 2));
  vip = tstrip_index_append(vip, I_NE(n / 2), false);
  vip = submesh_refine(vip, n - 1, ROOT_N(n / 2));
  vip = tstrip_index_append(vip, I_NW(n / 2), false);
  vip = submesh_refine(vip, n - 1, ROOT_W(n / 2));

  *vip++ = I_SW(n / 2);
  vertex_buffer.count = (unsigned)(vip - vertex_buffer.strip);
}


#define doNORMAL(v, p0, p1, p2)\
  (void)(\
    (v)[0] = ((p1)[1] - (p0)[1]) * ((p2)[2] - (p0)[2]) -\
             ((p1)[2] - (p0)[2]) * ((p2)[1] - (p0)[1]),\
    (v)[1] = ((p1)[2] - (p0)[2]) * ((p2)[0] - (p0)[0]) -\
             ((p1)[0] - (p0)[0]) * ((p2)[2] - (p0)[2]),\
    (v)[2] = ((p1)[0] - (p0)[0]) * ((p2)[1] - (p0)[1]) -\
             ((p1)[1] - (p0)[1]) * ((p2)[0] - (p0)[0])\
  )

void CSOARTerrainIndexed::tstrip_index_render_lighted(	const VERTEX	*vv,
												const VERTEXid	* const begin,
												const VERTEXid	* const end)
{
  const GLfloat         *p0, *p1, *p2;
  bool                  flip = false;
  unsigned int			indexPos = 0;
  unsigned int			ip0 = 0, ip1 = 0, ip2 = 0;
  unsigned int			j = 0;
  float					n = 0;
  const VERTEXid*		vip = begin; 

  p0 = p1 = vv[*vip].p;
  ip0 = ip1 = *vip;
  do
  {
    ip2 = *vip;
	indexPos++;

    //
    // If triangle is not degenerate, compute its normal.
    //
    if ((p2 = vv[*vip++].p) != p0) 
	{
      GL_COORD_VERTEX v;
      doNORMAL(v, flip ? p1 : p0, flip ? p0 : p1, p2);

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
	  ip0 = begin[j];
	  float val = pNormals[4*ip0+3];
	  if (val > 1.0f)
	  {
		  val = 1.0f / val;
		  pNormals[4*ip0+0] *= val;
		  pNormals[4*ip0+1] *= val;
		  pNormals[4*ip0+2] *= val;
		  pNormals[4*ip0+3] = 1.0f;
		  /*
		  float nx = pNormals[4*ip0+0] * val;
		  float ny = pNormals[4*ip0+1] * val;
		  float nz = pNormals[4*ip0+2] * val;
		  setNormal(ip0,nx,ny,nz,1.0f);
		  */
	  }
  }
}

void CSOARTerrainIndexed::glClipRender(float fov_pixel,GL_COORD_VERTEX viewPosition)
{
	view.fov_pixel = fov_pixel;

	lod.tau = 0.1f * lod.tolerance;
	lod.tau_min = lod.tau;
	lod.tau_max = lod.tau_min; //lod.vd.morph ? (3.0f / 2.0f) * lod.tau_min : lod.tau_min;
	float kappa = lod.tau * view.fov_pixel;

	lod.vd.nu = kappa > 0.0f ? 1.0f / kappa : FLT_MAX;
	lod.vd.nu_min = (2.0f / 3.0f) * lod.vd.nu;
	lod.vd.nu_max = lod.vd.nu;

	lod.vd.viewpoint[0] = viewPosition.x;
	lod.vd.viewpoint[1] = viewPosition.y;
	lod.vd.viewpoint[2] = viewPosition.z;

	if (vertex_buffer.strip != NULL)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		mesh_refine();
				
		memset(pNormals,0,4*40000*sizeof(float));
		tstrip_index_render_lighted(*m_pVertex,
									(VERTEXid *)vertex_buffer.strip + 1,
									(VERTEXid *)vertex_buffer.strip + vertex_buffer.count);
		glNormalPointer(GL_FLOAT,4*sizeof(float),pNormals);
		//glNormalPointer(GL_FLOAT,4*sizeof(float),normals);
		glVertexPointer(3,GL_FLOAT,sizeof(VERTEX),*m_pVertex);
		glDrawElements( GL_TRIANGLE_STRIP, vertex_buffer.count-1, GL_UNSIGNED_INT, (VERTEXid *)vertex_buffer.strip + 1);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}
}
