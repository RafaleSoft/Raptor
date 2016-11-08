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


class CVulkanPipeline;

class CRaptorVulkanDisplay : public CRaptorDisplay
{
public:
	CRaptorVulkanDisplay(const CRaptorDisplayConfig& pcs);
	virtual ~CRaptorVulkanDisplay(void);

	//! see base class
	virtual bool glBindDisplay(const RAPTOR_HANDLE& device);

	//! see base class
	virtual bool glUnBindDisplay(void);


	//!	Implements base class.
	virtual bool glRender(void);

	//!	Implements base class.
	virtual void glResize(	unsigned int sx,unsigned int sy,
							unsigned int ox, unsigned int oy);

	//!	Implements base class.
	virtual bool glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height,
						unsigned char* &data,unsigned int& size) const;

	//!	Implements base class.
	virtual bool glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
						unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
						CRaptorDisplay *pDst) const;

	//!	Implements base class.
	virtual void glGenerate(CTextureObject* );

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CRaptorVulkanDisplayClassID,"RaptorVulkanDisplay",CRaptorDisplay)

private:
	std::vector<CVulkanPipeline*> m_pipelines;
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
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORVULKANDISPLAY_H__1D39CB77_CA4D_4A8F_90FA_F9C76774CE6F__INCLUDED_)

