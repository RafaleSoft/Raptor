// RaptorRenderBufferDisplay.h: interface for the CRaptorRenderBufferDisplay class.
//
///////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_RAPTORRENDERBUFFERDISPLAY_H__AC23F2BD_DAC5_4B9A_9F75_AD9A2CEEAE08__INCLUDED_)
#define AFX_RAPTORRENDERBUFFERDISPLAY_H__AC23F2BD_DAC5_4B9A_9F75_AD9A2CEEAE08__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "System/RaptorDisplay.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CTextureSet;


class CRaptorRenderBufferDisplay : public CRaptorDisplay
{
public:
	CRaptorRenderBufferDisplay(const CRaptorDisplayConfig& pcs);
	virtual ~CRaptorRenderBufferDisplay(void);

	//!	The render buffer can also be bound to texture targets defined by device:
	//! @param device : an empty handle for general binding ( @see base class ).
	//! To bind to textures, device.handle parameter is a pointe to a texture set,
	//!	and the device.class is the class id of a texture set.
	//! Each color texture of the set will be bound to a color attachment,
	//! and each depth texture will be bound to the depth attachment.
	virtual bool glBindDisplay(const RAPTOR_HANDLE& device);

	//! see base class
	virtual bool glUnBindDisplay(void);

	//! see base class
	virtual void glResize(unsigned int sx,unsigned int sy,unsigned int ox,unsigned int oy);

	//! see base class
	virtual bool hasSwapControl(void) const { return false; };

	//!	Renders the display, if it has been bound.
	//! Returns true if rendered without errors, false otherwise.
	virtual bool glRender(void);

    //! Implements CRaptorDisplay
    virtual bool glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* &data,unsigned int& size) const;

	//! Implements CRaptorDisplay
	virtual bool glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
						unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
						CRaptorDisplay *pDst) const;

    //! Implements ITextureGenerator
    virtual ITextureGenerator::GENERATOR_KIND getKind(void) const { return ITextureGenerator::BUFFERED; };

    //! Implements CTextureGenerator
    virtual void glGenerate(CTextureObject* );

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CRaptorRenderBufferDisplayClassID,"RaptorRenderBufferDisplay",CRaptorDisplay)


private:
    //! Implements base class
    virtual void unLink(const CPersistence* obj);

	//!	A helper to release & delete all used resources.
	void glDestroyBuffer(void);

    //! Implement base class : to retrieve buffer configuration
    virtual bool glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const;

	//!	Returns buffer state for internal usage
	bool checkBufferStatus(void) const;

	//!	Creates framebuffer resources
	bool createFrameBuffer(void);

	//!	Creates texture targets attachments, if bound to a texture set.
	bool glAttachBuffers();

	//!	Removes texture attachments if a texture set has been bound.
	bool glDetachBuffers();

	//!	Data necessary to implement base class services
	CRaptorDisplayConfig	cs;

	//! Main object
	unsigned int m_framebuffer;
	unsigned int m_colorbuffer;
	unsigned int m_depthbuffer;
	unsigned int m_stencilbuffer;

	CTextureSet *m_pAttachments;

	//! A stack for buffer bound
    static  vector<unsigned int> m_bindingStack;
};

RAPTOR_NAMESPACE_END

#endif	// !defined(AFX_RAPTORRENDERBUFFERDISPLAY_H__AC23F2BD_DAC5_4B9A_9F75_AD9A2CEEAE08__INCLUDED_)

