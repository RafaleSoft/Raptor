// RaptorBufferDisplay.h: interface for the CRaptorBufferDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORBUFFERDISPLAY_H__DBB1E3EC_EAF3_4EBA_AAB4_C51FA49C5884__INCLUDED_)
#define AFX_RAPTORBUFFERDISPLAY_H__DBB1E3EC_EAF3_4EBA_AAB4_C51FA49C5884__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "RaptorDisplay.h"
#endif

#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CRaptorBufferDisplay : public CRaptorDisplay
{
public:
	CRaptorBufferDisplay(const CRaptorDisplayConfig& pcs);
	virtual ~CRaptorBufferDisplay();

	//! see base class
	virtual bool glvkBindDisplay(const RAPTOR_HANDLE& device);

	//! see base class
	virtual bool glUnBindDisplay(void);

	//! see base class
	virtual void glResize(unsigned int sx,unsigned int sy,unsigned int ox,unsigned int oy);

	//!	Renders the display, if it has been bound.
	//! Returns true if rendered without errors, false otherwise.
	virtual bool glRender(void);
	
    //! Implements CRaptorDisplay
    virtual bool glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* &data,unsigned int& size) const;

	//! Implements CRaptorDisplay
	virtual bool glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
						unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
						CRaptorDisplay *pDst) const;


    //! Implements CTextureGenerator
    virtual ITextureGenerator::GENERATOR_KIND getKind(void) const { return ITextureGenerator::BUFFERED; };

    //! Implements CTextureGenerator
    virtual void glGenerate(CTextureObject* );

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CRaptorBufferDisplayClassID,"RaptorBufferDisplay",CRaptorDisplay)


private:
    //! Implement base class : to retrieve buffer configuration
    virtual bool glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const;

	CContextManager::PIXEL_BUFFER_ID			m_pBuffer;
	CContextManager::RENDERING_CONTEXT_ID		m_previousContext;
	CRaptorDisplayConfig	cs;

	bool	m_bBoundToTexture;

    //! A stack for buffer bound
    static  vector<unsigned int> m_bindingStack;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORBUFFERDISPLAY_H__DBB1E3EC_EAF3_4EBA_AAB4_C51FA49C5884__INCLUDED_)

