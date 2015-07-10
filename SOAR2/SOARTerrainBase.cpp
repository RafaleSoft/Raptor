#include "StdAfx.h"
#include "SOARTerrainBase.h"


#include <float.h>

#include "index.h"


static const unsigned int LEVEL_MIN = 2;


CSOARTerrainBase::CSOARTerrainBase()
{
	lod.tolerance = 3;
	lod.tau = lod.tau_min = lod.tau_max = 0.1f * lod.tolerance;
	vertex_buffer.count = 0;
	vertex_buffer.triangles = 0;
	vertex_buffer.strip = NULL;

	terrain.height = 0;
	terrain.width = 0;
	terrain.levels = 0;

	m_pVertex = NULL;

	m_pNorth = NULL;
	m_pSouth = NULL;
	m_pWest = NULL;
	m_pEast = NULL;

	pNormals = NULL;
}

CSOARTerrainBase::~CSOARTerrainBase(void)
{
}

bool CSOARTerrainBase::glInitFromDTED(	const char* fname,
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

	free(image);
	vertex_buffer.strip = (VERTEXid*)malloc(n*n*3*sizeof(unsigned int));
	if (pNormals == NULL)
		pNormals = (float*)calloc(4*40000,sizeof(float));
	
	//createPrimitive(CGeometryPrimitive::TRIANGLE_STRIP);
	//glSetVertices(n*n,NULL);

	return true;
}


CSOARTerrainBase::VERTEX ** CSOARTerrainBase::dted_mesh_construct(
  CBoundingBox			&bbox, /* bounding box min, max */
  unsigned short        *image, /* height field data */
  unsigned              size_x, /* height field width */
  unsigned              size_y, /* height field height */
  unsigned              n,      /* grid width/height */
  const GL_COORD_VERTEX &res)    /* xyz resolution */
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

		vv[j][i].z = vv[j][i].p.z;

		bbox.extendTo(vv[j][i].p.x,vv[j][i].p.y,vv[j][i].p.z);
	}

	if (size_x < n || size_y < n) 
	{
		for (unsigned int j = 0; j != size_y; j++)
		  for (unsigned int i = size_x; i != n; i++)
		  {
			COPY(vv[j][i].p, vv[j][size_x - 1].p);
			vv[j][i].z = vv[j][size_x - 1].z;
		  }
		for (unsigned int j = size_y; j != n; j++)
		  for (unsigned int i = 0; i != size_x; i++)
		  {
			COPY(vv[j][i].p, vv[size_y - 1][i].p);
			vv[j][i].z = vv[size_y - 1][i].z;
		  }
		for (unsigned int j = size_y; j != n; j++)
		  for (unsigned int i = size_x; i != n; i++)
		  {
			COPY(vv[j][i].p, vv[size_y - 1][size_x - 1].p);
			vv[j][i].z = vv[size_y - 1][size_x - 1].z;
		  }
	}

	return vv;
}

void CSOARTerrainBase::dted_mesh_lod_compute()
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
CSOARTerrainBase::dted_vertex_lod_propagate(unsigned int x,   /* column index */
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
CSOARTerrainBase::dted_vertex_lod_compute(
  unsigned      x,      /* column index */
  unsigned      y,      /* row index */
  int           dx,     /* non-negative col offset to bisected edge endpoint */
  int           dy,     /* row offset to bisected edge endpoint */
  unsigned      n)      /* one less array width/height (zero for leaves) */
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
		  CSOARTerrainBase *brother = NULL;
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
void CSOARTerrainBase::dted_inflate_error_radius(VERTEX * const vp,const VERTEX  *cp)
{
	vp->e = (vp->e > cp->e ? vp->e : cp->e);
	float r = DISTANCE(vp->p, cp->p) + cp->r;
	vp->r = (vp->r > r ? vp->r : r);
}

void CSOARTerrainBase::setBrothers(CSOARTerrainBase *pNorth,
							   CSOARTerrainBase *pSouth,
							   CSOARTerrainBase *pWest,
							   CSOARTerrainBase *pEast)
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
			vtx.z = z;

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
			vtx.z = z;

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
		v.z = v.p[2];
	}


	GL_COORD_VERTEX Min(bbox.xMin(),bbox.yMin(),bbox.zMin());
	GL_COORD_VERTEX Max(bbox.xMax(),bbox.yMax(),bbox.zMax());
	setBoundingBox(Min,Max);

	dted_mesh_lod_compute();
}

