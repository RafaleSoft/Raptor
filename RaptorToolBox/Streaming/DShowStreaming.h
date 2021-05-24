/***************************************************************************/
/*                                                                         */
/*  DShowStreaming.h                                                       */
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


#if !defined(AFX_DSHOWSTREAMING_H__039A618A_339A_4BE8_B61F_E7DA513185AA__INCLUDED_)
#define AFX_DSHOWSTREAMING_H__039A618A_339A_4BE8_B61F_E7DA513185AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
    #include "Engine/Animator.h"
#endif



RAPTOR_NAMESPACE

#if defined(_WIN32)

#include <dshow.h>


class CDShowStreaming : public CAnimator::IVideoIO
{
public:
	CDShowStreaming()
		:frameLength(0), streamPos(0), locked(false),
		m_pGraph(NULL), m_pControl(NULL), m_pPosition(NULL), m_pEvent(NULL), m_pFilter(NULL),
		lastFrameTime(0) {};
    virtual ~CDShowStreaming() {};

	virtual bool isOfKind(const std::string &kind) const;
	virtual std::vector<std::string> getKind(void) const;
	virtual unsigned int getWidth() const;
	virtual unsigned int getHeight() const;
    virtual float getFrameLength() const { return frameLength; };

    virtual CAnimator::IVideoIO*   newInstance(void) const { return new CDShowStreaming; };
	virtual bool releaseInstance(void) { delete this; return true;};

	virtual bool openReader(const std::string &fname);
	virtual bool readFrame(unsigned char *& readBuffer, float timestamp = 0.0f);
    virtual void seekFrame(unsigned int framePos);
    virtual unsigned int getFramePosition(void) const { return streamPos; };
    virtual bool closeReader(void);

    virtual bool openRecorder(const std::string& fname,unsigned int width,unsigned int height);
    virtual bool writeFrame(const unsigned char *writeBuffer);
    virtual bool closeRecorder(void);

	//!	Static method to implement base class virtual method isOfKind
public:
	static bool _isOfKind(const std::string &kind);


private:
    float			frameLength;
	float			lastFrameTime;
    LONG			streamPos;
    bool			locked;
	IGraphBuilder	*m_pGraph;
	IMediaControl	*m_pControl;
	IMediaPosition	*m_pPosition;
	IMediaEvent		*m_pEvent;
	IBaseFilter		*m_pFilter;
};
#endif


#endif	// !defined(AFX_DSHOWSTREAMING_H__039A618A_339A_4BE8_B61F_E7DA513185AA__INCLUDED_)


