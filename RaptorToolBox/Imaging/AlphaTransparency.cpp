/***************************************************************************/
/*                                                                         */
/*  AlphaTransparency.cpp                                                  */
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

#if !defined(AFX_ALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_)
	#include "AlphaTransparency.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlphaTransparency::CAlphaTransparency(uint32_t t)
	:m_transparency(t)
{
}


CAlphaTransparency::CAlphaTransparency(const CAlphaTransparency& alpha)
	: m_transparency(alpha.m_transparency)
{

}

CAlphaTransparency& CAlphaTransparency::operator =(const CAlphaTransparency& alpha)
{
	m_transparency = alpha.m_transparency;
	return *this;
}


CAlphaTransparency::~CAlphaTransparency()
{
}


bool CAlphaTransparency::apply(CImage* const src) const
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
