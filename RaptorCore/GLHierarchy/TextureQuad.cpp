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
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryShader.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
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


RAPTOR_NAMESPACE


//!	The shader is common to all texture quads by definition.
const uint32_t CTextureQuad::max_texture_quad = 256;
CShader	*CTextureQuad::m_pShader = NULL;
uint32_t CTextureQuad::max_index = 0;
uint32_t CTextureQuad::nb_quads = 0;
CTextureQuad::Attributes*	CTextureQuad::s_attributes = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureQuad::CTextureQuad()
	:CSimpleObject("TEXTURE_QUAD"), m_index(max_index++)
{
	nb_quads++;
}

CTextureQuad::~CTextureQuad()
{
	m_rTexture = NULL;
	nb_quads--;
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

bool CTextureQuad::setQuadTexture(CTextureObject *pTexture)
{
	m_rTexture = pTexture;

	return (NULL != pTexture);
}

bool CTextureQuad::setQuadCenter(const GL_COORD_VERTEX &center)
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

	//! Update vertex buffer
	s_attributes = (Attributes*)pAllocator->glvkMapPointer((float*)s_attributes);
	CTextureQuad::Attributes& attrs = s_attributes[m_index];
	attrs.m_center = center;
	s_attributes = (Attributes*)pAllocator->glvkUnMapPointer((float*)s_attributes);

	if (lock)
		pAllocator->glvkLockMemory(true);

	return true;
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
		CTextureObject *T = Txt.glCreateTexture( ITextureObject::CGL_COLOR24_ALPHA,
												 CTextureObject::CGL_MULTIPLY,
												 ITextureObject::CGL_BILINEAR);
		
		CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
		bool lock = pAllocator->isMemoryLocked();
		if (lock)
			pAllocator->glvkLockMemory(false);

		T->glSetTransparency(256);

		if (compressed)
			Txt.glLoadCompressedTexture(T, texname);
		else
		{
			CVaArray<CImage::IImageOP::OP_KIND> iops;
			Txt.glLoadTexture(T, texname, iops);
		}

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
	if (NULL == m_pShader)
	{
		m_pShader = new CShader(getName() + "_SHADER");

		CVertexShader *vp = m_pShader->glGetVertexShader("TEXTURE_QUAD_VTX_PROGRAM");
		CGeometryShader *gp = m_pShader->glGetGeometryShader("TEXTURE_QUAD_GEO_PROGRAM");
		gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
		CFragmentProgram *fs = m_pShader->glGetFragmentProgram("TEXTURE_QUAD_TEX_PROGRAM");
		CProgramParameters params;
		params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

		res = res & m_pShader->glCompileShader();
	}

	//!	Render (activate) shader and texture.
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	m_pShader->glRender();
	m_rTexture->glvkRender();

	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::POSITION);
	pExtensions->glVertexAttribPointerARB(CProgramParameters::POSITION, 4, GL_FLOAT, false, sizeof(Attributes), &s_attributes[m_index].m_center);
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
	pExtensions->glVertexAttribPointerARB(CProgramParameters::PRIMARY_COLOR, 4, GL_FLOAT, false, sizeof(Attributes), &s_attributes[m_index].m_color);
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);
	pExtensions->glVertexAttribPointerARB(CProgramParameters::ADDITIONAL_PARAM1, 4, GL_FLOAT, false, sizeof(Attributes), &s_attributes[m_index].m_sizes);

	glDrawArrays(GL_POINTS, 0, 1);

	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::POSITION);
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);

	m_pShader->glStop();
}

