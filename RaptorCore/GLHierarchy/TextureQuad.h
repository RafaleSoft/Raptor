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
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "ITextureObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CShader;


//!
//!	This class implements a simple object 
//!	when only a simple render is needed
//!	( simple glCalls when a Geometry is not needed )
class RAPTOR_API CTextureQuad : public CSimpleObject
{
public:
	CTextureQuad();
	virtual ~CTextureQuad();

	//! Void manipulators as we do not need a Geometry,
	//!	We only add a an overridable glRender call
	//! @see CObject3D
	DECLARE_OBJECT3D_NOMANIPULATORS

	//!	Laod the texture of the Quad.
	//! @return false if texture loading failed.
	bool CTextureQuad::glLoadTexture(const std::string &texname,bool compressed = false);


private:
	//!	Forbidden operators
	CTextureQuad(const CTextureQuad&);
	CTextureQuad& operator=(const CTextureQuad&);

	static CShader				*m_pShader;
	ITextureObject				*m_pTexture;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)

