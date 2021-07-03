/***************************************************************************/
/*                                                                         */
/*  ImageScaler.cpp                                                        */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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

#if !defined(AFX_IMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)
    #include "ImageScaler.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageScaler::CImageScaler(float sx, float sy)
	: scale_x(sx), scale_y(sy)
{

}

CImageScaler::CImageScaler(const CImageScaler& scaler)
	: scale_x(scaler.scale_x), scale_y(scaler.scale_y)
{
	
}

CImageScaler& CImageScaler::operator =(const CImageScaler& scaler)
{
	scale_x = scaler.scale_x;
	scale_y = scaler.scale_y;
	return *this;
}

CImageScaler::~CImageScaler()
{

}

bool CImageScaler::apply(CImage* const src) const
{
	uint8_t *pixels = src->getPixels();
	if (NULL != pixels)
	{
		CImage resize;
		resize.allocatePixels(scale_x, scale_y);
		uint8_t *dst = resize.getPixels();
		memset(dst, 255, scale_x * scale_y * 4);

		int32_t w = src->getWidth();
		int32_t h = src->getHeight();
		float dw = (float)(src->getWidth()) / (float)scale_x;
		float dh = (float)(src->getHeight()) / (float)scale_y;

		for (size_t j = 0; j < scale_y; j++)
		{
			for (size_t i = 0; i < scale_x; i++)
			{
				float dx = (float)i * dw;
				float dy = (float)j * dh;

				uint16_t r = 0;
				uint16_t g = 0;
				uint16_t b = 0;
				uint16_t a = 0;

				//
				// Box filter on 4 pixels
				//
				int I = floor(dx);
				int J = floor(dy);
				float factx = (dx - I);
				float facty = (dy - J);

				int offset_src = ((J * w) + I) * 4;
				float fxy = (1 - factx) * (1 - facty);
				r += fxy * pixels[offset_src];
				g += fxy * pixels[offset_src + 1];
				b += fxy * pixels[offset_src + 2];
				a += fxy * pixels[offset_src + 3];

				if (I < w - 1)
				{
					offset_src = ((J * w) + I + 1) * 4;
					fxy = factx * (1 - facty);
					r += fxy * pixels[offset_src];
					g += fxy * pixels[offset_src + 1];
					b += fxy * pixels[offset_src + 2];
					a += fxy * pixels[offset_src + 3];
				}

				if (J < h - 1)
				{
					offset_src = (((J + 1) * w) + I) * 4;
					fxy = (1 - factx) * facty;
					r += fxy * pixels[offset_src];
					g += fxy * pixels[offset_src + 1];
					b += fxy * pixels[offset_src + 2];
					a += fxy * pixels[offset_src + 3];
				}

				if ((J < h - 1) && (I < w - 1))
				{
					offset_src = (((J + 1) * w) + I + 1) * 4;
					fxy = factx * facty;
					r += fxy * pixels[offset_src];
					g += fxy * pixels[offset_src + 1];
					b += fxy * pixels[offset_src + 2];
					a += fxy * pixels[offset_src + 3];
				}

				int offset_dst = ((j * scale_x) + i) * 4;

				dst[offset_dst] = r;
				dst[offset_dst + 1] = g;
				dst[offset_dst + 2] = b;
				dst[offset_dst + 3] = a;
			}
		}

		src->releasePixels();
		src->allocatePixels(scale_x, scale_y);
		memcpy(src->getPixels(), dst, scale_x * scale_y * 4);

		return true;
	}
	else
		return false;
}

