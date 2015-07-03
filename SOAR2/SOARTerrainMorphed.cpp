#include "StdAfx.h"
#include "index.h"
#include "SOARTerrainMorphed.h"

CSOARTerrainMorphed::CSOARTerrainMorphed(void)
{
}

CSOARTerrainMorphed::~CSOARTerrainMorphed(void)
{
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

void CSOARTerrainMorphed::normalize(void)
{
	memset(pNormals,0,4*40000*sizeof(float));

	const VERTEX	*vv = *m_pVertex;
	bool			flip = true;
	unsigned int	ip0 = 0, ip1 = 0, ip2 = 0;

	ip0 = vertex_buffer.strip[0];
	ip1 = vertex_buffer.strip[1];

	for (unsigned int j=2;j<vertex_buffer.count;j++)
	{
		ip2 = vertex_buffer.strip[j];

		VECTOR3f v;
		doNORMAL(v, flip ? vv[ip1].p : vv[ip0].p, flip ? vv[ip0].p : vv[ip1].p, vv[ip2].p);

		//v[2] *= 5.0f;
		float n = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
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
		flip = !flip;
		ip0 = ip1;
		ip1 = ip2;
	}

	for (unsigned int j=0;j<vertex_buffer.count;j++)
	{
		VERTEXid idx = vertex_buffer.strip[j];
		float val = pNormals[4*idx+3];
		if (val > 1.0f)
		{
			val = 1.0f / val;
			pNormals[4*idx+0] *= val;
			pNormals[4*idx+1] *= val;
			pNormals[4*idx+2] *= val;
			pNormals[4*idx+3] = 1.0f;
		}
	}
}


bool CSOARTerrainMorphed::vertex_morph(
  float         *zmp,   /* pointer to morphed elevation */
  const VERTEX  *vp,    /* pointer to vertex */
  float         zl,     /* elevation of left endpoint of split edge */
  float         zr)      /* elevation of right endpoint of split edge */
{
	/*
	** Compute the elevation of the morphed vertex.  The return value indicates
	** whether the vertex is active or not.
	*/
	GL_COORD_VERTEX V(vp->p[0],vp->p[1],vp->z,1.0f);
	float d = DISTANCE_SQR(V, lod.vd.viewpoint);
	float dmax = SQR(lod.vd.nu_max * vp->e + vp->r);
	if (dmax > d) 
	{
		float dmin = SQR(lod.vd.nu_min * vp->e + vp->r);
		*zmp = dmin > d
				 ? vp->z //p[2]
				 : ((dmax - d) * vp->z /*p[2]*/ + (d - dmin) * 0.5f * (zl + zr)) / (dmax - dmin);
		return true;
	}
	else
		return false;
}

void CSOARTerrainMorphed::submesh_morph(unsigned	l,      /* refinement level */
										VERTEXid	i,      /* triangle apex */
										VERTEXid	j,      /* supplemental vertex #1 */
										VERTEXid	k,       /* supplemental vertex #2 */
										float		za,     /* elevation of apex */
										float		zl,     /* elevation of left corner */
										float		zr)      /* elevation of right corner */
{
  float         zm = 0;
  const bool    refine = (l != 0) &&
                         vertex_morph(&zm, *m_pVertex + SPLIT(i, j, k), zl, zr);

  /*
  ** Recursively refine and morph the mesh.
  */
  if (refine)
    submesh_morph(l - 1, CHILD_L(i, j, k), zm, zl, za);
  tstrip_vector_append(i, l & 1, za);
  if (refine)
    submesh_morph(l - 1, CHILD_R(i, j, k), zm, za, zr);
}


void CSOARTerrainMorphed::tstrip_vector_append(	VERTEXid      v,      /* index of vertex to append */
												bool          p,      /* parity of vertex */
												float         z)       /* elevation of morphed vertex */
{
	VERTEX  *vv = *m_pVertex;

	if (v != vector_strip.tail && v != vector_strip.head) 
	{
		if (p == vector_strip.parity)
		{
			vertex_buffer.strip[vertex_buffer.count++] = vector_strip.tail;
		}
		else 
		{
			vector_strip.parity = p;
			vector_strip.tail = vector_strip.head;
		}
		vector_strip.head = v;
		vv[v].p[2] = z;
		vertex_buffer.triangles++;
		vertex_buffer.strip[vertex_buffer.count++] = v;
	}
}


void CSOARTerrainMorphed::tstrip_vector_begin(	VERTEXid v,            /* index of first vertex in strip */
												bool p)               /* parity of first vertex */
{
	for (int i=terrain.width*terrain.height-1;i>=0;i--)
	{
		VERTEX  *vp = *m_pVertex + i;
		vp->p.z = vp->z;
	}

	vector_strip.head = vector_strip.tail = v;
	vector_strip.parity = p;
	vertex_buffer.triangles = 0;
	vertex_buffer.count = 0;

	vertex_buffer.strip[vertex_buffer.count++] = v;
	vertex_buffer.strip[vertex_buffer.count++] = v;
}


void CSOARTerrainMorphed::mesh_morph()
{
	/* maximum number of refinement levels */
	unsigned int n = terrain.levels;

	const VERTEX  *vv = *m_pVertex;
	const FLOAT  zc = vv[I_C(n / 2)].z,
				zsw = vv[I_SW(n / 2)].z,
				zse = vv[I_SE(n / 2)].z,
				zne = vv[I_NE(n / 2)].z,
				znw = vv[I_NW(n / 2)].z;

	// index of last vertex in strip
	VERTEXid v = I_SW(n / 2);

	/*
	** Top-level function for constructing a morphed mesh.
	*/  
	tstrip_vector_begin(v, true);
	submesh_morph(n - 1, ROOT_S(n / 2), zc, zsw, zse);
	tstrip_vector_append(I_SE(n / 2), 0, zse);
	submesh_morph(n - 1, ROOT_E(n / 2), zc, zse, zne);
	tstrip_vector_append(I_NE(n / 2), 0, zne);
	submesh_morph(n - 1, ROOT_N(n / 2), zc, zne, znw);
	tstrip_vector_append(I_NW(n / 2), 0, znw);
	submesh_morph(n - 1, ROOT_W(n / 2), zc, znw, zsw);

	vertex_buffer.strip[vertex_buffer.count++] = v;
}

void CSOARTerrainMorphed::glClipRender(float fov_pixel,GL_COORD_VERTEX viewPosition)
{
	view.fov_pixel = fov_pixel;

	lod.tau = 0.1f * lod.tolerance;
	lod.tau_min = lod.tau;
	lod.tau_max = (3.0f / 2.0f) * lod.tau_min;
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

		mesh_morph();
		normalize();

		glVertexPointer(3,GL_FLOAT,sizeof(VERTEX),*m_pVertex);
		glNormalPointer(GL_FLOAT,4*sizeof(float),pNormals);
		glDrawElements( GL_TRIANGLE_STRIP, vertex_buffer.count-1, GL_UNSIGNED_INT, vertex_buffer.strip + 1);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}
}

