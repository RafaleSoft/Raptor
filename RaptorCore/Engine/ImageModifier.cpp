// ImageModifier.cpp: implementation of the CImageModifier class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_IMAGEMODIFIER_H__98DC74E1_150B_449D_8BA0_C9CB8F9AEA3D__INCLUDED_)
	#include "ImageModifier.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

RAPTOR_NAMESPACE

static CImageModifier::CImageModifierClassID imageId;

// Stock modifier functions
void blowFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam);
void motionFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam);
void staticFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam);
void spinFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageModifier::CImageModifier(	TIME_FUNCTION_TYPE timeFunction,
								float			timeArg0,
								float			timeArg1,
								float			timeArg2,
								float			timeArg3,
								USER_FUNCTION   pUserFunction,
								const std::string& name)
    :CModifier(timeFunction,timeArg0,timeArg1,timeArg2,timeArg3,pUserFunction,imageId,name),
	m_pImage(NULL),m_pSrcBuffer(NULL),m_pDstBuffer(NULL),m_pfnModifier(NULL),
	m_pBufferImage(NULL), m_pBufferPointer(NULL)
{
}



CImageModifier::~CImageModifier()
{
	if (m_pSrcBuffer != NULL)
	{
		CHostMemoryManager::GetInstance()->release(m_pSrcBuffer);
		m_pSrcBuffer = NULL;
	}
	if (m_pDstBuffer != NULL)
	{
		CHostMemoryManager::GetInstance()->release(m_pDstBuffer);
		m_pDstBuffer = NULL;
	}

	if (CTexelAllocator::GetInstance()->isMemoryRelocated())
	{
		CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
		pAllocator->releaseTexels(m_pBufferPointer);
	}
}


unsigned int CImageModifier::getGenerateWidth(void) const
{
    if (m_pImage != NULL)
        return m_pImage->getWidth();
    else
        return 0;
}

unsigned int CImageModifier::getGenerateHeight(void) const
{
    if (m_pImage != NULL)
        return m_pImage->getHeight();
    else
        return 0;
}


bool CImageModifier::setImage(const ITextureObject* image)
{
	m_pBufferImage = NULL;
    m_pImage = NULL;

	if (m_pSrcBuffer != NULL)
	{
		CHostMemoryManager::GetInstance()->release(m_pSrcBuffer);
		m_pSrcBuffer = NULL;
	}
	if (m_pDstBuffer != NULL)
	{
		CHostMemoryManager::GetInstance()->release(m_pDstBuffer);
		m_pDstBuffer = NULL;
	}

    //! Be sure we have something to modify !!!
    if ((image != NULL) && (image->getWidth() > 0) && (image->getHeight() > 0))
    {
        m_pImage = const_cast<ITextureObject*>(image);

	    //	Allocate buffers with a one pixel border
		size_t s = (m_pImage->getWidth()) * (m_pImage->getHeight() + 2) * 4;
		CHostMemoryManager::Allocator<unsigned char, 32> allocator;

		//	Allocate memory blocs
		m_pSrcBuffer = allocator.allocate(s);
		m_pDstBuffer = allocator.allocate(s);
		memset(m_pSrcBuffer, 0, s);
		memset(m_pDstBuffer, 0, s);

		
		if (CTexelAllocator::GetInstance()->isMemoryRelocated())
		{
			CTexelAllocator *pAllocator = CTexelAllocator::GetInstance();
			m_pBufferPointer = pAllocator->allocateTexels(m_pImage->getWidth()*m_pImage->getHeight() * 4);
		}

	    m_pBufferImage = &m_pSrcBuffer[m_pImage->getWidth() * 4];
    }
    else
    {
		Raptor::GetErrorManager()->generateRaptorError(	CModifier::CModifierClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_TEXTURE_MISS);
    }
	
	CATCH_GL_ERROR

	return true;
}


void RAPTOR_FASTCALL CImageModifier::deltaTime(float dt)
{
	CModifier::deltaTime(dt);

	// then compute actions with m_ft
	if ((m_pImage != NULL) && (m_pfnModifier != NULL))
	{
		//	user defined buffer prepare operations
		onPrepareBuffer();

		//	line offset
		int offset = m_pImage->getWidth() * 4;

		//	Apply modifier
		m_pfnModifier(	m_pImage->getWidth(),
						m_pImage->getHeight(),
						&m_pSrcBuffer[offset],
						&m_pDstBuffer[offset],
						m_dwFncParam);
		//	Swap buffers
		unsigned char *tmp = m_pSrcBuffer;
		m_pSrcBuffer = m_pDstBuffer;
		m_pDstBuffer = tmp;
	}
}


