#include "StdAfx.h"
#include "SOARTerrain.h"


#include <float.h>

#include "index.h"

extern void terrain_render_lighted(const VERTEXbuffer    *buffer);
extern void tstrip_vector_render_lighted(	const VECTOR3f        *vp,
											const VECTOR3f        * const end);

static const unsigned int LEVEL_MIN = 2;

/*@tstrip_index_append.......................................................*/

VERTEXid *
CSOARTerrain::tstrip_index_append(
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


/*@submesh_refine............................................................*/

VERTEXid *
CSOARTerrain::submesh_refine(
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


void CSOARTerrain::mesh_refine()
{
  unsigned int n = terrain.levels;

  vertex_buffer.triangles = 0;

  VERTEXid *vip = (VERTEXid*)vertex_buffer.strip;
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
  vertex_buffer.count = (unsigned)(vip - (VERTEXid *)vertex_buffer.strip);
}


void CSOARTerrain::tstrip_vector_begin(	VERTEXid v,            /* index of first vertex in strip */
										bool p)               /* parity of first vertex */
{
  vector_strip.strip = (VECTOR3f*)vertex_buffer.strip;
  vector_strip.head = vector_strip.tail = v;
  vector_strip.parity = p;
  COPY(*vector_strip.strip, m_pVertex[v]->p);
  vector_strip.strip++;
  COPY(*vector_strip.strip, m_pVertex[v]->p);
  vector_strip.strip++;
  vector_strip.triangles = 0;
}

void CSOARTerrain::tstrip_vector_append(VERTEXid      v,      /* index of vertex to append */
										bool          p,      /* parity of vertex */
										float         z)       /* elevation of morphed vertex */
{
  const VERTEX  *vv = vertex_buffer.vertex;

  if (v != vector_strip.tail && v != vector_strip.head) 
  {
    VECTOR3f *vp = vector_strip.strip;
    if (p == vector_strip.parity) 
	{
      COPY(vp[0], vp[-2]); /* turn corner; duplicate vertex */
      vp++;
    }
    else 
	{
      vector_strip.parity = p;
      vector_strip.tail = vector_strip.head;
    }
    vector_strip.head = v;
    ASSIGN(*vp, vv[v].p[0], vv[v].p[1], z); /* append new vertex */
    vector_strip.strip = ++vp;
    vector_strip.triangles++;
  }
}

bool CSOARTerrain::vertex_morph(
  float         *zmp,   /* pointer to morphed elevation */
  const VERTEX  *vp,    /* pointer to vertex */
  float         zl,     /* elevation of left endpoint of split edge */
  float         zr)      /* elevation of right endpoint of split edge */
{
  /*
  ** Compute the elevation of the morphed vertex.  The return value indicates
  ** whether the vertex is active or not.
  */
  const VDinfo  *vdp = &lod.vd;

  float d = DISTANCE_SQR(vp->p, vdp->viewpoint);
  float dmax = SQR(vdp->nu_max * vp->e + vp->r);
  if (dmax > d) 
  {
    float dmin = SQR(vdp->nu_min * vp->e + vp->r);
    *zmp = dmin > d
             ? vp->p[2]
             : ((dmax - d) * vp->p[2] + (d - dmin) * 0.5f * (zl + zr)) / (dmax - dmin);
    return true;
  }
  else
    return false;
}

void CSOARTerrain::submesh_morph(
  unsigned      l,      /* refinement level */
  VERTEXid    i,      /* triangle apex */
  VERTEXid    j,      /* supplemental vertex #1 */
  VERTEXid    k,       /* supplemental vertex #2 */
  float         za,     /* elevation of apex */
  float         zl,     /* elevation of left corner */
  float         zr      /* elevation of right corner */
)
{
  float         zm;
  const bool    refine = (l != 0) &&
                         vertex_morph(&zm, vertex_buffer.vertex + SPLIT(i, j, k), zl, zr);

  /*
  ** Recursively refine and morph the mesh.
  */
  if (refine)
    submesh_morph(l - 1, CHILD_L(i, j, k), zm, zl, za);
  tstrip_vector_append(i, l & 1, za);
  if (refine)
    submesh_morph(l - 1, CHILD_R(i, j, k), zm, za, zr);
}

void CSOARTerrain::tstrip_vector_end(
  VERTEXbuffer  *buffer,        /* vertex buffer */
  VECTORstrip   *vsp,           /* pointer to triangle strip */
  const VERTEX  *vv,            /* array of vertices */
  VERTEXid      v)               /* index of last vertex in strip */
{
  COPY(*vsp->strip, vv[v].p);
  vsp->strip++;
  buffer->triangles = vsp->triangles;
  buffer->count = (unsigned)(vsp->strip - (VECTOR3f *)buffer->strip);
}


void CSOARTerrain::mesh_morph()
{
	/* maximum number of refinement levels */
	unsigned int n = terrain.levels;

	const VERTEX  *vv = vertex_buffer.vertex;
	const FLOAT   zc = vv[I_C(n / 2)].p.z,
				zsw = vv[I_SW(n / 2)].p.z,
				zse = vv[I_SE(n / 2)].p.z,
				zne = vv[I_NE(n / 2)].p.z,
				znw = vv[I_NW(n / 2)].p.z;

  /*
  ** Top-level function for constructing a morphed mesh.
  */  
  tstrip_vector_begin(I_SW(n / 2), 1);
  submesh_morph(n - 1, ROOT_S(n / 2), zc, zsw, zse);
  tstrip_vector_append(I_SE(n / 2), 0, zse);
  submesh_morph(n - 1, ROOT_E(n / 2), zc, zse, zne);
  tstrip_vector_append(I_NE(n / 2), 0, zne);
  submesh_morph(n - 1, ROOT_N(n / 2), zc, zne, znw);
  tstrip_vector_append(I_NW(n / 2), 0, znw);
  submesh_morph(n - 1, ROOT_W(n / 2), zc, znw, zsw);
  
  tstrip_vector_end(&vertex_buffer, &vector_strip, vv, I_SW(n / 2));
}


CSOARTerrain::CSOARTerrain()
{
	lod.tolerance = 3; //3;
	lod.tau = lod.tau_min = lod.tau_max = 0.1f * lod.tolerance;
	vertex_buffer.count = 0;
	vertex_buffer.triangles = 0;
	vertex_buffer.strip = NULL;
	vertex_buffer.vertex = NULL;

	terrain.morph = false;
	terrain.height = 0;
	terrain.width = 0;
	terrain.levels = 0;
	terrain.vertices = 0;

	m_pVertex = NULL;

	m_pNorth = NULL;
	m_pSouth = NULL;
	m_pWest = NULL;
	m_pEast = NULL;
}

CSOARTerrain::~CSOARTerrain(void)
{
}

void CSOARTerrain::glClipRender(float fov_pixel,GL_COORD_VERTEX viewPosition)
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
		vertex_buffer.vertex = *m_pVertex;

		terrain.morph = true;
		if (terrain.morph)
		{
			mesh_morph();
			tstrip_vector_render_lighted(	(const VECTOR3f *)vertex_buffer.strip + 1,
											(const VECTOR3f *)vertex_buffer.strip + vertex_buffer.count);
		}
		else
		{
			mesh_refine();
			terrain_render_lighted(&vertex_buffer);
		}
	}
}

