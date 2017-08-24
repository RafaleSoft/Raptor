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
#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
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
    :m_type(type),m_bufferType(type)
{
	m_name = "<unknown>";
	texname = 0;
	target = 0;
	level = 0;
	m_width = 0;
	m_height = 0;
    m_depth = 0;
	texels = NULL;
	env_mode = GL_REPLACE;
	m_filter = CTextureObject::CGL_UNFILTERED;
	m_alpha = 255;
	m_pTexelGenerator = NULL;
    aniso_level = 0.0f;
    source[0] = source[1] = source[2] = source[3] = 0;
#if defined(VK_VERSION_1_0)
	vk_texname = VK_NULL_HANDLE;
#endif
}

CTextureObject::CTextureObject(const CTextureObject& rsh)
{
	m_name = rsh.m_name;
	texname = 0;
	target = rsh.target;
	level = rsh.level;
	m_width = rsh.m_width;
	m_height = rsh.m_height;
    m_depth = rsh.m_depth;
	texels = NULL;
	env_mode = rsh.env_mode;
	m_filter = rsh.m_filter;
	m_alpha = rsh.m_alpha;
	m_pTexelGenerator = rsh.m_pTexelGenerator;
    aniso_level = rsh.aniso_level;
    source[0] = source[1] = source[2] = source[3] = 0;
	m_type = rsh.m_type;
	m_bufferType = rsh.m_bufferType;
#if defined(VK_VERSION_1_0)
	vk_texname = VK_NULL_HANDLE;
#endif
}

CTextureObject::~CTextureObject()
{
    if (texname != 0)
	    glDeleteTextures(1,&texname);

	releaseTexels();
}


void CTextureObject::glRender()
{
    if (texname == 0)   // much faster
		return;

	glBindTexture((target & 0xFFFF),texname);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,env_mode);

	if (((target >> 16) == ITextureGenerator::BUFFERED) ||
        ((target >> 16) == ITextureGenerator::ANIMATED))
	{
        m_pTexelGenerator->glGenerate(this);
    }

    CATCH_GL_ERROR
}

void CTextureObject::setSize(unsigned int width, unsigned int height, unsigned int depth)
{
	if ((width == 0) || (height == 0) || (depth == 0))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "CTextureObject wrong size update");
#endif
		return;
	}

    if ((width != m_width) || (height != m_height) || (depth != m_depth))
    {
        releaseTexels();

        m_width = width;
        m_height = height;
        m_depth = depth;
    }
}

CTextureObject::CUBE_FACE CTextureObject::getCurrentCubeFace(void) const
{
	GLuint t = (target & 0xFFFF);
#if defined(GL_ARB_texture_cube_map)
	if ((t >= GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB) && (t <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB))
		return (CTextureObject::CUBE_FACE)(t-GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB);
	else
#endif
		return CGL_CUBEMAP_NONE;
}

void CTextureObject::selectCubeFace(CUBE_FACE face)
{
#if defined(GL_ARB_texture_cube_map)
	GLuint t = (target & 0xFFFF);

	if ((t == GL_TEXTURE_CUBE_MAP_ARB) ||
		((t >= GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB) && (t <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB)))
	{
		if (face == CGL_CUBEMAP_NONE)
			target = (target & 0xFFFF0000) + GL_TEXTURE_CUBE_MAP_ARB;
		else
			target = (target & 0xFFFF0000) + GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + (face - CGL_CUBEMAP_PX);
	}
#endif
}

unsigned char* CTextureObject::getTexels(void)
{
    if ((m_bufferType == CGL_COLOR24) ||
        (m_bufferType == CGL_COLOR24_ALPHA) ||
        (m_bufferType == CGL_LIGHTMAP) ||
        (m_bufferType == CGL_LIGHTMAP_ALPHA) ||
		(m_bufferType == CGL_LIGHTMAP16) ||
        (m_bufferType == CGL_LIGHTMAP16_ALPHA) ||
		(m_bufferType == CGL_DEPTH24_STENCIL8))
    {
        return (unsigned char*)texels;
    }
    else
    {
        return NULL;
    }
}


float* CTextureObject::getFloatTexels(void)
{
	if ((m_bufferType == CGL_COLOR_FLOAT32) ||
        (m_bufferType == CGL_COLOR_FLOAT32_ALPHA) ||
        (m_bufferType == CGL_COLOR_FLOAT16) ||
        (m_bufferType == CGL_COLOR_FLOAT16_ALPHA) ||
		(m_bufferType == CGL_DEPTH8) ||
		(m_bufferType == CGL_DEPTH16) ||
        (m_bufferType == CGL_DEPTH24) ||
        (m_bufferType == CGL_DEPTH32))
    {
		return (float*)texels;
    }
    else
    {
        return NULL;
    }
}

