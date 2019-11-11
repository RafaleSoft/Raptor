// RaptorVulkanDisplay.h: interface for the CRaptorBufferDisplay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORVULKANDISPLAY_H__1D39CB77_CA4D_4A8F_90FA_F9C76774CE6F__INCLUDED_)
#define AFX_RAPTORVULKANDISPLAY_H__1D39CB77_CA4D_4A8F_90FA_F9C76774CE6F__INCLUDED_

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
class CUniformAllocator;
class CVulkanPipeline;

class CRaptorVulkanDisplay : public CRaptorDisplay
{
public:
	CRaptorVulkanDisplay(const CRaptorDisplayConfig& pcs);
	virtual ~CRaptorVulkanDisplay(void);

	//! see base class
	virtual bool glvkBindDisplay(const RAPTOR_HANDLE& device);

	//! see base class
	virtual bool glvkUnBindDisplay(void);

	//! see base class
	virtual IViewPoint *const createViewPoint(void) const;

	//!	Implements base class.
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

	//!	Implements base class.
	virtual void glResize(	unsigned int sx,unsigned int sy,
							unsigned int ox, unsigned int oy);

	//!	Implements base class.
	virtual bool glGrab(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
						uint8_t* &data, size_t& size) const;

	//!	Implements base class.
	virtual bool glBlit(uint32_t xSrc, uint32_t ySrc, uint32_t widthSrc, uint32_t heightSrc,
						uint32_t xDst, uint32_t yDst, uint32_t widthDst, uint32_t heightDst,
						CRaptorDisplay *pDst) const;

	//!	Implements base class.
	virtual void glGenerate(CTextureObject* );

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CRaptorVulkanDisplayClassID,"RaptorVulkanDisplay",CRaptorDisplay)



private:
	void allocateResources(void);

	CContextManager::RENDERING_CONTEXT_ID	m_context;

	//!	Display configuration.
	CRaptorDisplayConfig	cs;

	//! number of frames in previous second
	float	fps;
	//! time between two frames (inverse of fps)
	float	ftime;
	//! real-time number of frames per second ( time elapsed since last frame )
	float	rtfps;
	//! time to render scenes
	float	rtime;
	//!	Previous rendered absolute time.
	float	l1;
	//!	Previous rendered second.
	float	lastfreq;
	//!	Frame number in current second.
	int		nbFramesPerSecond;


	//!	Allocators are resources physically part of a graphic context,
	//!	so they are managed by the screen display which is an abstraction
	//!	of the graphical context
    CGeometryAllocator  *m_pGAllocator;
    CGeometryAllocator  *m_pGOldAllocator;
	CTexelAllocator		*m_pTAllocator;
	CTexelAllocator		*m_pTOldAllocator;
	CUniformAllocator	*m_pUAllocator;
	CUniformAllocator	*m_pUOldAllocator;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANDISPLAY_H__1D39CB77_CA4D_4A8F_90FA_F9C76774CE6F__INCLUDED_)

