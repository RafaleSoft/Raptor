// TextureObject.cpp: implementation of the CTextureObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "TextureObject.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_TEXTUREGENERATOR_H__214CB62C_FE2E_4737_9EA8_CE5D97F16093__INCLUDED_)
    #include "TextureGenerator.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureObject::CTextureObject(TEXEL_TYPE type)
    :ITextureObject(type)
{
	texname = 0;
	target = 0;
	level = 0;
	env_mode = GL_REPLACE;
	m_filter = ITextureObject::CGL_UNFILTERED;
	m_pTexelGenerator = NULL;
    aniso_level = 0.0f;
    source[0] = source[1] = source[2] = source[3] = 0;
#if defined(VK_VERSION_1_0)
	vk_texname = VK_NULL_HANDLE;
#endif
}

CTextureObject::CTextureObject(const CTextureObject& rsh)
	:ITextureObject(rsh)
{
	texname = 0;
	target = rsh.target;
	level = rsh.level;
	env_mode = rsh.env_mode;
	m_filter = rsh.m_filter;
	m_pTexelGenerator = rsh.m_pTexelGenerator;
    aniso_level = rsh.aniso_level;
    source[0] = source[1] = source[2] = source[3] = 0;
#if defined(VK_VERSION_1_0)
	vk_texname = VK_NULL_HANDLE;
#endif
}

CTextureObject::~CTextureObject()
{
    if (texname != 0)
	    glDeleteTextures(1,&texname);
}


void CTextureObject::glvkRender()
{
    if (texname == 0)   // much faster
		return;

	glBindTexture(target,texname);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,env_mode);

	
	ITextureGenerator::GENERATOR_KIND kind = ITextureGenerator::NONE;
	if (NULL != m_pTexelGenerator)
		kind = m_pTexelGenerator->getKind();
	if ((kind == ITextureGenerator::BUFFERED) ||
		(kind == ITextureGenerator::ANIMATED))
	{
        m_pTexelGenerator->glGenerate(this);
    }

    CATCH_GL_ERROR
}

CTextureObject::CUBE_FACE CTextureObject::getCurrentCubeFace(void) const
{
#if defined(GL_ARB_texture_cube_map)
	if ((target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB) && (target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB))
		return (CTextureObject::CUBE_FACE)(target-GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB);
	else
#endif
		return CGL_CUBEMAP_NONE;
}

void CTextureObject::selectCubeFace(CUBE_FACE face)
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

void CTextureObject::setFunction(CTextureObject::TEXTURE_FUNCTION F)
{
    switch(F)
	{
		case CTextureObject::CGL_OPAQUE:
			env_mode = GL_REPLACE;
			break;
		case CTextureObject::CGL_MULTIPLY:
			env_mode = GL_MODULATE;
			break;
		case CTextureObject::CGL_ALPHA_TRANSPARENT:
			env_mode = GL_DECAL;
			break;
		case CTextureObject::CGL_CONSTANT_BLENDED:
			env_mode = GL_BLEND;
			break;
	}
}

CTextureObject::TEXTURE_FUNCTION CTextureObject::getFunction(void) const
{
    TEXTURE_FUNCTION res = CTextureObject::CGL_OPAQUE;

    switch(env_mode)
	{
		case GL_REPLACE:
			res = CTextureObject::CGL_OPAQUE;
			break;
		case GL_MODULATE:
			res = CTextureObject::CGL_MULTIPLY;
			break;
		case GL_DECAL:
			res = CTextureObject::CGL_ALPHA_TRANSPARENT;
			break;
		case GL_BLEND:
			res = CTextureObject::CGL_CONSTANT_BLENDED;
			break;
	}

    return res;
}

void CTextureObject::glvkUpdateFilter(ITextureObject::TEXTURE_FILTER F)
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

void CTextureObject::glvkUpdateClamping(ITextureObject::CLAMP_MODE C)
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

void CTextureObject::glSetTransparentColor(unsigned char r, unsigned char g, unsigned char b)
{
    if ((target & GL_TEXTURE_2D) == GL_TEXTURE_2D)
    {
	    glBindTexture(target,texname);

        GLint currentWidth = 0;
        GLint currentHeight = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &currentWidth);
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &currentHeight);

	    GLubyte *data = new GLubyte[currentWidth*currentHeight*4];

	    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
        
	    for (int i=0 ; i<currentWidth*currentHeight*4 ; i+=4)
        {
		    if ((data[i] == r) && (data[i+1] == g) && (data[i+2] == b))
				data[i+3] = 0;
        }

	    glTexSubImage2D( target, level,
					     0,	0, currentWidth,	currentHeight,
					     GL_RGBA, GL_UNSIGNED_BYTE, data);

	    CATCH_GL_ERROR

	    delete [] data;
    }
}


void CTextureObject::glSetTransparency(unsigned int	alpha)
{
    m_alpha = alpha;

    if ((target & GL_TEXTURE_2D) == GL_TEXTURE_2D)
    {
        GLint currentWidth = 0;
        GLint currentHeight = 0;
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &currentWidth);
        glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &currentHeight);

        if ((currentWidth != 0) && (currentHeight != 0))
        {
			setSize(currentWidth, currentHeight, getDepth());

            // Here currentWidth ( & currentHeight) sould be equal to m_width ( & m_height)
            GLubyte *data = new unsigned char[currentWidth*currentHeight*4];

            glGetTexImage(target,level,GL_RGBA,GL_UNSIGNED_BYTE,data);

            for (int i=3;i<currentWidth*currentHeight;i+=4)
                data[i] = alpha;

            glTexSubImage2D(target, level,
							0,	0, currentWidth,	currentHeight,
							GL_RGBA, GL_UNSIGNED_BYTE, data);

            delete [] data;
        }
    }

    CATCH_GL_ERROR
}

bool CTextureObject::setGenerationSize(int posx, int posy, unsigned int width, unsigned int height)
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

void CTextureObject::getGenerationSize(   int &posx, int &posy, int &width, int &height) const
{
    posx = source[0];
    posy = source[1];
    width = source[2];
    height = source[3];
}

unsigned int CTextureObject::getTexelFormat(void) const
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
