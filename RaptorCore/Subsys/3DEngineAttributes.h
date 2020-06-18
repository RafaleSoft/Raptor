/***************************************************************************/
/*                                                                         */
/*  3DEngineAttributes.h                                                   */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __3DENGINEATTRIBUTES_H__
#define __3DENGINEATTRIBUTES_H__


RAPTOR_NAMESPACE_BEGIN


class C3DEngineAttributes
{
public:
	C3DEngineAttributes() {};
	~C3DEngineAttributes() {};

	/*
		Viewpoint current state:
		- a viewport dimensions,
		- a projection matrix,
		- a modelview matrix,
		- clipping definitions.
	*/
	int						viewport[4];
    CGenericMatrix<float>   projection;
    CGenericMatrix<float>   modelview;
    float                   farPlane;
	std::vector<CGenericVector<float> >	clippingPlanes;

	/*
		Matrix stacks
	*/
	std::vector<CGenericMatrix<float>>   projection_stack;
	std::vector<CGenericMatrix<float>>   modelview_stack;


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

