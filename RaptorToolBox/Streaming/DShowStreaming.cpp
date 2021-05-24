/***************************************************************************/
/*                                                                         */
/*  DShowStreaming.cpp                                                     */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_DSHOWSTREAMING_H__039A618A_339A_4BE8_B61F_E7DA513185AA__INCLUDED_)
	#include "Streaming/DShowStreaming.h"
#endif


//! Direct Show base classes include.
#include "streams.h"

#if defined(_WIN32)


bool CDShowStreaming::isOfKind(const std::string &kind) const
{
	return _isOfKind(kind);
}

bool CDShowStreaming::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return (("AVI" == ext) ||
		("3G2" == ext) ||
		("3GP" == ext) ||
		("3GP2" == ext) ||
		("3GPP" == ext) ||
		("ASF" == ext) ||
		//("WMA" == ext) ||
		//("WMV" == ext) ||
		("AAC" == ext) ||
		("ADTS" == ext) ||
		("MP3" == ext) ||
		("M4A" == ext) ||
		("M4V" == ext) ||
		("MOV" == ext) ||
		//("MP4" == ext) ||
		("SAMI" == ext) ||
		("SMI" == ext) ||
		("WAV" == ext));
}

std::vector<std::string> CDShowStreaming::getKind(void) const
{
	std::vector<std::string> result;

	result.push_back("AVI");
	result.push_back("3G2");
	result.push_back("3GP");
	result.push_back("3GP2");
	result.push_back("3GPP");
	result.push_back("ASF");
	//result.push_back("WMA");
	//result.push_back("WMV");
	result.push_back("AAC");
	result.push_back("ADTS");
	result.push_back("MP3");
	result.push_back("M4A");
	result.push_back("M4V");
	result.push_back("MOV");
	//result.push_back("MP4");
	result.push_back("SAMI");
	result.push_back("SMI");
	result.push_back("WAV");

	return result;
}

void CDShowStreaming::seekFrame(unsigned int framePos)
{ 
	streamPos = framePos; 
    lastFrameTime = 0;
	/*
    HRESULT hr = m_pReader->SetRangeByFrame( m_wNumStream, streamPos, 0);
    if ( FAILED( hr ) )
    {
        // what should I do here ?
		return ;
    }
	*/
}

//-----------------------------------------------------------------------------
// CTextureRenderer Class Declarations
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;
class CTextureRenderer : public CBaseVideoRenderer
{
public:
	CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr);
	~CTextureRenderer();

public:
	HRESULT CheckMediaType(const CMediaType *pmt);     // Format acceptable?
	HRESULT SetMediaType(const CMediaType *pmt);       // Video format notification
	HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	LONG m_lVidWidth;   // Video width
	LONG m_lVidHeight;  // Video Height
	LONG m_lVidPitch;   // Video Pitch

	uint8_t	*m_pFrameBuffer;
};

unsigned int CDShowStreaming::getWidth() const
{
	CTextureRenderer *txt = (CTextureRenderer *)m_pFilter;
	
	return txt->m_lVidWidth;
};

unsigned int CDShowStreaming::getHeight() const
{
	CTextureRenderer *txt = (CTextureRenderer *)m_pFilter;

	return txt->m_lVidHeight;
};


bool CDShowStreaming::readFrame(unsigned char *& readBuffer, float timestamp)
{
	if ((!locked) || (m_pControl == NULL))
		return false;

	if (NULL == m_pFilter)
		return false;

	CTextureRenderer *txt = (CTextureRenderer*)m_pFilter;
	readBuffer = txt->m_pFrameBuffer;

		/*
		if (SUCCEEDED(hr))
		{
			BYTE*  pdwBuffer = NULL;
			DWORD  dwLength = 0;

			hr = m_pBuffer->GetBufferAndLength(&pdwBuffer,&dwLength);
			if (FAILED(hr))
				return false;

			readBuffer = pdwBuffer;
			// What is duration ?
			//frameLength = ((float)cnsDuration) / 1000000;
			frameLength = ((float)(__int64)cnsSampleTime - lastFrameTime) / 10000000;
			lastFrameTime = (float)(__int64)cnsSampleTime;
			streamPos++;
		}
		else
		{
			//!	Here we could manage looping
			if (hr == NS_E_NO_MORE_SAMPLES)
				return false;
			else if (hr == E_UNEXPECTED)
			{
				RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Unexpected error while GetNextSample");
				return false;
			}
			else if (hr == E_INVALIDARG)
				return false;
			else if (hr == NS_E_INVALID_REQUEST)
				return false;
			else if (hr == NS_E_VIDEO_CODEC_NOT_INSTALLED)
			{
				RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Video codec not installed while GetNextSample");
				return false;
			}
			else if (hr == NS_E_AUDIO_CODEC_NOT_INSTALLED)
			{
				RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Audio codec not installed while GetNextSample");
				return false;
			}
		}
		*/
		return true;
}

