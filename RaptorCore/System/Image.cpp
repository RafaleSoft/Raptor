/***************************************************************************/
/*                                                                         */
/*  Image.cpp                                                              */
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

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "Image.h"
#endif
#ifndef __CGLTYPES_HPP__
	#include "CGLTypes.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif


RAPTOR_NAMESPACE

std::map<std::string, CImage::IImageIO*>				CImage::IMAGE_KIND_IO;
std::map<CImage::IImageOP::OP_KIND, CImage::IImageOP*>	CImage::IMAGE_KIND_OP;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImage::CImage()
	:m_name(""), m_width(0), m_height(0), m_layers(1),
	m_pixelType(CGL_COLOR24_ALPHA), pixels(NULL)
{
}

CImage::~CImage()
{
	releasePixels();
}

CImage* CImage::createSubImage(size_t x, size_t y, size_t w, size_t h)
{
	if ((x > getWidth()) || (y > getHeight()))
		return NULL;

	if (x + w > getWidth())
		return NULL;
	if (y + h > getHeight())
		return NULL;

	if (NULL == pixels)
		return NULL;

	CImage *res = new CImage();
	if (res->allocatePixels(w, h, m_pixelType))
	{
		size_t y_end = y + h;
		size_t y_start = y;
		size_t x_start = x;
		size_t x_end = x + w;

		uint8_t *pixels2 = res->getPixels();
		if (NULL != pixels2)
		{
			uint8_t *src = getPixels();
			for (size_t v = y_start, j = 0; v < y_end; v++)
				for (size_t o = (w * v + x_start) * 4; o < (w * v + x_end) * 4; o++, j++)
					pixels2[j] = src[o];
		}
		else
		{
			float *pixels3 = res->getFloatPixels();
			if (NULL != pixels3)
			{
				float *src = getFloatPixels();
				for (size_t v = y_start, j = 0; v < y_end; v++)
					for (size_t o = (w * v + x_start) * 4; o < (w * v + x_end) * 4; o++, j++)
						pixels3[j] = src[o];
			}
		}
	}
		
	return res;
}

uint8_t* CImage::getPixels(uint32_t layer) const
{
	uint8_t* ret = NULL;
	if (layer >= m_layers)
		return ret;

	switch (m_pixelType)
	{
		case CGL_COLOR24:
		case CGL_COLOR24_ALPHA:
			ret = (uint8_t*)pixels + layer*(sizeof(uint32_t) * m_width*m_height);
			break;
		default:
			ret = NULL;
			break;
	}

	return ret;
}


float* CImage::getFloatPixels(uint32_t layer) const
{
	float* ret = NULL;
	if (layer >= m_layers)
		return ret;

	switch (m_pixelType)
	{
		case CGL_COLOR_FLOAT16:
		case CGL_COLOR_FLOAT16_ALPHA:
		case CGL_COLOR_FLOAT32:
		case CGL_COLOR_FLOAT32_ALPHA:
			ret = (float*)pixels + layer*(sizeof(float) * m_width*m_height);;
			break;
		default:
			ret = NULL;
			break;
	}

	return ret;
}

bool CImage::loadImage(const std::string &filename,
					   const CVaArray<CImage::IImageOP*>& ops)
					   //const CImage::IImageOP::operation_param_t& param)
{
	if (filename.empty())
		return false;

	bool ret = true;

	CImage::IImageIO *imager = CImage::getImageKindIO(filename);
	if (NULL != imager)
	{
		if (imager->loadImageFile(filename, this))
		{
			m_name = filename;

			/*std::map<IImageOP::OP_KIND, IImageOP*>::const_iterator it = IMAGE_KIND_OP.begin();
			while (it != IMAGE_KIND_OP.end())
			{
				if (ops.hasValue((*it).first))		//	CImage::IImageOP::OP_KIND
				{
					CImage::IImageOP* op = (*it).second;	//	CImage::IImageOP *
					op->apply(this); // , param);
				}

				it++;
			}*/
			for (size_t i = 0; i < ops.size();i++)
			{
				CImage::IImageOP *op = ops[i];
				op->apply(this);
			}
		}
		else	// load from file failed
		{
			CRaptorMessages::MessageArgument arg;
			arg.arg_sz = filename.data();
			vector<CRaptorMessages::MessageArgument> args;
			args.push_back(arg);
			Raptor::GetErrorManager()->generateRaptorError(CTextureFactory::CTextureFactoryClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_WARNING,
														   CRaptorMessages::ID_TEXTURE_MISS, args);
			ret = false;
		}
	}
	else
		ret = false;

	return ret;
}