bool CSOARTerrain::initFromDTED(const char* fname,
								const GL_COORD_VERTEX &resolution,
								const GL_COORD_VERTEX &refPoint)
{
	reference = refPoint;

	unsigned int size_x = 0;
	unsigned int size_y = 0;
	unsigned int depth = 0;
	unsigned short *image = dted_read(size_x, size_y, fname);
	if (image == NULL)
		return false;

	unsigned int levels = 0;
	unsigned int n = 0;
	for (levels = 0; (1u << levels) + 1 < MAX(size_x, size_y); levels++);
	n = (1u << levels) + 1;
	levels *= 2;
	if (levels < LEVEL_MIN)
		return false;

	unsigned short *img = image;
	image = dted_resize(img,size_x,size_y,n);
	free(img);

	terrain.levels = levels;
	terrain.width = size_x;
	terrain.height = size_y;

	CBoundingBox bbox;
	m_pVertex = dted_mesh_construct(bbox, image, size_x, size_y, n, resolution);

	GL_COORD_VERTEX Min(bbox.xMin(),bbox.yMin(),bbox.zMin());
	GL_COORD_VERTEX Max(bbox.xMax(),bbox.yMax(),bbox.zMax());
	setBoundingBox(Min,Max);

	dted_mesh_lod_compute();
	terrain.vertices = LINEAR_COUNT(levels);

	free(image);
	vertex_buffer.strip = malloc(n*n*3*sizeof(unsigned int));

	return true;
}

