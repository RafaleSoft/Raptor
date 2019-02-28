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
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryProgram.h"
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


RAPTOR_NAMESPACE



static const std::string vp_src =
"#version 460 compatibility\n\
\n\
layout(location = 0) in vec4 i_Position; \n\
layout(location = 3) in vec4 i_Color; \n\
layout(location = 6) in vec4 i_Size; \n\
\n\
out vec4 size; \n\
out vec4 v_color; \n\
\n\
void main (void) \n\
{\n\
	vec4 pos = vec4(vec3(i_Position.xyz),1.0); \n\
	gl_Position =  gl_ModelViewProjectionMatrix * pos; \n\
	size = i_Size; \n\
	v_color =  i_Color;\n\
}";

static const std::string gp_src =
"#version 460\n\
\n\
//	Expect the geometry shader extension to be available, warn if not. \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
in vec4 size[]; \n\
in vec4 v_color[]; \n\
\n\
layout(points) in; \n\
layout(triangle_strip, max_vertices=4) out; \n\
layout(location = 1) out vec4 g_TexCoord[1]; \n\
out vec4 g_color; \n\
\n\
void main() \n\
{\n\
	g_color = v_color[0]; \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(-size[0].x, -size[0].y, 0.0, 0.0); \n\
	g_TexCoord[0] = vec4(0.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(size[0].x,-size[0].y,0.0,0.0); \n\
	g_TexCoord[0] = vec4(1.0,0.0,0.0,0.0); \n\
	EmitVertex(); \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(-size[0].x,size[0].y,0.0,0.0); \n\
	g_TexCoord[0] = vec4(0.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	\n\
	gl_Position = gl_in[0].gl_Position + vec4(size[0].x,size[0].y,0.0,0.0); \n\
	g_TexCoord[0] = vec4(1.0,1.0,0.0,0.0); \n\
	EmitVertex(); \n\
	\n\
	EndPrimitive(); \n\
}";

static const std::string fp_src =
"#version 460\n\
\n\
uniform	sampler2D diffuseMap; \n\
\n\
in vec4 g_color; \n\
layout(location = 1) in vec4 g_TexCoord; \n\
layout(location = 0) out vec4 o_Color;	\n\
\n\
void main (void) \n\
{\n\
	o_Color = g_color * texture2D(diffuseMap,vec2(g_TexCoord.st)); \n\
}";


//!	The shader is common to all texture quads by definition.
const uint32_t CTextureQuad::max_texture_quad = 256;
CShader	*CTextureQuad::m_pShader = NULL;
uint32_t CTextureQuad::max_index = 0;
CTextureQuad::Attributes*	CTextureQuad::s_attributes = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureQuad::CTextureQuad()
	:CSimpleObject("TEXTURE_QUAD"), m_index(max_index++)
{

}

CTextureQuad::~CTextureQuad()
{
	delete m_pTexture;
}

bool CTextureQuad::glSetQuadAttributes(	const GL_COORD_VERTEX &center,
										const CColor::RGBA& color,
										const GL_COORD_VERTEX &sizes)
{
	if ((max_index >= max_texture_quad) || (m_index > max_index))
		return false;

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	
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
	
		T->glSetTransparency(256);

		if (compressed)
			Txt.glLoadCompressedTexture(T, texname);
		else
		{
			CVaArray<CImage::IImageOP::OP_KIND> iops;
			Txt.glLoadTexture(T, texname, iops);
		}

		m_pTexture = T;
	}
	else
		return false;
	
	//	For DEBUG purposes: regenerate the texture for RaptorData.pck
	/*
	p_Logo->glSetTransparency(256);
	if (Txt.glLoadTexture(p_Logo,"Raptor_logo_sml.tga",CGL_USER_MIPMAPPED))
	Txt.glExportCompressedTexture("Raptor_logo_sml.txt",p_Logo);
	*/

	bool res = true;
	if (NULL == m_pShader)
	{
		m_pShader = new CShader(getName() + "_SHADER");

		CVertexProgram *vp = m_pShader->glGetVertexProgram();
		res = vp->glLoadProgram(vp_src);

		CGeometryProgram *gp = m_pShader->glGetGeometryProgram();
		gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);
		res = res & gp->glLoadProgram(gp_src);

		CFragmentProgram *fs = m_pShader->glGetFragmentProgram();
		res = res & fs->glLoadProgram(fp_src);
		CProgramParameters params;
		params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

		res = res & m_pShader->glCompileShader();
	}

	return res;
}


void CTextureQuad::glRender(void)
{
	if (NULL == m_pTexture)
		return;
	if (NULL == m_pShader)
		return;

	//!	Render (activate) shader and texture.
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	m_pShader->glRender();
	m_pTexture->glvkRender();

CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
pAllocator->glvkLockMemory(true);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Attributes), &s_attributes[m_index].m_center);
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
	pExtensions->glVertexAttribPointerARB(CProgramParameters::PRIMARY_COLOR, 4, GL_FLOAT, false, sizeof(Attributes), &s_attributes[m_index].m_color);
	pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);
	pExtensions->glVertexAttribPointerARB(CProgramParameters::ADDITIONAL_PARAM1, 4, GL_FLOAT, false, sizeof(Attributes), &s_attributes[m_index].m_sizes);

	glDrawArrays(GL_POINTS, 0, 1);

	glDisableClientState(GL_VERTEX_ARRAY);
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
	pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);

	m_pShader->glStop();

pAllocator->glvkLockMemory(false);
}