//-----------------------------------------------------------------------------
// CTextureRenderer constructor
//-----------------------------------------------------------------------------
CTextureRenderer::CTextureRenderer(LPUNKNOWN pUnk, HRESULT *phr)
	: CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr),
	m_lVidWidth(0), m_lVidHeight(0), m_lVidPitch(0), m_pFrameBuffer(NULL)
{
	// Store and AddRef the texture for our use.
	ASSERT(phr);
	if (phr)
		*phr = S_OK;
}


//-----------------------------------------------------------------------------
// CTextureRenderer destructor
//-----------------------------------------------------------------------------
CTextureRenderer::~CTextureRenderer()
{
	if (NULL != m_pFrameBuffer)
		delete[] m_pFrameBuffer;
}

//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::CheckMediaType(const CMediaType *pmt)
{
	HRESULT   hr = E_FAIL;
	VIDEOINFO *pvi = 0;

	CheckPointer(pmt, E_POINTER);

	const GUID *format = pmt->FormatType();
	const GUID *type = pmt->Type();
	const GUID *subtype = pmt->Subtype();

	// Reject the connection if this is not a video type
	if (*format != FORMAT_VideoInfo)
	{
		if (*type == MEDIATYPE_Stream)
		{
			if ((*subtype == MEDIASUBTYPE_Asf) ||
				(*subtype == MEDIASUBTYPE_Avi) ||
				(*subtype == MEDIASUBTYPE_MPEG1Video) ||
				(*subtype == MEDIASUBTYPE_MPEG1VideoCD))
				hr = S_OK;
		}
		else if (*type == MEDIATYPE_Video)
		{
			// Only accept RGB24 video
			if (*subtype == MEDIASUBTYPE_RGB24)
				hr = S_OK;
		}
		else
			hr = E_INVALIDARG;
	}
	else
		hr = S_OK;

	return hr;
}

//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::SetMediaType(const CMediaType *pmt)
{
	// Retrive the Bitmap info header
	VIDEOINFO *pviBmp = (VIDEOINFO *)pmt->Format();
	if (NULL == pviBmp)
		return E_FAIL;

	m_lVidWidth = pviBmp->bmiHeader.biWidth;
	m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
	m_lVidPitch = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

	if (NULL != m_pFrameBuffer)
		delete[] m_pFrameBuffer;
	m_pFrameBuffer = new uint8_t[m_lVidPitch * m_lVidHeight];

	return S_OK;
}


//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::DoRenderSample(IMediaSample * pSample)
{
	CheckPointer(pSample, E_POINTER);

	long size = pSample->GetActualDataLength();

	// Get the video bitmap buffer
	BYTE  *pBmpBuffer;	// Bitmap buffer
	pSample->GetPointer(&pBmpBuffer);

	if (NULL != m_pFrameBuffer)
		memcpy(m_pFrameBuffer, pBmpBuffer, min(size,m_lVidPitch*m_lVidHeight));

	return S_OK;
}

HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	IEnumPins  *pEnum = NULL;
	IPin       *pPin = NULL;
	HRESULT    hr;

	if (ppPin == NULL)
	{
		return E_POINTER;
	}

	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		hr = pPin->QueryDirection(&PinDirThis);
		if (FAILED(hr))
		{
			pPin->Release();
			pEnum->Release();
			return hr;
		}
		if (PinDir == PinDirThis)
		{
			// Found a match. Return the IPin pointer to the caller.
			*ppPin = pPin;
			pEnum->Release();
			return S_OK;
		}
		// Release the pin for the next time through the loop.
		pPin->Release();
	}
	// No more pins. We did not find a match.
	pEnum->Release();
	return E_FAIL;
}

