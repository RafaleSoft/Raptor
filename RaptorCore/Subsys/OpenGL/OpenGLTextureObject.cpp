/***************************************************************************/
/*                                                                         */
/*  OpenGLTextureObject.cpp                                                */
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

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_OPENGLTEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLTextureObject.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)
    #include "GLHierarchy/TextureGenerator.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLTextureObject::COpenGLTextureObject(ITextureObject::TEXEL_TYPE type)
    :ITextureObject(type)
{
	texname = 0;
	target = 0;
	level = 0;
	m_filter = ITextureObject::CGL_UNFILTERED;
	m_pTexelGenerator = NULL;
    source[0] = source[1] = source[2] = source[3] = 0;
}

COpenGLTextureObject::COpenGLTextureObject(const COpenGLTextureObject& rsh)
	:ITextureObject(rsh)
{
	texname = 0;
	target = rsh.target;
	level = rsh.level;
	m_filter = rsh.m_filter;
	m_pTexelGenerator = rsh.m_pTexelGenerator;
    source[0] = source[1] = source[2] = source[3] = 0;
}

COpenGLTextureObject::~COpenGLTextureObject()
{
    if (texname != 0)
	    glDeleteTextures(1,&texname);
}


void COpenGLTextureObject::glvkRender()
{
    if (texname == 0)   // much faster
		return;

	glBindTexture(target,texname);
	
	ITextureGenerator::GENERATOR_KIND kind = ITextureGenerator::NONE;
	if (NULL != m_pTexelGenerator)
		kind = m_pTexelGenerator->getKind();
	if ((kind == ITextureGenerator::BUFFERED) || (kind == ITextureGenerator::ANIMATED))
	{
        m_pTexelGenerator->glGenerate(this, source[0], source[1], source[2], source[3]);
    }

    CATCH_GL_ERROR
}

ITextureObject::CUBE_FACE COpenGLTextureObject::getCurrentCubeFace(void) const
{
#if defined(GL_ARB_texture_cube_map)
	if ((target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB) && (target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB))
		return (ITextureObject::CUBE_FACE)(target - GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB);
	else
#endif
		return CGL_CUBEMAP_NONE;
}

void COpenGLTextureObject::selectCubeFace(CUBE_FACE face)
{
#if defined(GL_ARB_texture_cube_map)
	if ((target == GL_TEXTURE_CUBE_MAP_ARB) ||
		((target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB) && (target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB)))
	{
		if (face == CGL_CUBEMAP_NONE)
			target = GL_TEXTURE_CUBE_MAP_ARB;
		else
			target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + (face - CGL_CUBEMAP_PX);
	}
#endif
}

void COpenGLTextureObject::glvkUpdateFilter(ITextureObject::TEXTURE_FILTER F)
{
	GLint	mag_filter = GL_NEAREST;
	GLint	min_filter = GL_NEAREST;

    if (F == ITextureObject::CGL_BILINEAR)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR;
	}
	else if (F == ITextureObject::CGL_BILINEAR_MIPMAPPED)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
	}
	else if (F == ITextureObject::CGL_TRILINEAR)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
	}
	else if (F == ITextureObject::CGL_ANISOTROPIC)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
	}
	
	glTexParameteri(target,GL_TEXTURE_MAG_FILTER,mag_filter);
	glTexParameteri(target,GL_TEXTURE_MIN_FILTER,min_filter);

	m_filter = F;

	CATCH_GL_ERROR
}