unsigned short* CSOARTerrain::dted_resize(unsigned short *image,unsigned int &size_x,unsigned int &size_y,unsigned int n)
{
	unsigned short* img = (unsigned short*)malloc(sizeof(unsigned short)*n*n);

	float stepx = ((float)size_x) / ((float)n);
	float stepy = ((float)size_y) / ((float)n);

	for (unsigned int j=0;j<n;j++)
	{
		for (unsigned int i=0;i<n;i++)
		{
			unsigned int col = i*stepx;
			unsigned int row = j*stepy;
			unsigned int src_pos = col + row*size_x;
			unsigned short alt = image[src_pos];
			unsigned short alt2 = (col < size_x-1 ? image[src_pos+1] : alt);
			unsigned short alt3 = (row < size_y-1 ? image[src_pos+size_x] : alt);
			unsigned short alt4 = ((col < size_x-1) && (row < size_y-1) ? image[src_pos+size_x+1] : alt);
			img[i+j*n] = (alt + alt2 + alt3 + alt4) >> 2;
		}
	}

	size_x = n;
	size_y = n;
	return img;
}

unsigned short* CSOARTerrain::dted_read(unsigned int &size_x,unsigned int &size_y,const char *infile)
{
	unsigned char w = 0;
	unsigned char h = 0;
	unsigned short *img = NULL;
	FILE *in = NULL;
	size_t readSize = 0;

	if (NULL == infile)
		return NULL;

	in = fopen(infile,"rb");
	if (NULL == in)
		return NULL;

	if (1 != fread(&w,1,1,in))
	{
		fclose(in);
		return NULL;
	}
	if (1 != fread(&h,1,1,in))
	{
		fclose(in);
		return NULL;
	}

	img = (unsigned short*)malloc(w*h*2);

	readSize = fread(img,2,w*h,in);
	if (w*h != readSize)
	{
		fclose(in);
		free(img);
		return NULL;
	}

	fclose(in);

	size_x = w;
	size_y = h;

	return img;
}


VERTEX ** CSOARTerrain::dted_mesh_construct(
  CBoundingBox			&bbox, /* bounding box min, max */
  unsigned short        *image, /* height field data */
  unsigned              size_x, /* height field width */
  unsigned              size_y, /* height field height */
  unsigned              n,      /* grid width/height */
  const GL_COORD_VERTEX &res    /* xyz resolution */
)
{
	VERTEX			**vv = NULL;
	unsigned short	*sp = image;
	float			sx = 1.0f / (float)(size_x-1);
	float			sy = 1.0f / (float)(size_y-1);

	vv = (VERTEX**)malloc(n * sizeof(VERTEX*));
    *vv = (VERTEX*)malloc(n*n * sizeof(VERTEX)); 
	for (unsigned int j = 1; j != n; j++)
		vv[j] = vv[j - 1] + n;

	for (unsigned int i = 0; i != size_x; i++)
	for (unsigned int j = size_y-1; j < size_y; j--)
	{
		vv[j][i].p.x = reference.x + res.x * i * sx;
		vv[j][i].p.y = reference.y + res.y * j * sy;
		vv[j][i].p.z = reference.z + res.z * (float)(*sp++);

		bbox.extendTo(vv[j][i].p.x,vv[j][i].p.y,vv[j][i].p.z);
	}

	if (size_x < n || size_y < n) 
	{
		for (unsigned int j = 0; j != size_y; j++)
		  for (unsigned int i = size_x; i != n; i++)
			COPY(vv[j][i].p, vv[j][size_x - 1].p);
		for (unsigned int j = size_y; j != n; j++)
		  for (unsigned int i = 0; i != size_x; i++)
			COPY(vv[j][i].p, vv[size_y - 1][i].p);
		for (unsigned int j = size_y; j != n; j++)
		  for (unsigned int i = size_x; i != n; i++)
			COPY(vv[j][i].p, vv[size_y - 1][size_x - 1].p);
	}

	return vv;
}

