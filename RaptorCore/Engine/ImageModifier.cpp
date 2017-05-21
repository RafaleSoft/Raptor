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

#ifndef __INTERNAL_PROCS_H__
	#include "Subsys/InternalProcs.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

RAPTOR_NAMESPACE

static CImageModifier::CImageModifierClassID imageId;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageModifier::CImageModifier(TIME_FUNCTION_TYPE timeFunction,
							                        float			timeArg0,
							                        float			timeArg1,
							                        float			timeArg2,
							                        float			timeArg3,
                                                    USER_FUNCTION   pUserFunction ,
							                        const std::string& name)
    :CModifier(timeFunction,timeArg0,timeArg1,timeArg2,timeArg3,pUserFunction,imageId,name),
	m_pImage(NULL),m_pSrcBuffer(NULL),m_pDstBuffer(NULL),m_pfnModifier(NULL),
	m_pBufferImage(NULL)
{
}



CImageModifier::~CImageModifier()
{
    if (m_pSrcBuffer != NULL)
		delete [] m_pSrcBuffer;
	if (m_pDstBuffer != NULL)
		delete [] m_pDstBuffer;
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


bool CImageModifier::setImage(const CTextureObject* image)
{
	m_pBufferImage = NULL;
    m_pImage = NULL;

	if (m_pSrcBuffer != NULL)
		delete [] m_pSrcBuffer;
	if (m_pDstBuffer != NULL)
		delete [] m_pDstBuffer;

    //! Be sure we have something to modify !!!
    if ((image != NULL) && (image->getWidth() > 0) && (image->getHeight() > 0))
    {
        m_pImage = const_cast<CTextureObject*>(image);

	    //	Allocate buffers with a one pixel border
		size_t s = (m_pImage->getWidth()) * (m_pImage->getHeight() + 2) * 4;
	    m_pSrcBuffer = new unsigned char[s];
	    m_pDstBuffer = new unsigned char[s];
		memset(m_pSrcBuffer, 0, s);
		memset(m_pDstBuffer, 0, s);

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


CImageModifier::MODIFIER_STOCK_FNC CImageModifier::GetModifierFunction(CImageModifier::RENDERING_MODIFIER fnc)
{
	switch(fnc)
	{
		case CGL_MOTIONFADER_MODIFIER:
			return motionFader;
			break;
		case CGL_BLOWFADER_MODIFIER:
			return blowFader;
			break;
		case CGL_SPINFADER_MODIFIER:
			return spinFader;
			break;
		case CGL_STATICFADER_MODIFIER:
			return staticFader;
			break;
		default:
			return NULL;
			break;
	}
}


