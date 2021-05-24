/***************************************************************************/
/*                                                                         */
/*  MFStreaming.h                                                          */
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


#if !defined(AFX_MFSTREAMING_H__25A33139_3FCF_477D_9509_1BA9A2F0F6B9__INCLUDED_)
#define AFX_MFSTREAMING_H__25A33139_3FCF_477D_9509_1BA9A2F0F6B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Engine/Animator.h"
#endif

#if defined(_WIN32)
	#include <windows.h>
	#include <windowsx.h>

	// Media Foundation 
	#include <mfapi.h>
	#include <mfidl.h>
	#include <mfreadwrite.h>
	#include <mferror.h>
#endif


RAPTOR_NAMESPACE

#if defined(_WIN32)
class CMFStreaming : public CAnimator::IVideoIO
{
public:
	CMFStreaming();
	virtual ~CMFStreaming();

	virtual bool isOfKind(const std::string &kind) const;
	virtual std::vector<std::string> getKind(void) const;
	virtual unsigned int getWidth() const { return m_format.imageWidthPels; };
	virtual unsigned int getHeight() const { return m_format.imageHeightPels; };
	virtual float getFrameLength() const { return frameLength; };

	virtual CAnimator::IVideoIO*   newInstance(void) const { return new CMFStreaming; };
	virtual bool releaseInstance(void) { delete this; return true; };

	virtual bool openReader(const std::string &fname);
	virtual bool readFrame(uint8_t *& readBuffer, float timestamp = 0.0f);
	virtual void seekFrame(unsigned int framePos) { streamPos = framePos; };
	virtual unsigned int getFramePosition(void) const { return streamPos; };
	virtual bool closeReader(void);

	virtual bool openRecorder(const std::string& fname, unsigned int width, unsigned int height);
	virtual bool writeFrame(const uint8_t *writeBuffer);
	virtual bool closeRecorder(void);

	//!	Static method to implement base class virtual method isOfKind
public:
	static bool _isOfKind(const std::string &kind);

private:
	struct FormatInfo
	{
		UINT32          imageWidthPels;
		UINT32          imageHeightPels;
		BOOL            bTopDown;
		RECT            rcPicture;    // Corrected for pixel aspect ratio

		FormatInfo() : imageWidthPels(0), imageHeightPels(0), bTopDown(FALSE)
		{
			SetRectEmpty(&rcPicture);
		}
	};

	//!
	//! SelectVideoStream: finds the first video stream and sets the format to RGB32.
	//!	@return S_OK if video format selected, error in any other case.
	//!
	HRESULT SelectVideoStream(void);
	
	//!
	//! GetVideoFormat: gets format information for the video stream.
	//! @param pFormat: Receives the format information.
	//!	@return S_OK if video format retrieved, error in any other case.
	//!
	HRESULT GetVideoFormat(CMFStreaming::FormatInfo *pFormat);

	//!
	//!	CanSeek: queries the streamer if seek capability is present.
	//! @param pbCanSeek: a boolean to store seking capability.
	//!	@return S_OK if video seek capability retrieved, error in any other case.
	//!
	HRESULT CMFStreaming::CanSeek(bool *pbCanSeek);


	IMFSourceReader *m_pReader;
	IMFSample		*m_pSample;
	FormatInfo      m_format;
	uint8_t			*m_pFrameBuffer;

	float       frameLength;
	float		lastFrameTime;
	LONG		streamPos;
	bool        locked;
	bool		m_bCanSeek;
};
#endif

#endif	// !defined(AFX_MFSTREAMING_H__25A33139_3FCF_477D_9509_1BA9A2F0F6B9__INCLUDED_)