void CImageModifier::glGenerate(CTextureObject* t)
{
    if ((m_pImage == NULL) || (!m_bEnabled) || (m_pDstBuffer == NULL) || (m_pSrcBuffer == NULL))
        return;

	m_pBufferImage = &m_pSrcBuffer[m_pImage->getWidth() * 4];

	if (CTexelAllocator::GetInstance()->isMemoryRelocated() &&
		CTexelAllocator::GetInstance()->isMemoryLocked() &&
		(NULL != m_pBufferPointer))
	{
		CTexelAllocator::GetInstance()->glvkCopyPointer(m_pBufferPointer,
														m_pBufferImage,
														t->getWidth() * t->getHeight() *4);
		glTexSubImage2D(GL_TEXTURE_2D,
						t->getCurrentMipMapLevel(),
						0, 0,
						t->getWidth(), t->getHeight(),
						GL_RGBA, GL_UNSIGNED_BYTE,
						m_pBufferPointer);
	}
	else
		glTexSubImage2D(GL_TEXTURE_2D,
						t->getCurrentMipMapLevel(),
						0,	0,
						t->getWidth(), t->getHeight(),
						GL_RGBA, GL_UNSIGNED_BYTE,
						m_pBufferImage);
}


bool CImageModifier::selectModifierFunction(MODIFIER_STOCK_FNC fnc,unsigned long dwParam)
{
	m_dwFncParam = dwParam;

	if (fnc == NULL)
		return false;

	m_pfnModifier = fnc;

	return true;
}


bool CImageModifier::selectModifierFunction(CImageModifier::RENDERING_MODIFIER fnc,unsigned long dwParam)
{
	m_dwFncParam = dwParam;

	switch(fnc)
	{
		case CGL_MOTIONFADER_MODIFIER:
			m_pfnModifier = motionFader;
			break;
		case CGL_BLOWFADER_MODIFIER:
			m_pfnModifier = blowFader;
			break;
		case CGL_SPINFADER_MODIFIER:
			m_pfnModifier = spinFader;
			break;
		case CGL_STATICFADER_MODIFIER:
			m_pfnModifier = staticFader;
			break;
		default:
			m_pfnModifier = NULL;
			break;
	}

	return (m_pfnModifier != NULL);
}



void blowFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam)
{
	unsigned char *ofsdst = dst;
	unsigned char *ofssrc = src;
	unsigned char *finaloffset = 0;

	int xinc = 0;
	int yinc = 0;

	int dx = 0;
	int dy = 0;
	unsigned int width4 = width * 4;

	unsigned int val = 0;

#ifdef RAPTOR_SSE_CODE_GENERATION
	__m64 param = _mm_unpacklo_pi8(_mm_cvtsi32_si64(dwParam), _mm_setzero_si64());

	for (int i = 0; i<height; i++)
	{
		for (int j = 0; j<width; j++)
		{
			dx = j - (width >> 1);
			dy = i - (height >> 1);

			if (dx < 0)
				xinc = 4;
			else
				xinc = -4;

			if (dy < 0)
				yinc = (width << 2);
			else
				yinc = -(width << 2);

			dx = abs(dx);
			dy = abs(dy);

			if (dx>dy)
				finaloffset = ofssrc + xinc;
			else if (dx<dy)
				finaloffset = ofssrc + yinc;
			else
				finaloffset = ofssrc + xinc + yinc;

			unsigned int* pixels = (unsigned int*)finaloffset;

			// read pixels
			//	... on current line ...
			__m64 c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			__m64 c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			__m64 c7 = _mm_add_pi16(c1, c0);
			pixels += width;

			// ... on line below ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*pixels), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels = pixels - width - width;
			c7 = _mm_add_pi16(c7, c1);

			// ... on line over ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels += width;
			c7 = _mm_add_pi16(c7, c1);

			// compute fading
			c7 = _mm_srli_pi16(c7, 3);
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c0);
			c7 = _mm_srli_pi16(c7, 1);
			c7 = _mm_sub_pi16(c7, param);
			c7 = _mm_packs_pu16(c7, _mm_setzero_si64());

			// store pixel
			*((unsigned int*)ofsdst) = _mm_cvtsi64_si32(c7);

			ofsdst += 4;
			ofssrc += 4;
		}
	}

	_mm_empty();
#endif
}

void motionFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam)
{
#ifdef RAPTOR_SSE_CODE_GENERATION

	unsigned char *ofsdst = dst;
	unsigned char *ofssrc = src;

	// to perform motion : source pixels are one line below destination
	ofssrc += 4 * width;

	// fading parameter
	__m64 param = _mm_unpacklo_pi8(_mm_cvtsi32_si64(dwParam), _mm_setzero_si64());

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			unsigned int* pixels = (unsigned int*)ofssrc;

			// read pixels
			//	... on current line ...
			__m64 c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			__m64 c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			__m64 c7 = _mm_add_pi16(c1, c0);
			pixels += width;

			// ... on line below ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*pixels), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels = pixels - width - width;
			c7 = _mm_add_pi16(c7, c1);

			// ... on line over ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels += width;
			c7 = _mm_add_pi16(c7, c1);

			// compute fading
			c7 = _mm_srli_pi16(c7, 3);
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c0);
			c7 = _mm_srli_pi16(c7, 1);
			c7 = _mm_sub_pi16(c7, param);
			c7 = _mm_packs_pu16(c7, _mm_setzero_si64());

			// store pixel
			*((unsigned int*)ofsdst) = _mm_cvtsi64_si32(c7);

			ofsdst += 4;
			ofssrc += 4;
		}
	}

	_mm_empty();

#endif
}

void staticFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam)
{
#ifdef RAPTOR_SSE_CODE_GENERATION

	unsigned char *ofsdst = dst;
	unsigned char *ofssrc = src;

	// fading parameter
	__m64 param = _mm_unpacklo_pi8(_mm_cvtsi32_si64(dwParam), _mm_setzero_si64());

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			unsigned int* pixels = (unsigned int*)ofssrc;

			// read pixels
			//	... on current line ...
			__m64 c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			__m64 c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			__m64 c7 = _mm_add_pi16(c1, c0);
			pixels += width;

			// ... on line below ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*pixels), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels = pixels - width - width;
			c7 = _mm_add_pi16(c7, c1);

			// ... on line over ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels += width;
			c7 = _mm_add_pi16(c7, c1);

			// compute fading
			c7 = _mm_srli_pi16(c7, 3);
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c0);
			c7 = _mm_srli_pi16(c7, 1);
			c7 = _mm_sub_pi16(c7, param);
			c7 = _mm_packs_pu16(c7, _mm_setzero_si64());

			// store pixel
			*((unsigned int*)ofsdst) = _mm_cvtsi64_si32(c7);

			ofsdst += 4;
			ofssrc += 4;
		}
	}

	_mm_empty();

#endif
}

void spinFader(int width, int height, unsigned char *src, unsigned char *dst, unsigned long dwParam)
{
	unsigned char *ofsdst = dst;
	unsigned char *ofssrc = src;
	unsigned char *finaloffset = 0;

	int xinc = 0;
	int yinc = 0;

	int dx = 0;
	int dy = 0;
	unsigned int width4 = width * 4;

	unsigned int val = 0;

#ifdef RAPTOR_SSE_CODE_GENERATION

	__m64 param = _mm_unpacklo_pi8(_mm_cvtsi32_si64(dwParam), _mm_setzero_si64());

	for (int i = 0; i<height; i++)
	{
		for (int j = 0; j<width; j++)
		{
			dx = j - (width >> 1);
			dy = i - (height >> 1);

			xinc = 4;
			yinc = (width << 2);

			if (dx > 0)
			{
				if (dy > 0)
					finaloffset = ofssrc - yinc;
				else
					finaloffset = ofssrc - xinc;
			}
			else
			{
				if (dy > 0)
					finaloffset = ofssrc + xinc;
				else
					finaloffset = ofssrc + yinc;
			}

			unsigned int* pixels = (unsigned int*)finaloffset;

			// read pixels
			//	... on current line ...
			__m64 c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			__m64 c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			__m64 c7 = _mm_add_pi16(c1, c0);
			pixels += width;

			// ... on line below ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*pixels), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels = pixels - width - width;
			c7 = _mm_add_pi16(c7, c1);

			// ... on line over ...
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels - 1)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c1);
			c7 = _mm_add_pi16(c7, c0);
			c1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels + 1)), _mm_setzero_si64());
			pixels += width;
			c7 = _mm_add_pi16(c7, c1);

			// compute fading
			c7 = _mm_srli_pi16(c7, 3);
			c0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(pixels)), _mm_setzero_si64());
			c7 = _mm_add_pi16(c7, c0);
			c7 = _mm_srli_pi16(c7, 1);
			c7 = _mm_sub_pi16(c7, param);
			c7 = _mm_packs_pu16(c7, _mm_setzero_si64());

			// store pixel
			*((unsigned int*)ofsdst) = _mm_cvtsi64_si32(c7);

			ofsdst += 4;
			ofssrc += 4;
		}
	}

	_mm_empty();

#endif
}

