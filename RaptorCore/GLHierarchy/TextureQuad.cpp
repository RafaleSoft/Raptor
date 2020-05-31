/***************************************************************************/
/*                                                                         */
/*  TextureQuad.cpp                                                        */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)
	#include "TextureQuad.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_)
	#include "GLHierarchy/Reference.cxx"
#endif
#if !defined(AFX_OPENGLTEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLTextureObject.h"
#endif


RAPTOR_NAMESPACE


//!	The shader is common to all texture quads by definition.
const uint32_t CTextureQuad::max_texture_quad = 256;
uint32_t CTextureQuad::max_index = 0;
uint32_t CTextureQuad::nb_quads = 0;
CTextureQuad::Attributes*	CTextureQuad::s_attributes = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureQuad::CTextureQuad()
	:CSimpleObject("TEXTURE_QUAD"), m_index(max_index++), m_pBinder(NULL)
{
	nb_quads++;
}

CTextureQuad::~CTextureQuad()
{
	m_rTexture = NULL;
	nb_quads--;

	if (NULL != m_pBinder)
	{
		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
		delete binder;
	}

	if (0 == nb_quads)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		bool res = pAllocator->releaseVertices((float*)s_attributes);
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if (!res)
			RAPTOR_WARNING(	COpenGL::COpenGLClassID::GetClassId(),
							"Raptor Texture Quad Allocator released out of owning display !");
#endif
		s_attributes = NULL;
		max_index = 0;
	}
}

bool CTextureQuad::setQuadTexture(ITextureObject *pTexture)
{
	m_rTexture = pTexture;

	return (NULL != pTexture);
}

bool CTextureQuad::glSetQuadAttributes(	const GL_COORD_VERTEX &center,
										const CColor::RGBA& color,
										const GL_COORD_VERTEX &sizes)
{
	if ((max_index >= max_texture_quad) || (m_index > max_index))
		return false;

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	bool lock = pAllocator->isMemoryLocked();
	if (lock)
		pAllocator->glvkLockMemory(false);

	if (NULL == s_attributes)
	{
		size_t s = sizeof(Attributes) / sizeof(float);
		s_attributes = (Attributes*)(pAllocator->allocateVertices(max_texture_quad * s));
	}

	if (NULL == m_pBinder)
	{
		CResourceAllocator::CResourceBinder *binder = new CResourceAllocator::CResourceBinder();
		binder->setArray(CProgramParameters::POSITION, &s_attributes[m_index].m_center, 4, sizeof(Attributes));
		binder->setArray(CProgramParameters::PRIMARY_COLOR, &s_attributes[m_index].m_color, 4, sizeof(Attributes));
		binder->setArray(CProgramParameters::ADDITIONAL_PARAM1, &s_attributes[m_index].m_sizes, 4, sizeof(Attributes));
		binder->useVertexArrayObjects();
		m_pBinder = binder;
	}


	//! Update vertex buffer
	s_attributes = (Attributes*)pAllocator->glvkMapPointer((float*)s_attributes);
	CTextureQuad::Attributes& attrs = s_attributes[m_index];
	attrs.m_center = center;
	attrs.m_color = color;
	attrs.m_sizes = sizes;
	s_attributes = (Attributes*)pAllocator->glvkUnMapPointer((float*)s_attributes);
	
	if (lock)
		pAllocator->glvkLockMemory(true);

	return true;
}

bool CTextureQuad::glLoadTexture(const std::string &texname, bool compressed)
{
	if (!texname.empty())
	{
		CTextureFactory &Txt = CTextureFactory::getDefaultFactory();
		ITextureObject *T = Txt.glCreateTexture( ITextureObject::CGL_COLOR24_ALPHA,
												 ITextureObject::CGL_MULTIPLY,
												 ITextureObject::CGL_BILINEAR);
		
		CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
		bool lock = pAllocator->isMemoryLocked();
		if (lock)
			pAllocator->glvkLockMemory(false);

		Txt.glSetTransparency(T, 256);

		if (compressed)
			Txt.glLoadCompressedTexture(T, texname);
		else
			Txt.glLoadTexture(T, texname);

		m_rTexture = T;

		if (lock)
			pAllocator->glvkLockMemory(true);
	}
	else
		return false;
	
	//	For DEBUG purposes: regenerate the texture for RaptorData.pck
	/*
	p_Logo->glSetTransparency(256);
	if (Txt.glLoadTexture(p_Logo,"Raptor_logo_sml.tga",CGL_USER_MIPMAPPED))
	Txt.glExportCompressedTexture("Raptor_logo_sml.txt",p_Logo);
	*/

	return true;
}


void CTextureQuad::glRender(void)
{
	bool res = true;

	//!	Render (activate) shader and texture.
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	instance.m_pQuadShader->glRender();
	m_rTexture->glvkRender();

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();

	glDrawArrays(GL_POINTS, 0, 1);

	binder->glvkUnbindArrays();

	instance.m_pQuadShader->glStop();
}

