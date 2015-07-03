#pragma once

#include "common.h"

#include "DTEDReader.h"
#include "Engine/BoundingBox.h"
#include "GLHierarchy/ShadedGeometry.h"

RAPTOR_NAMESPACE


class CSOARTerrainBase :	public CDTEDReader,
							public CShadedGeometry
{
public:
	CSOARTerrainBase();
	virtual ~CSOARTerrainBase(void);

	bool glInitFromDTED(const char* fname,
						const GL_COORD_VERTEX &resolution,
						const GL_COORD_VERTEX &refPoint);

	unsigned int getNbTriangles(void) const { return vertex_buffer.triangles; };

	void setBrothers(	CSOARTerrainBase *pNorth,
						CSOARTerrainBase *pSouth,
						CSOARTerrainBase *pWest,
						CSOARTerrainBase *pEast);

	virtual void glClipRender(float fov_pixel,GL_COORD_VERTEX viewPosition) = 0;

protected:
	typedef struct VIEW_t
	{
	  float			fov_pixel;
	  VECTOR3f      position;               /* viewpoint */
	  VECTOR4f      plane[6];               /* view frustum plane equations */
	} VIEW;

	VIEW			view;

	typedef struct LODinfo_t
	{
	  unsigned      tolerance;          /* tolerance in tenths of pixels */
	  float         tau;                /* error tolerance in pixels */
	  float         tau_min;            /* min error tolerance for morphing */
	  float         tau_max;            /* max error tolerance for morphing */
	  struct 
	  {
		  float         nu;             /* inverse of error tolerance in radians */
		  float         nu_min;         /* lower morph parameter */
		  float         nu_max;         /* upper morph parameter */
		  VECTOR3f      viewpoint;      /* viewpoint */
		  VECTOR4f      viewplane[6];   /* view frustum plane equations */
	  } vd;								/* view-dependent parameters */  
	} LODinfo;

	LODinfo			lod;

	typedef struct GEOMETRY_t
	{
	  unsigned int	levels;         /* number of levels in terrain DAG */
	  unsigned int	width;
	  unsigned int	height;
	} GEOMETRY;

	GEOMETRY		terrain;

	typedef unsigned VERTEXid;

	typedef struct VERTEX_t
	{
	  GL_COORD_VERTEX	p;
	  float				e;              /* error */
	  float				r;              /* bounding sphere radius */
	  float				z;				/* morphed value */
	} VERTEX;

	typedef struct VERTEXbuffer_t
	{
	  unsigned int  count;          /* number of vertices in buffer */
	  unsigned int  triangles;      /* number of triangles in buffer */
	  VERTEXid		*strip;         /* triangle strip vertex buffer */
	} VERTEXbuffer;

	VERTEXbuffer	vertex_buffer;

	typedef struct VECTORstrip_t
	{
	  VECTOR3f      *strip;         /* pointer to end of triangle strip */
	  VERTEXid      head;           /* id of most recent vertex */
	  VERTEXid      tail;           /* id of second most recent vertex */
	  bool          parity;         /* parity of most recent vertex */
	} VECTORstrip;

	VECTORstrip     vector_strip;
	VERTEX			**m_pVertex;
	float			*pNormals;

private:
	VERTEX ** dted_mesh_construct(CBoundingBox		&bbox,
								  unsigned short	*image,
								  unsigned			size_x,
								  unsigned			size_y,
								  unsigned			n,
								  const GL_COORD_VERTEX &res);

	void dted_mesh_lod_compute();   /* 2D vertex array */

	void dted_vertex_lod_compute(	unsigned      i,      /* column index */
									unsigned      j,      /* row index */
									int           di,     /* non-negative col offset to bisected edge endpoint */
									int           dj,     /* row offset to bisected edge endpoint */
									unsigned      n      /* one less array width/height (zero for leaves) */
									);

	void dted_inflate_error_radius(VERTEX * const vp,const VERTEX  *cp);

	void dted_vertex_lod_propagate(	unsigned int x,      /* column index */
									unsigned int y);
	

	CSOARTerrainBase *m_pNorth;
	CSOARTerrainBase *m_pSouth;
	CSOARTerrainBase *m_pWest;
	CSOARTerrainBase *m_pEast;

	GL_COORD_VERTEX	reference;
};
