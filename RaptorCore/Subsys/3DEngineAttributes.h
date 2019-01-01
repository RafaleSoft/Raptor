#ifndef __3DENGINEATTRIBUTES_H__
#define __3DENGINEATTRIBUTES_H__


RAPTOR_NAMESPACE_BEGIN


class C3DEngineAttributes
{
public:
	C3DEngineAttributes() {};
	~C3DEngineAttributes() {};

	int						viewport[4];
    CGenericMatrix<float>   projection;
    CGenericMatrix<float>   modelview;
    float                   farPlane;
	vector<CGenericVector<float> >	clippingPlanes;

	static CCollisionStatus			currentStatus;
	static int						m_iNbCollisions;

	GL_MATRIX				currentModelview;
    CGenericMatrix<float>	visibilityProjection;

	//!	Description of a visible Zone, expressed
	//! in the viewer's coordinate system
	//! ( which can also be used from any point of view,
	//! for example for a light to determine illumination
	//!	or shadows ).
	typedef struct visibleZone_t
	{
		//!	The min coord of bbox in viewer coordinates
		GL_COORD_VERTEX			minZone;
		//!	The max coord of bbox in viewer coordinates
		GL_COORD_VERTEX			maxZone;
		//!	The transformed bbox ccords in viewer coordindates.
		CGenericVector<float>	tCoords[8];
		//!	The corresponding bbox.
		const CBoundingBox*	pBbox;
	} visibleZone;

	vector<visibleZone>				visibility;
	map<const CBoundingBox*,vector<visibleZone> > visibleBBox;
	vector<visibleZone>				visibilityResult;
};

RAPTOR_NAMESPACE_END

#endif

