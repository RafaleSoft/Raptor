#pragma once

#include "SOARTerrainBase.h"

RAPTOR_NAMESPACE

class CSOARTerrainIndexed :	public CSOARTerrainBase
{
public:
	CSOARTerrainIndexed(void);
	virtual ~CSOARTerrainIndexed(void);

	virtual void glClipRender(float fov_pixel,GL_COORD_VERTEX viewPosition);

private:
	void mesh_refine();

	void tstrip_index_render_lighted(	const VERTEX	* vertex,
										const VERTEXid	* const begin,
										const VERTEXid	* const end);

	VERTEXid * submesh_refine(	VERTEXid *vip,   /* pointer to end of triangle strip */
								unsigned l,      /* refinement level */
								VERTEXid i,      /* triangle apex */
								VERTEXid j,      /* supplemental vertex #1 */
								VERTEXid k);       /* supplemental vertex #2 */

	VERTEXid * tstrip_index_append(	VERTEXid *vip,   /* pointer to end of triangle strip */
									VERTEXid v,      /* index of vertex to append */
									bool     p);       /* parity of vertex */

};