void CImage::releasePixels(void)
{
	if (pixels != NULL)
		CHostMemoryManager::GetInstance()->garbage(pixels);

	pixels = NULL;
}

bool CImage::allocatePixels(uint32_t width, uint32_t height, uint32_t layers, PIXEL_TYPE type)
{
	m_layers = layers;

	return allocatePixels(width, height, type);
}

bool CImage::allocatePixels(uint32_t width, uint32_t height, PIXEL_TYPE type)
{
	//   reserve default space for at least rgba type, even if it larger than necessary.
	uint32_t size = 4 * width * height * m_layers;

	if (size > 0)
	{
		// always clear memory, even is type is identical:
		// the texture may have been resized.
		releasePixels();

		switch (type)
		{
			case CGL_COLOR24:
			case CGL_COLOR24_ALPHA:
			{
				CHostMemoryManager::Allocator<unsigned char> allocator;
				pixels = allocator.allocate(size);
				break;
			}
			case CGL_COLOR_FLOAT16:
			case CGL_COLOR_FLOAT16_ALPHA:
			case CGL_COLOR_FLOAT32:
			case CGL_COLOR_FLOAT32_ALPHA:
			{
				CHostMemoryManager::Allocator<float> allocator;
				pixels = allocator.allocate(size);
				break;
			}
		}
	}
	else
		return false;

	m_pixelType = type;
	m_width = width;
	m_height = height;

	return true;
}

void CImage::setImageKindIO(IImageIO *imager)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if ((imager == NULL) || (imager->getImageKind().empty()))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_PROCEDURE_FAILED);
		return;
	}
#endif

	vector<std::string> extensionKind = imager->getImageKind();
	for (size_t j=0;j<extensionKind.size();j++)
	{
		string ext;
		for (unsigned int i=0;i<extensionKind[j].size();i++)
			ext += toupper(extensionKind[j][i]);

		IMAGE_KIND_IO.insert(std::map<std::string,IImageIO*>::value_type(ext,imager));
	}
}

CImage::IImageIO* const CImage::getImageKindIO(const std::string &extension)
{
	//	extract the right image loader
    string ext = extension;
	std::string::size_type pos = ext.rfind('.');
    if (pos < ext.size())
        ext = ext.substr(pos+1);

    pos = 0;
	for (pos=0;pos<ext.size();pos++)
        ext[pos] = toupper(ext[pos]);

	std::map<std::string,IImageIO*>::const_iterator itr = IMAGE_KIND_IO.find(ext);
	if (IMAGE_KIND_IO.end() != itr)
		return (*itr).second;
	else 
		return NULL;
}


void CImage::setImageKindOP(IImageOP *op)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
    if (op == NULL)
	{
        Raptor::GetErrorManager()->generateRaptorError(	CTextureFactory::CTextureFactoryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														CRaptorMessages::ID_PROCEDURE_FAILED);
		return;
	}
#endif

	IMAGE_KIND_OP.insert(std::map<IImageOP::OP_KIND,IImageOP*>::value_type(op->getKind(),op));
}


CImage::IImageOP* const CImage::getImageKindOP(IImageOP::OP_KIND kind)
{
    std::map<IImageOP::OP_KIND,IImageOP*>::const_iterator itr = IMAGE_KIND_OP.find(kind);
	if (IMAGE_KIND_OP.end() != itr)
		return (*itr).second;
	else 
		return NULL;
}



unsigned int CImage::getBufferType(void) const
{
	//!	A default value, though it may be wrong.
	unsigned int res = GL_UNSIGNED_BYTE;

	switch (m_pixelType)
	{
		case CGL_COLOR24:
		case CGL_COLOR24_ALPHA:
		{
			res = GL_UNSIGNED_BYTE;
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
		{
			res = GL_FLOAT;
			break;
		}
	}

	return res;
}

unsigned int CImage::getBufferFormat(void) const
{
	//!	A default value, though it may be wrong.
	unsigned int res = GL_RGBA;

	switch (m_pixelType)
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
	}

	return res;
}
