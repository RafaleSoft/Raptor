// DefaultImageScaler.cpp: implementation of the CDefaultImageScaler class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)
    #include "DefaultImageScaler.h"
#endif
#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif


#include <GL/glu.h>	// gluScaleImage

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDefaultImageScaler::CDefaultImageScaler(float sx, float sy)
	: scale_x(sx), scale_y(sy)
{

}

CDefaultImageScaler::CDefaultImageScaler(const CDefaultImageScaler& scaler)
	: scale_x(scaler.scale_x), scale_y(scaler.scale_y)
{
	
}

CDefaultImageScaler& CDefaultImageScaler::operator =(const CDefaultImageScaler& scaler)
{
	scale_x = scaler.scale_x;
	scale_y = scaler.scale_y;
	return *this;
}

CDefaultImageScaler::~CDefaultImageScaler()
{

}

bool CDefaultImageScaler::apply(CImage* const src, const operation_param_t& param) const
{
	unsigned int powx = 1;
	unsigned int powy = 1;

	while (powx < src->getWidth())
		powx *= 2;
	while (powy < src->getHeight())
		powy *= 2;

	if ((powx != src->getWidth()) || (powy != src->getHeight()))
	{
		void *pixels = NULL;

		unsigned int elemSize = 1;
		unsigned char* pPixels = src->getPixels();

		//! Allocate a destination bloc for the scaled result
		if (pPixels != NULL)
		{
			CHostMemoryManager::Allocator<unsigned char> allocator;
			pixels = allocator.allocate(powx*powy * 4);
		}
		else
		{
			elemSize = 4;
			float *pfTexels = src->getFloatPixels();
			if (pfTexels != NULL)
			{
				CHostMemoryManager::Allocator<float> allocator;
				pixels = allocator.allocate(powx*powy * 4);
			}
			else
				return false;
		}

		//! We keep the same type in & out of pixel data
		//! to be able to control values with accuracy.
		gluScaleImage(src->getBufferFormat(),
					  src->getWidth(),
					  src->getHeight(),
					  src->getBufferType(),
					  pPixels,
					  powx,
					  powy,
					  src->getBufferType(),
					  pixels);

		src->allocatePixels(powx,powy,src->getPixelType());
		pPixels = src->getPixels();

		memcpy(pPixels, pixels, powx*powy * 4 * elemSize);

		CHostMemoryManager::GetInstance()->garbage(pixels);
	}

	return true;
}