void CTextureObject::releaseTexels(void)
{
	if (texels != NULL)
		CHostMemoryManager::GetInstance()->garbage(texels);

	texels = NULL;
}

void CTextureObject::allocateTexels(TEXEL_TYPE type)
{
    //   reserve default space for at least rgba type, even if it larger than necessary.
    unsigned int size = 4 * m_width * m_height * m_depth;

    if (size > 0)
    {
		// always clear memory, even is type is identical:
		// the texture may have been resized.
		releaseTexels();

		switch(type)
		{
			case CGL_COLOR24:
			case CGL_COLOR24_ALPHA:
			case CGL_LIGHTMAP:
			case CGL_LIGHTMAP_ALPHA:
			case CGL_LIGHTMAP16:
			case CGL_LIGHTMAP16_ALPHA:
			case CGL_DEPTH24_STENCIL8:
			{
				CHostMemoryManager::Allocator<unsigned char> allocator;
				texels = allocator.allocate(size); 
				break;
			}
			case CGL_COLOR_FLOAT16:
			case CGL_COLOR_FLOAT16_ALPHA:
			case CGL_COLOR_FLOAT32:
			case CGL_COLOR_FLOAT32_ALPHA:
			case CGL_DEPTH8:
			case CGL_DEPTH16:
			case CGL_DEPTH24:
			case CGL_DEPTH32:
			{
				CHostMemoryManager::Allocator<float> allocator;
				texels = allocator.allocate(size);
				break;
			}
		}
    }

    m_bufferType = type;
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

void CTextureObject::glUpdateFilter(CTextureObject::TEXTURE_FILTER F)
{
	GLint	mag_filter = GL_NEAREST;
	GLint	min_filter = GL_NEAREST;

    if (F == CTextureObject::CGL_BILINEAR)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR;
	}
	else if (F == CTextureObject::CGL_BILINEAR_MIPMAPPED)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
	}
	else if (F == CTextureObject::CGL_TRILINEAR)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
	}
	else if (F == CTextureObject::CGL_ANISOTROPIC)
	{
		mag_filter = GL_LINEAR;
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
	}
	
	glTexParameteri(target & 0xFFFF,GL_TEXTURE_MAG_FILTER,mag_filter);
	glTexParameteri(target & 0xFFFF,GL_TEXTURE_MIN_FILTER,min_filter);

	m_filter = F;

	CATCH_GL_ERROR
}

void CTextureObject::glUpdateClamping(CLAMP_MODE C)
{
	GLenum clamp_mode = GL_CLAMP;

	switch(C)
	{
		case CGL_REPEAT:
			clamp_mode = GL_REPEAT;
			break;
		case CGL_CLAMP:
			clamp_mode = GL_CLAMP;
			break;
		case CGL_EDGECLAMP:
#if defined (GL_VERSION_1_2)
			clamp_mode = GL_CLAMP_TO_EDGE;
#elif defined (GL_EXT_texture_edge_clamp)
			clamp_mode = GL_CLAMP_TO_EDGE_EXT;
#else
			clamp_mode = GL_CLAMP;
#endif
			break;
	}

	glTexParameteri(target & 0xFFFF,GL_TEXTURE_WRAP_S,clamp_mode);
	glTexParameteri(target & 0xFFFF,GL_TEXTURE_WRAP_T,clamp_mode);
	if  (m_depth > 0)
	{
#if defined(GL_VERSION_1_2)
		glTexParameteri(target & 0xFFFF,GL_TEXTURE_WRAP_R,clamp_mode);
#elif defined(GL_EXT_texture3D)
		glTexParameteri(target & 0xFFFF,GL_TEXTURE_WRAP_R_EXT,clamp_mode);
#endif
	}
}

void CTextureObject::glSetTransparentColor(unsigned char r, unsigned char g, unsigned char b)
{
    if ((target & GL_TEXTURE_2D) == GL_TEXTURE_2D)
    {
	    glBindTexture(target & 0xFFFF,texname);

        //! Free any allocated texels, structure may have changed;
		releaseTexels();

        GLint currentWidth = 0;
        GLint currentHeight = 0;
        glGetTexLevelParameteriv(target & 0xFFFF, level, GL_TEXTURE_WIDTH, &currentWidth);
        glGetTexLevelParameteriv(target & 0xFFFF, level, GL_TEXTURE_HEIGHT, &currentHeight);

	    GLubyte *data = new GLubyte[currentWidth*currentHeight*4];

	    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
        
	    for (int i=0 ; i<currentWidth*currentHeight*4 ; i+=4)
        {
		    if ((data[i] == r) && (data[i+1] == g) && (data[i+2] == b))
				    data[i+3] = 0;
        }

	    glTexSubImage2D( (target & 0xFFFF), level,
					     0,	0, currentWidth,	currentHeight,
					     GL_RGBA, GL_UNSIGNED_BYTE, data);

	    CATCH_GL_ERROR

	    delete [] data;
    }
}