void CSOARTerrain::dted_mesh_lod_compute()
{
  const unsigned	n = 1 << (terrain.levels / 2);
  unsigned int		i, j, s;
  int				a, b, c;

  /*
  ** Compute error and radius bottom-up, level-by-level.  This is done
  ** assuming a standard 2D matrix layout of the data, since this layout
  ** allows easy access to all four children of a vertex.  Depending on
  ** the choice of indexing scheme, the data may later have to be rearranged.
  */

  for (a = c = 1, b = 2, s = 0; (unsigned)a != n; a = c = b, b *= 2, s = n) 
  {
    /*
    ** Process level in black quadtree.
    */
    for (j = a; j < n; j += b) 
	{
      for (i = 0; i <= n; i += b) 
	  {
        dted_vertex_lod_compute(i // column
								, j,// row	=> v[j][i]
								0, a, s);

		dted_vertex_lod_propagate(i,j);

        dted_vertex_lod_compute(j, // column
								i, // row	=> v[i][j]
								a, 0, s);

		dted_vertex_lod_propagate(j,i);
      }
    }

    /*
    ** Process level in white quadtree.
    */
    for (j = a; j < n; c = -c, j += b)
      for (i = a; i < n; c = -c, i += b)
	  {
        dted_vertex_lod_compute(i, j, a, c, n);

		dted_vertex_lod_propagate(i,j);
	  }
  }

  /*
  ** Lock center and corner vertices.
  */
  const unsigned		m = n / 2;
  m_pVertex[0][0].e = m_pVertex[n][0].e = m_pVertex[0][n].e = m_pVertex[n][n].e = m_pVertex[m][m].e = FLT_MAX;
  m_pVertex[0][0].r = m_pVertex[n][0].r = m_pVertex[0][n].r = m_pVertex[n][n].r = m_pVertex[m][m].r = FLT_MAX;
}


void
CSOARTerrain::dted_vertex_lod_propagate(unsigned int x,   /* column index */
										unsigned int y)   /* row index */
{
	if ((y == (terrain.height-1)) && (NULL != m_pNorth))
	{
		VERTEX &V = m_pNorth->m_pVertex[0][x];
		float e = MAX(m_pVertex[y][x].e,V.e);
		float r = MAX(m_pVertex[y][x].r,V.r);

		m_pVertex[y][x].e = V.e = e;
		m_pVertex[y][x].r = V.r = r;
	}
	else if ((y == 0) && (NULL != m_pSouth))
	{
		VERTEX &V = m_pSouth->m_pVertex[terrain.height-1][x];
		float e = MAX(m_pVertex[y][x].e,V.e);
		float r = MAX(m_pVertex[y][x].r,V.r);

		m_pVertex[y][x].e = V.e = e;
		m_pVertex[y][x].r = V.r = r;
	}

	if ((x == (terrain.width-1)) && (NULL != m_pEast))
	{
		VERTEX &V = m_pEast->m_pVertex[y][0];
		float e = MAX(m_pVertex[y][x].e,V.e);
		float r = MAX(m_pVertex[y][x].r,V.r);

		m_pVertex[y][x].e = V.e = e;
		m_pVertex[y][x].r = V.r = r;
	}
	else if ((x == 0) && (NULL != m_pWest))
	{
		VERTEX &V = m_pWest->m_pVertex[y][terrain.width-1];
		float e = MAX(m_pVertex[y][x].e,V.e);
		float r = MAX(m_pVertex[y][x].r,V.r);

		m_pVertex[y][x].e = V.e = e;
		m_pVertex[y][x].r = V.r = r;
	}
}


void
CSOARTerrain::dted_vertex_lod_compute(
  unsigned      x,      /* column index */
  unsigned      y,      /* row index */
  int           dx,     /* non-negative col offset to bisected edge endpoint */
  int           dy,     /* row offset to bisected edge endpoint */
  unsigned      n      /* one less array width/height (zero for leaves) */
)
{
  VERTEX        * const vp = &m_pVertex[y][x];

  /*
  ** Compute actual error and initialize radius to zero.  The error is
  ** simply the vertical difference between the vertex and the bisected
  ** edge, i.e. the error between two consecutive levels of refinement.
  ** This object-space error can be replaced with any measure of error,
  ** as long as the nesting step below is performed.
  */

  vp->e = fabsf(vp->p[2] - 0.5f * (m_pVertex[y-dy][x-dx].p[2] + m_pVertex[y+dy][x+dx].p[2]));
  vp->r = 0.0f;

  /*
  ** If the vertex is not a leaf node, ensure that the error and radius are
  ** nested using information from its four children.  Note that the offsets
  ** (+di, +dj) and (-di, -dj) from (i, j) initially get us to the two
  ** vertices of the bisected edge.  By "rotating" (di, dj) 45 degrees (in
  ** a topological sense), we arrive at one of the children of (i, j)
  ** (assuming we're not on a boundary).  Successive 90-degree rotations
  ** then allow us to visit all four children.
  */

  if (n) 
  {
    /*
    ** di' = (di - dj) / 2
    ** dj' = (di + dj) / 2
    */
    dy = (dx + dy) / 2;
    dx -= dy;
    unsigned int k = 4;
    do 
	{
      /*
      ** Test whether child vertex exists.
      */
      if ((x != 0 || dx >= 0) && (x != n || dx <= 0) &&
          (y != 0 || dy >= 0) && (y != n || dy <= 0)) 
	  {
		dted_inflate_error_radius(vp,&m_pVertex[y + dy][x + dx]);
      }
	  else if ((m_pNorth != NULL) || (m_pSouth != NULL) || (m_pEast != NULL) || (m_pWest != NULL))
	  {
		  CSOARTerrain *brother = NULL;
		  int row = y + dy;
		  if (row > (int)n)
		  {
			  row = row - terrain.height; //n - 1;
			  brother = m_pNorth;
		  } 
		  else if (row < 0)
		  {
			  row = terrain.height + row;// + 1;
			  brother = m_pSouth;
		  }

		  int col = x + dx;
		  if (col > (int)n)
		  {
			  col = col - terrain.width; //n - 1;
			  brother = m_pEast;
		  }
		  else if (col < 0)
		  {
			  col = terrain.width + col;// + 1;
			  brother = m_pWest;
		  }

		  if (brother != NULL)
			  dted_inflate_error_radius(vp,&brother->m_pVertex[row][col]);
	  }
      /*
      ** di' = -dj
      ** dj' = +di
      */
      dy += dx;
      dx -= dy;
      dy += dx;
    } while (--k);
  }
}