void COpenGLTextureObject::glvkUpdateClamping(ITextureObject::CLAMP_MODE C)
{
	GLenum clamp_mode = GL_CLAMP;

	switch(C)
	{
		case ITextureObject::CGL_REPEAT:
			clamp_mode = GL_REPEAT;
			break;
		case ITextureObject::CGL_MIRROR_REPEAT:
#if defined(GL_ARB_texture_mirrored_repeat)
			clamp_mode = GL_MIRRORED_REPEAT_ARB;
#elif defined (GL_VERSION_1_4)
			clamp_mode = GL_MIRRORED_REPEAT;
#else
			clamp_mode = GL_REPEAT;
#endif
			break;
		case ITextureObject::CGL_MIRROR_EDGECLAMP:
#if defined(GL_ARB_texture_mirror_clamp_to_edge)
			clamp_mode = GL_MIRROR_CLAMP_TO_EDGE_ARB;
#elif defined (GL_VERSION_1_4)
			clamp_mode = GL_MIRROR_CLAMP_TO_EDGE;
#endif
			break;
		case ITextureObject::CGL_CLAMP:
			clamp_mode = GL_CLAMP;
			break;
		case ITextureObject::CGL_EDGECLAMP:
#if defined (GL_VERSION_1_2)
			clamp_mode = GL_CLAMP_TO_EDGE;
#elif defined (GL_EXT_texture_edge_clamp)
			clamp_mode = GL_CLAMP_TO_EDGE_EXT;
#else
			clamp_mode = GL_CLAMP;
#endif
			break;
	}

	glTexParameteri(target,GL_TEXTURE_WRAP_S,clamp_mode);
	glTexParameteri(target,GL_TEXTURE_WRAP_T,clamp_mode);
	if (getDepth() > 0)
	{
#if defined(GL_VERSION_1_2)
		glTexParameteri(target,GL_TEXTURE_WRAP_R,clamp_mode);
#elif defined(GL_EXT_texture3D)
		glTexParameteri(target,GL_TEXTURE_WRAP_R_EXT,clamp_mode);
#endif
	}
}


bool COpenGLTextureObject::setGenerationSize(uint32_t posx, uint32_t posy, uint32_t width, uint32_t height)
{
    if (m_pTexelGenerator == NULL)
        return false;

    unsigned int gH = m_pTexelGenerator->getGenerateHeight() ;
    unsigned int gW = m_pTexelGenerator->getGenerateWidth();
    if ((gH == 0) || (gW == 0))
        return false;

    int H = MIN(height,MIN(getHeight(),gH));
    int W = MIN(width,MIN(getWidth(),gW));

//  OGL does not raise errors for that. 
//  Specific check must then be done in each generator
//    if (posx + W > gW)
//        posx = gW - W;
//    if (posy + H > gH)
//        posy = gH - H;

    source[0] = posx;
    source[1] = posy;
    source[2] = W;
    source[3] = H;

    return true;
}


unsigned int COpenGLTextureObject::getTexelFormat(void) const
{
	//!	A default value, though it may be wrong.
	unsigned int res = GL_RGBA8;

	switch(getTexelType())
	{
		case CGL_COLOR24:
			res = GL_RGB8;
			break;
		case CGL_COLOR24_ALPHA:
			res = GL_RGBA8;
			break;
		case CGL_LIGHTMAP:
			res = GL_LUMINANCE8;
			break;
		case CGL_LIGHTMAP_ALPHA:
			res = GL_LUMINANCE8_ALPHA8;
			break;
		case CGL_LIGHTMAP16:
			res = GL_LUMINANCE16;
			break;
		case CGL_LIGHTMAP16_ALPHA:
			res = GL_LUMINANCE16_ALPHA16;
			break;
#if defined(GL_ARB_texture_float)
		case CGL_COLOR_FLOAT16:
			res = GL_RGB16F_ARB;
			break;
		case CGL_COLOR_FLOAT16_ALPHA:
			res = GL_RGBA16F_ARB;
			break;
		case CGL_COLOR_FLOAT32:
			res = GL_RGB32F_ARB;
			break;
		case CGL_COLOR_FLOAT32_ALPHA:
			res = GL_RGBA32F_ARB;
			break;
#endif
#ifdef GL_ARB_depth_texture
		case CGL_DEPTH8:
			res = GL_DEPTH_COMPONENT;
			break;
		case CGL_DEPTH16:
			res = GL_DEPTH_COMPONENT16_ARB;
			break;
		case CGL_DEPTH24:
			res = GL_DEPTH_COMPONENT24_ARB;
			break;
		case CGL_DEPTH32:
			res = GL_DEPTH_COMPONENT32_ARB;
			break;
#endif
#if defined(GL_EXT_packed_depth_stencil)
		case CGL_DEPTH24_STENCIL8:
			res = GL_DEPTH24_STENCIL8_EXT;
			break;
#endif
	}

	return res;
}
