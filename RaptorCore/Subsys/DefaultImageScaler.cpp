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

CDefaultImageScaler::CDefaultImageScaler()
{

}

CDefaultImageScaler::~CDefaultImageScaler()
{

}

bool CDefaultImageScaler::apply(CTextureObject* const src,
								unsigned int,
								unsigned int texelFormat,
								unsigned int texelType,
								const CTextureFactoryConfig& config) const
{
	if (src->getDepth() != 0)
		return false;	//	Unsupported Texture3D resize.

	
	unsigned int powx=1;
    unsigned int powy=1;

	while (powx < src->getWidth())
		powx *= 2;	
	while (powy < src->getHeight())
		powy *= 2;
	
	if ((powx != src->getWidth()) || (powy != src->getHeight()))
	{
        void *texels = NULL;

        unsigned int elemSize = 1;
		unsigned char* pTexels = src->getTexels();

        //! Allocate a destination bloc for the scaled result
        if (pTexels != NULL)
        {
            CMemory::Allocator<unsigned char> allocator;
            texels = allocator.allocate(powx*powy*4);
        }
        else
        {
            elemSize = 4;
			float *pfTexels = src->getFloatTexels();
            if (pfTexels != NULL)
            {
                CMemory::Allocator<float> allocator;
                texels = allocator.allocate(powx*powy*4);
            }
            else
                return false;
        }

        //! We keep the same type in & out of pixel data
        //! to be able to control values with accuracy.
        gluScaleImage(	texelFormat, 
						src->getWidth(), 
						src->getHeight(), 
						texelType, 
						pTexels, 
						powx, 
						powy, 
						texelType, 
						texels);

        CTextureObject::TEXEL_TYPE tType = src->getTexelType();
        src->setSize(powx,powy);
        src->allocateTexels(tType);
		pTexels = src->getTexels();

        memcpy(pTexels,texels,powx*powy*4*elemSize);

        CMemory::GetInstance()->garbage(texels);
	}

    return true;
}