void CTextureObject::glSetTransparency(unsigned int	alpha)
{
    m_alpha = alpha;

    if (((target & 0xFFFF) & GL_TEXTURE_2D) == GL_TEXTURE_2D)
    {
        GLint currentWidth = 0;
        GLint currentHeight = 0;
        glGetTexLevelParameteriv(target & 0xFFFF, level, GL_TEXTURE_WIDTH, &currentWidth);
        glGetTexLevelParameteriv(target & 0xFFFF, level, GL_TEXTURE_HEIGHT, &currentHeight);

        if ((currentWidth != 0) && (currentHeight != 0))
        {
            m_width = currentWidth;
            m_height = currentHeight;

            //! Free any allocated texels, structure may have changed;
            releaseTexels();

            // Here currentWidth ( & currentHeight) sould be equal to m_width ( & m_height)
            GLubyte *data = new unsigned char[currentWidth*currentHeight*4];

            glGetTexImage(target&0xFFFF,level,GL_RGBA,GL_UNSIGNED_BYTE,data);

            for (int i=3;i<currentWidth*currentHeight;i+=4)
                data[i] = alpha;

            glTexSubImage2D((target & 0xFFFF), level,
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

    int H = MIN(height,MIN(m_height,gH));
    int W = MIN(width,MIN(m_width,gW));

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



unsigned int CTextureObject::getBufferType(void) const
{
	//!	A default value, though it may be wrong.
	unsigned int res = GL_UNSIGNED_BYTE;

	switch(m_bufferType)
	{
		case CGL_COLOR24:
        case CGL_COLOR24_ALPHA:
        case CGL_LIGHTMAP:
        case CGL_LIGHTMAP_ALPHA:
        {
			res = GL_UNSIGNED_BYTE;
            break;
        }
		case CGL_LIGHTMAP16:
		case CGL_LIGHTMAP16_ALPHA:
		{
			res = GL_UNSIGNED_SHORT;
            break;
		}
#if defined(GL_ARB_half_float_pixel)
        case CGL_COLOR_FLOAT16:
        case CGL_COLOR_FLOAT16_ALPHA:
        {
            res = GL_HALF_FLOAT_ARB;
            break;
        }
#else
        case CGL_COLOR_FLOAT16:
        case CGL_COLOR_FLOAT16_ALPHA:
#endif
        case CGL_COLOR_FLOAT32:
        case CGL_COLOR_FLOAT32_ALPHA:
        case CGL_DEPTH8:
        case CGL_DEPTH16:
        case CGL_DEPTH24:
        case CGL_DEPTH32:
        {
			res = GL_FLOAT;
            break;
        }
#if defined(GL_EXT_packed_depth_stencil)
		case CGL_DEPTH24_STENCIL8:
		{
			res = GL_UNSIGNED_INT_24_8_EXT;
			break;
		}
#endif
    }

	return res;
}

unsigned int CTextureObject::getBufferFormat(void) const
{
	//!	A default value, though it may be wrong.
	unsigned int res = GL_RGBA;

	switch(m_bufferType)
	{
		case CGL_COLOR24:
        case CGL_COLOR_FLOAT16:
		case CGL_COLOR_FLOAT32:
		{
			res = GL_RGB;
			break;
		}
        case CGL_COLOR24_ALPHA:
        case CGL_COLOR_FLOAT16_ALPHA:
		case CGL_COLOR_FLOAT32_ALPHA:
		{
			res = GL_RGBA;
			break;
		}
        case CGL_LIGHTMAP:
		case CGL_LIGHTMAP16:
        {
			res = GL_LUMINANCE;
            break;
        }
		case CGL_LIGHTMAP_ALPHA:
		case CGL_LIGHTMAP16_ALPHA:
		{
			res = GL_LUMINANCE_ALPHA;
            break;
		}
        case CGL_DEPTH8:
        case CGL_DEPTH16:
        case CGL_DEPTH24:
        case CGL_DEPTH32:
        {
			res = GL_DEPTH_COMPONENT;
            break;
        }
#if defined(GL_EXT_packed_depth_stencil)
		case CGL_DEPTH24_STENCIL8:
		{
			res = GL_DEPTH_STENCIL_EXT;
			break;
		}
#endif
    }

	return res;
}

unsigned int CTextureObject::getTexelFormat(void) const
{
	//!	A default value, though it may be wrong.
	unsigned int res = GL_RGBA8;

	switch(m_type)
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
