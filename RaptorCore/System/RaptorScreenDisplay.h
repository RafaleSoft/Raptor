// RaptorScreenDisplay.h: interface for the CRaptorGDIDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORSCREENDISPLAY_H__D3165157_E39B_4770_990F_26D44A7BD1A3__INCLUDED_)
#define AFX_RAPTORSCREENDISPLAY_H__D3165157_E39B_4770_990F_26D44A7BD1A3__INCLUDED_

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

class CGeometryAllocator;
class CTexelAllocator;
class CAnimatorStream;

//! This class implements an OGL display for direct screen rendering
class CRaptorScreenDisplay : public CRaptorDisplay  
{
public:
	CRaptorScreenDisplay(const CRaptorDisplayConfig& pcs);
	virtual ~CRaptorScreenDisplay();

	//! Implements CTextureGenerator
    virtual CTextureGenerator::GENERATOR_KIND getKind(void) const { return CTextureGenerator::ANIMATED; };

	//! see base class
	virtual bool glBindDisplay(const RAPTOR_HANDLE& device);

    //! see base class
    virtual RAPTOR_HANDLE getCurrentDevice(void) const;

	//! see base class
	virtual bool glUnBindDisplay(void);

	//! Real implementation of the viewport resize
	virtual void glResize(unsigned int sx,unsigned int sy,unsigned int ox, unsigned int oy);

	//! see base class
	virtual bool hasSwapControl(void) const { return m_swapControl; }

	//!	Renders the display, if it has been bound.
	//! Returns true if rendered without errors, false otherwise.
	virtual bool glRender(void);

	//!	Return the number of frames rendered during the last second.
	virtual float getFPS(void) const { return fps; };

	//!	Return the current frame rate : it is the time elapsed since previous
	//!	call to glRender, which is always >= time required to render a frame.
	//! So it vary a lot but is more likely to be the maximum instant frame rate 
	//!	because it comprises the whole engine + system time.
	virtual float getRTFPS(void) const { return rtfps; };

	//!	Return the delay of a whole frame, including
	//! buffer swapping and driver workload
	virtual float getFrameTime(void) const { return ftime; };

	//!	Return the render time of a single frame (likely to be realtime)
	//! It measures only scene drawing without any deferred tasks
	virtual float getRenderTime(void) const { return rtime; };

    //! @see CRaptorDisplay
    virtual bool glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* &data,unsigned int& size) const;

	//! @see CRaptorDisplay
	virtual bool glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
						unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
						CRaptorDisplay *pDst) const;

    //! Implement base class CTextureGenerator of CRaptorDisplay
    virtual void glGenerate(CTextureObject* );

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CRaptorScreenDisplayClassID,"RaptorScreenDisplay",CRaptorDisplay)



protected:
	virtual void glRenderScene();

	//! number of frames in previous second
	float	fps;
	//! time between two frames (inverse of fps)
	float	ftime;
	//! real-time number of frames per second ( time elapsed since last frame )
	float	rtfps;
	//! time to render scenes
	float	rtime;

	//!	rendering rate management
	bool			m_swapControl;
	unsigned long	m_framerate;
	float	lastfreq;
	float	l1;
	CAnimatorStream	*m_streamer;

	CContextManager::RENDERING_CONTEXT_ID	m_context;
	CContextManager::RENDERING_CONTEXT_ID	m_layerContext;
						
	CRaptorDisplayConfig	cs;

	
    //!	Allocators are resources physically part of a graphic context,
	//!	so they are managed by the screen display which is an abstraction
	//!	of the graphical context
    CGeometryAllocator  *m_pGAllocator;
    CGeometryAllocator  *m_pGOldAllocator;
	CTexelAllocator		*m_pTAllocator;
    CTexelAllocator		*m_pTOldAllocator;

private:
    virtual bool glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const;

	void allocateResources(void);

	//!	Frame number in current second.
	int		nbFramesPerSecond;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORSCREENDISPLAY_H__D3165157_E39B_4770_990F_26D44A7BD1A3__INCLUDED_)