bool CDShowStreaming::openReader(const std::string &fname)
{
	if (locked)
        return false;
	
	// Create the filter graph manager and query for interfaces.
    HRESULT hr = CoCreateInstance(	CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
									IID_IGraphBuilder, (void **)&m_pGraph);
    if (FAILED(hr))
    {
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(),
						"Could not create the Filter Graph Manager.");
        return false;
    }

	// Create the Texture Renderer object
	CTextureRenderer *pCTR = new CTextureRenderer(NULL, &hr);
	if (FAILED(hr))
	{
		if (NULL != pCTR)
			delete pCTR;
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(), 
						"Could not create texture renderer object");
		return false;
	}

	// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	m_pFilter = pCTR;
	hr = m_pGraph->AddFilter(m_pFilter, L"TEXTURERENDERER");
	if (FAILED(hr))
	{
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(),
						"Could not add renderer filter to graph");
		return false;
	}

	//	Add source filter from filename
	WCHAR wFname[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0L, fname.c_str(), fname.length() + 1, wFname, MAX_PATH);
	IBaseFilter	*pFilterSrc = NULL;
	hr = m_pGraph->AddSourceFilter(wFname, L"SOURCE", &pFilterSrc);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(), 
						"Could not open media file " + fname);
		return false;
	}

	// Connect pins: find src output Pin
	IPin *pFSrcPinOut = NULL;
	hr = GetPin(pFilterSrc, PINDIR_OUTPUT, &pFSrcPinOut);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(),
						"Could not find output pin");
		return false;
	}
	PIN_INFO pinfo;
	pFSrcPinOut->QueryPinInfo(&pinfo);
	
	IPin *pFTRPinIn = NULL;
	hr = GetPin(m_pFilter, PINDIR_INPUT, &pFTRPinIn);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(),
						"Could not find input pin");
		return false;
	}
	pFTRPinIn->QueryPinInfo(&pinfo);

	// Connect these two filters
	hr = m_pGraph->Connect(pFSrcPinOut, pFTRPinIn);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(),
						"Could not connect pins");
		return false;
	}



	hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pControl);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(), "Could not create the Media Control.");
		return false;
	}

	hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pEvent);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(), "Could not create the Media Event.");
		return false;
	}

	hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void **)&m_pPosition);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(), "Could not create the Media Position.");
		return false;
	}

	hr = m_pControl->Run();
	if (FAILED(hr))
	{
		RAPTOR_ERROR(	CAnimator::CAnimatorClassID::GetClassId(), 
						"Could not run the DirectShow graph!");
		return hr;
	}
	/*
	WCHAR wFname[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0L, fname.c_str(), fname.length() + 1, wFname, MAX_PATH);
	hr = m_pControl->RenderFile(wFname);
	if (FAILED(hr))
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(), "Could not open media file " + fname);
		return false;
	}
	*/

	locked = true;
	return true;
}

bool CDShowStreaming::closeReader(void)
{
	HRESULT hr = S_OK;
	
	if (NULL != m_pControl)
	{
		m_pControl->Stop();
	}

	if (NULL != m_pPosition)
		hr = m_pPosition->Release();
	m_pPosition = NULL;

	if (FAILED(hr))
		return false;


	if (NULL != m_pEvent)
		hr = m_pEvent->Release();
	m_pEvent = NULL;

	if (FAILED(hr))
		return false;


	if( NULL != m_pControl )
        hr = m_pControl->Release();
	m_pControl = NULL;
    
    if(FAILED(hr))
        return false;
	

	if (NULL != m_pGraph)
		hr = m_pGraph->Release();
	m_pGraph = NULL;

	if (FAILED(hr))
		return false;

	// TODO: delete Texture Renderer

	locked = false;
    return true;
}

bool CDShowStreaming::openRecorder(const std::string& fname,unsigned int width,unsigned int height)
{
	return false;
}

bool CDShowStreaming::writeFrame(const unsigned char *writeBuffer)
{
	return false;
}

bool CDShowStreaming::closeRecorder(void)
{
	return false;
}

#endif