/*
** Inflate error and radius as needed.
*/
void CSOARTerrain::dted_inflate_error_radius(VERTEX * const vp,const VERTEX  *cp)
{
	vp->e = (vp->e > cp->e ? vp->e : cp->e);
	float r = DISTANCE(vp->p, cp->p) + cp->r;
	vp->r = (vp->r > r ? vp->r : r);
}

void CSOARTerrain::setBrothers(CSOARTerrain *pNorth,
							   CSOARTerrain *pSouth,
							   CSOARTerrain *pWest,
							   CSOARTerrain *pEast)
{
	m_pNorth = pNorth;
	m_pSouth = pSouth;
	m_pWest = pWest;
	m_pEast = pEast;

	CBoundingBox bbox(*this->boundingBox());

	if (pNorth != NULL)
	{
		VERTEX *v = *m_pVertex;
		VERTEX *north = *m_pNorth->m_pVertex;

		unsigned int j = terrain.height-1;
		for (unsigned int i = 0 ; i < terrain.width ; i++)
		{
			VERTEX &vtx = v[i+terrain.width*j];
			float x = vtx.p.x = north[i].p.x;
			float y = vtx.p.y = north[i].p.y;
			float z = vtx.p.z = north[i].p.z;

			bbox.extendTo(x,y,z);
		}
	}

	if (pEast != NULL)
	{
		VERTEX *v = *m_pVertex;
		VERTEX *east = *m_pEast->m_pVertex;

		unsigned int i = terrain.width-1;
		for (unsigned int j=0 ; j < terrain.height ; j++)
		{
			VERTEX &vtx = v[i+terrain.width*j];
			float x = vtx.p.x = east[terrain.width*j].p.x;
			float y = vtx.p.y = east[terrain.width*j].p.y;
			float z = vtx.p.z = east[terrain.width*j].p.z;

			bbox.extendTo(x,y,z);
		}
	}

	/*
	** Specific case for corners: 4 brothers share each corner.
	** ... TODO ...
	*/
	// North-East
	{
		int nb = 1;
		VERTEX v = m_pVertex[128][128];

		if (m_pNorth != NULL)
		{
			nb ++;
			for (int i=0;i<3;i++)
				v.p[i] += m_pNorth->m_pVertex[0][128].p[i];
			if (m_pNorth->m_pEast != NULL)
			{
				nb ++;
				for (int i=0;i<3;i++)
					v.p[i] += m_pNorth->m_pEast->m_pVertex[0][0].p[i];
			}
		}
		if (m_pEast != NULL)
		{
			nb ++;
			for (int i=0;i<3;i++)
				v.p[i] += m_pEast->m_pVertex[128][0].p[i];

			if ((nb < 4) && (m_pEast->m_pNorth != NULL))
			{
				nb ++;
				for (int i=0;i<3;i++)
					v.p[i] += m_pEast->m_pNorth->m_pVertex[0][0].p[i];
			}
		}

		v.p[0] /= nb;
		v.p[1] /= nb;
		v.p[2] /= nb;
	}


	GL_COORD_VERTEX Min(bbox.xMin(),bbox.yMin(),bbox.zMin());
	GL_COORD_VERTEX Max(bbox.xMax(),bbox.yMax(),bbox.zMax());
	setBoundingBox(Min,Max);

	dted_mesh_lod_compute();
}

