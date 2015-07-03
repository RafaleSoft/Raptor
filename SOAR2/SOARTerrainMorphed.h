#pragma once

#include "SOARTerrainBase.h"


class CSOARTerrainMorphed :	public CSOARTerrainBase
{
public:
	CSOARTerrainMorphed(void);
	virtual ~CSOARTerrainMorphed(void);

	virtual void glClipRender(float fov_pixel,GL_COORD_VERTEX viewPosition);

private:
	void mesh_morph();

	void tstrip_vector_begin( VERTEXid v,              /* index of first vertex in strip */
							  bool     p);               /* parity of first vertex */

	void tstrip_vector_append(VERTEXid	v,
							  bool		p,
							  float		z);

	void submesh_morph(	unsigned	l,
						VERTEXid	i,
						VERTEXid	j,
						VERTEXid	k,
						float		za,
						float		zl,
						float		zr);

	bool vertex_morph(	float         *zmp,
						const VERTEX  *vp,
						float         zl,
						float         zr);

	void normalize(void);
};
