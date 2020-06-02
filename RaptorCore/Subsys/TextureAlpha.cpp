/***************************************************************************/
/*                                                                         */
/*  TextureAlpha.cpp                                                       */
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

#if !defined(AFX_TEXTUREALPHA_H__479A98C2_8277_4153_B96F_0706D8FB8A8A__INCLUDED_)
	#include "TextureAlpha.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureAlpha::CTextureAlpha(uint32_t t)
	:m_transparency(t)
{
}


CTextureAlpha::CTextureAlpha(const CTextureAlpha& alpha)
	: m_transparency(alpha.m_transparency)
{

}

CTextureAlpha& CTextureAlpha::operator =(const CTextureAlpha& alpha)
{
	m_transparency = alpha.m_transparency;
	return *this;
}


CTextureAlpha::~CTextureAlpha()
{
}


bool CTextureAlpha::apply(CImage* const src) const
{
	if (NULL == src)
		return false;

	uint32_t alpha = m_transparency;

	uint8_t* ppixels = src->getPixels();
	if (ppixels != NULL)
	{
		uint32_t size = 4 * src->getWidth() * src->getHeight() * src->getLayers();
		for (uint32_t i = 0; i < size; i += 4)
		{
			if (alpha > 255)
			{
				if ((ppixels[i] == 0) && (ppixels[i + 1] == 0) && (ppixels[i + 2] == 0))
					ppixels[i + 3] = 0;
				else
					ppixels[i + 3] = 255;
			}
			else if (alpha > 0)
			{
				ppixels[i + 3] = (uint8_t)(alpha & 0xFF);
			}
			else
			{
				ppixels[i + 3] = (uint8_t)((ppixels[i] + ppixels[i + 1] + ppixels[i + 2]) / 3);
			}
		}
		return true;
	}
	else
	{
		float* fpixels = src->getFloatPixels();
		if (fpixels != NULL)
		{
			uint32_t size = 4 * src->getWidth() * src->getHeight() * src->getLayers();
			for (uint32_t i = 0; i < size; i += 4)
			{
				if (alpha > 255)
				{
					if ((fpixels[i] == 0) && (fpixels[i + 1] == 0) && (fpixels[i + 2] == 0))
						fpixels[i + 3] = 0;
					else
						fpixels[i + 3] = 1.0f;
				}
				else if (alpha > 0)
				{
					fpixels[i + 3] = alpha * ONE_OVER_255_F;
				}
				else
				{
					fpixels[i + 3] = (fpixels[i] + fpixels[i + 1] + fpixels[i + 2]) / 3;
				}
			}
			return true;
		}
		else
			return false;
	}
}
