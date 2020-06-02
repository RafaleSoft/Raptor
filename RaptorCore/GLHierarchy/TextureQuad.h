/***************************************************************************/
/*                                                                         */
/*  TextureQuad.h                                                          */
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


#if !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)
#define AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SIMPLEOBJECT_H__D7942271_77C5_4514_A44F_67F653C82A16__INCLUDED_)
	#include "SimpleObject.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class ITextureObject;


//!
//!	This class implements a simple object 
//!	when only a simple render is needed
//!	( simple glCalls when a Geometry is not needed )
class RAPTOR_API CTextureQuad : public CSimpleObject
{
public:
	typedef struct Attributes_t
	{
		GL_COORD_VERTEX		m_center;
		CColor::RGBA		m_color;
		GL_COORD_VERTEX		m_sizes;
	} Attributes;

	static const uint32_t max_texture_quad;


public:
	CTextureQuad();
	virtual ~CTextureQuad();

	//! Void manipulators as we do not need a Geometry,
	//!	We only add a an overridable glRender call
	//! @see CObject3D
	DECLARE_OBJECT3D_NOMANIPULATORS

	//!	Load the texture of the Quad.
	//! @return false if texture loading failed.
	bool glLoadTexture(const std::string &texname,bool compressed = false);

	//!	Update the texture object rendered by this quad.
	//! @param pTexture : a non NULL texture object pointer.
	//!	@return true if texture quad updated without errors or texture not null.
	bool setQuadTexture(ITextureObject *pTexture);

	//!	Set quad attributes.
	//!	@param center : defines the center of the quad for drawing (model view reference)
	//!	@param color : defines the base color of the quad (multiplied by texture color)
	//! @param sizes : defines absolute width & height of the quad.
	//!	@return false if failed to set attributes or max texture quads reached.
	bool glSetQuadAttributes(	const GL_COORD_VERTEX &center, 
								const CColor::RGBA& color, 
								const GL_COORD_VERTEX &sizes);


private:
	//!	Forbidden operators
	CTextureQuad(const CTextureQuad&);
	CTextureQuad& operator=(const CTextureQuad&);

	//!	Quad texture.
	CReference<ITextureObject>	m_rTexture;

	//!	Vertex Input State Resource binder
	void	*m_pBinder;

	//!	Attributes.
	static uint32_t		nb_quads;
	static uint32_t		max_index;
	static Attributes*	s_attributes;
	uint32_t			m_index;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)

