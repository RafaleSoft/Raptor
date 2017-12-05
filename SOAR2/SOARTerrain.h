#pragma once

#include "common.h"
#include "terrain.h"
#include "refine.h"

#include "GLHierarchy/ShadedGeometry.h"

RAPTOR_NAMESPACE


class CSOARTerrain : public CShadedGeometry
{
public:
	CSOARTerrain();
	virtual ~CSOARTerrain(void);

	bool initFromDTED(	const char* fname,
						const GL_COORD_VERTEX &resolution,
						const GL_COORD_VERTEX &refPoint);

	void glClipRender(float fov_pixel,GL_COORD_VERTEX viewPosition);

	unsigned int getNbTriangles(void) const { return vertex_buffer.triangles; };

	void setBrothers(	CSOARTerrain *pNorth,
						CSOARTerrain *pSouth,
						CSOARTerrain *pWest,
						CSOARTerrain *pEast);

private:
	unsigned short* dted_read(unsigned int &size_x,unsigned int &size_y,const char *infile);

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

	unsigned short* dted_resize(unsigned short *image,
								unsigned int &size_x,
								unsigned int &size_y,
								unsigned int n);

	void mesh_refine();

	void mesh_morph();


	VERTEXid * submesh_refine(	VERTEXid *vip,   /* pointer to end of triangle strip */
								unsigned l,      /* refinement level */
								VERTEXid i,      /* triangle apex */
								VERTEXid j,      /* supplemental vertex #1 */
								VERTEXid k);       /* supplemental vertex #2 */

	VERTEXid * tstrip_index_append(	VERTEXid *vip,   /* pointer to end of triangle strip */
									VERTEXid v,      /* index of vertex to append */
									bool     p);       /* parity of vertex */

	void tstrip_vector_begin( VERTEXid v,              /* index of first vertex in strip */
							  bool     p);               /* parity of first vertex */

	void tstrip_vector_append(VERTEXid      v,
							  bool          p,
							  float         z);

	void submesh_morph(	unsigned      l,
						VERTEXid    i,
						VERTEXid    j,
						VERTEXid    k,
						float         za,
						float         zl,
						float         zr);

	bool vertex_morph(	float         *zmp,
						const VERTEX  *vp,
						float         zl,
						float         zr);

	void tstrip_vector_end(	VERTEXbuffer  *buffer,        /* vertex buffer */
							VECTORstrip   *vsp,           /* pointer to triangle strip */
							const VERTEX  *vv,            /* array of vertices */
							VERTEXid      v);




	CSOARTerrain *m_pNorth;
	CSOARTerrain *m_pSouth;
	CSOARTerrain *m_pWest;
	CSOARTerrain *m_pEast;

	GL_COORD_VERTEX	reference;
	GEOMETRY		terrain;
	VERTEXbuffer	vertex_buffer;
	VECTORstrip     vector_strip;
	LODinfo			lod;
	VIEW			view;
	VERTEX			**m_pVertex;
};
