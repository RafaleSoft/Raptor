/***************************************************************************/
/*                                                                         */
/*  MFStreaming.cpp                                                        */
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

#if !defined(AFX_MFSTREAMING_H__25A33139_3FCF_477D_9509_1BA9A2F0F6B9__INCLUDED_)
	#include "MFStreaming.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

#ifdef _WIN32
	#include "propvarutil.h"
#endif

//!
//! Media foundation media types:
//! https://gix.github.io/media-types/#ID0E5HAE
//!

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

static void MFError(HRESULT hr)
{
	if (SUCCEEDED(hr))
		return;

	std::string msg = "Error: ";

	switch (hr)
	{
		case MF_E_TOPO_CODEC_NOT_FOUND:
			msg += "Codec not found";
			break;
		case MF_E_ATTRIBUTENOTFOUND:
			msg += "Attribute not found";
			break;
		default:
			break;
	}

	RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(), msg);
}

//-----------------------------------------------------------------------------
// CorrectAspectRatio
//
// Converts a rectangle from the source's pixel aspect ratio (PAR) to 1:1 PAR.
// Returns the corrected rectangle.
//
// For example, a 720 x 486 rect with a PAR of 9:10, when converted to 1x1 PAR,  
// is stretched to 720 x 540. 
//-----------------------------------------------------------------------------
RECT CorrectAspectRatio(const RECT& src, const MFRatio& srcPAR)
{
	// Start with a rectangle the same size as src, but offset to the origin (0,0).
	RECT rc = { 0, 0, src.right - src.left, src.bottom - src.top };

	if ((srcPAR.Numerator != 1) || (srcPAR.Denominator != 1))
	{
		// Correct for the source's PAR.

		if (srcPAR.Numerator > srcPAR.Denominator)
		{
			// The source has "wide" pixels, so stretch the width.
			rc.right = MulDiv(rc.right, srcPAR.Numerator, srcPAR.Denominator);
		}
		else if (srcPAR.Numerator < srcPAR.Denominator)
		{
			// The source has "tall" pixels, so stretch the height.
			rc.bottom = MulDiv(rc.bottom, srcPAR.Denominator, srcPAR.Numerator);
		}
		// else: PAR is 1:1, which is a no-op.
	}
	return rc;
}


CMFStreaming::CMFStreaming()
	:frameLength(0), streamPos(0), locked(false),
	m_pReader(NULL), m_pSample(NULL), m_pFrameBuffer(NULL), m_bCanSeek(false)
{
	ZeroMemory(&m_format, sizeof(m_format));
}

CMFStreaming::~CMFStreaming()
{
	if (NULL != m_pFrameBuffer)
		delete[] m_pFrameBuffer;

	if (NULL != m_pReader)
		closeReader();
}

bool CMFStreaming::isOfKind(const std::string &kind) const
{
	return _isOfKind(kind);
}

bool CMFStreaming::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return (("3G2" == ext) ||
			("3GP" == ext) ||
			("3GP2" == ext) ||
			("3GPP" == ext) ||
			("ASF" == ext) ||
			("AAC" == ext) ||
			("ADTS" == ext) ||
			("MP3" == ext) ||
			("M4A" == ext) ||
			("M4V" == ext) ||
			("MOV" == ext) ||
			("MP4" == ext) ||
			("SAMI" == ext) ||
			("SMI" == ext) ||
			("WAV" == ext));
}

std::vector<std::string> CMFStreaming::getKind(void) const
{ 
	std::vector<std::string> result;

	result.push_back("3G2");
	result.push_back("3GP");
	result.push_back("3GP2");
	result.push_back("3GPP");
	result.push_back("ASF");
	result.push_back("AAC");
	result.push_back("ADTS");
	result.push_back("MP3");
	result.push_back("M4A");
	result.push_back("M4V");
	result.push_back("MOV");
	result.push_back("MP4");
	result.push_back("SAMI");
	result.push_back("SMI");
	result.push_back("WAV");

	return result;
}

bool CMFStreaming::openRecorder(const std::string& fname, unsigned int width, unsigned int height)
{
	return false;
}

bool CMFStreaming::writeFrame(const uint8_t *writeBuffer)
{
	return false;
}

bool CMFStreaming::closeRecorder(void)
{
	return false;
}


bool CMFStreaming::readFrame(uint8_t *& readBuffer, float timestamp)
{
	if ((!locked) || (m_pReader == NULL))
		return false;

	readBuffer = m_pFrameBuffer;
	SafeRelease(&m_pSample);

	if (m_bCanSeek && (timestamp > 0))
	{
		PROPVARIANT var;
		PropVariantInit(&var);

		var.vt = VT_I8;
		var.hVal.QuadPart = timestamp * 100000000;

		HRESULT hr = m_pReader->SetCurrentPosition(GUID_NULL, var);
		if (FAILED(hr))
		{
			MFError(hr);
			return false;
		}

	}

	DWORD		dwFlags = 0;
	LONGLONG    hnsTimeStamp = 0;
	HRESULT hr = m_pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL,
										&dwFlags, &hnsTimeStamp, &m_pSample);

	if (FAILED(hr))
	{
		MFError(hr);
		return false;
	}

	if ((dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) || (m_pSample == NULL))
		return false;

	if (dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
	{
		// Type change. Get the new format.
		hr = GetVideoFormat(&m_format);
		if (FAILED(hr))
		{
			SafeRelease(&m_pSample);
			MFError(hr);
			return false;
		}
	}

	//	Time stamp is in 100ns format.
	lastFrameTime = (float)hnsTimeStamp / 100000000.0f;

	if (m_pSample)
	{
		UINT32 pitch = 4 * m_format.imageWidthPels;

		// Get the bitmap data from the sample, and use it to create a
		// Direct2D bitmap object. Then use the Direct2D bitmap to 
		// initialize the sprite.
		IMFMediaBuffer *pBuffer = 0;
		hr = m_pSample->ConvertToContiguousBuffer(&pBuffer);
		if (FAILED(hr))
		{
			MFError(hr);
			return false;
		}

		BYTE        *pBitmapData = NULL;    // Bitmap data
		DWORD       cbBitmapData = 0;       // Size of data, in bytes
		hr = pBuffer->Lock(&pBitmapData, NULL, &cbBitmapData);

		if (FAILED(hr))
		{
			MFError(hr);
			return false;
		}

		if (cbBitmapData != (pitch * m_format.imageHeightPels))
		{
			return false;
		}

		//memcpy(m_pFrameBuffer, pBitmapData, cbBitmapData);
		for (int i = 0, j = 0; i < cbBitmapData; i += 4, j += 3)
		{
			m_pFrameBuffer[j + 0] = pBitmapData[i + 2];
			m_pFrameBuffer[j + 1] = pBitmapData[i + 1];
			m_pFrameBuffer[j + 2] = pBitmapData[i + 0];
		}

		if (pBitmapData)
			pBuffer->Unlock();

		SafeRelease(&pBuffer);
	}
	else
	{
		hr = MF_E_END_OF_STREAM;
		return false;
	}

	streamPos++;
	return true;
}

bool CMFStreaming::closeReader(void)
{
	SafeRelease(&m_pReader);
	SafeRelease(&m_pSample);

	HRESULT hr = MFShutdown();

	//CoUninitialize();

	return (SUCCEEDED(hr));
}

bool CMFStreaming::openReader(const std::string &fname)
{
	if (locked)
		return false;

	HRESULT hr = MFStartup(MF_VERSION);

	IMFAttributes *pAttributes = NULL;
	SafeRelease(&m_pReader);

	// Configure the source reader to perform video processing.
	//
	// This includes:
	//   - YUV to RGB-32
	//   - Software deinterlace
	hr = MFCreateAttributes(&pAttributes, 1);
	if (SUCCEEDED(hr))
		hr = pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
	else
		MFError(hr);

	// Create the source reader from the URL.
	if (SUCCEEDED(hr))
	{
		WCHAR wszFileName[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0L, fname.c_str(), (int)(fname.length() + 1), wszFileName, MAX_PATH);

		hr = MFCreateSourceReaderFromURL(wszFileName, pAttributes, &m_pReader);
	}
	else
		MFError(hr);

	// Attempt to find a video stream.
	if (SUCCEEDED(hr))
		hr = SelectVideoStream();

	if (FAILED(hr))
		return false;

	locked = true;
	return true;
}

HRESULT CMFStreaming::SelectVideoStream()
{
	HRESULT hr = S_OK;
	IMFMediaType *pType = NULL;

	// Configure the source reader to give us progressive RGB32 frames.
	// The source reader will load the decoder if needed.

	hr = MFCreateMediaType(&pType);

	if (SUCCEEDED(hr))
		hr = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

	if (SUCCEEDED(hr))
		hr = pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);

	if (SUCCEEDED(hr))
		hr = m_pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType);

	// Ensure the stream is selected.
	if (SUCCEEDED(hr))
		hr = m_pReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);

	if (SUCCEEDED(hr))
		hr = GetVideoFormat(&m_format);
	
	if (FAILED(hr))
		MFError(hr);
	else
	{
		// Allocate an RGBA buffer (worst case allocation)
		m_pFrameBuffer = new uint8_t[m_format.imageHeightPels * m_format.imageWidthPels * 4];
	}

	SafeRelease(&pType);
	return hr;
}

HRESULT CMFStreaming::GetVideoFormat(CMFStreaming::FormatInfo *pFormat)
{
	IMFMediaType *pType = NULL;
	HRESULT hr = m_pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pType);

	// Get the width and height
	UINT32 width = 0;
	UINT32 height = 0;
	if (SUCCEEDED(hr))
		hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);

	if (FAILED(hr))
	{
		SafeRelease(&pType);
		return hr;
	}
	else
	{
		pFormat->imageWidthPels = width;
		pFormat->imageHeightPels = height;
	}

	// Get the stride to find out if the bitmap is top-down or bottom-up.
	LONG lStride = (LONG)MFGetAttributeUINT32(pType, MF_MT_DEFAULT_STRIDE, 1);

	pFormat->bTopDown = (lStride > 0);

	// Get the pixel aspect ratio. (This value might not be set.)
	MFRatio par = { 0 , 0 };
	hr = MFGetAttributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, (UINT32*)&par.Numerator, (UINT32*)&par.Denominator);
	if (SUCCEEDED(hr) && (par.Denominator != par.Numerator))
	{
		RECT rcSrc = { 0, 0, (LONG)width, (LONG)height };

		pFormat->rcPicture = CorrectAspectRatio(rcSrc, par);
	}
	else
	{
		// Either the PAR is not set (assume 1:1), or the PAR is set to 1:1.
		SetRect(&pFormat->rcPicture, 0, 0, width, height);
	}

	// Get the frame-rate (in second)
	MFRatio rate = { 0 , 0 };
	hr = MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, (UINT32*)&rate.Numerator, (UINT32*)&rate.Denominator);
	if (SUCCEEDED(hr) && (rate.Denominator != rate.Numerator))
	{
		frameLength = 1.0f / (rate.Numerator / rate.Denominator);
	}
	else
	{
		// Either the frame rate is not set or the frame rate is set to 1:1
		frameLength = 1.0f;
	}

	bool pbCanSeek = false;
	hr = CanSeek(&pbCanSeek);
	if (SUCCEEDED(hr))
	{
		m_bCanSeek = pbCanSeek;
	}

	SafeRelease(&pType);
	if (!SUCCEEDED(hr))
		MFError(hr);

	return hr;
}

//-------------------------------------------------------------------
// CanSeek: Queries whether the current video file is seekable.
//-------------------------------------------------------------------
HRESULT CMFStreaming::CanSeek(bool *pbCanSeek)
{
	if (m_pReader == NULL)
	{
		return MF_E_NOT_INITIALIZED;
	}
	
	PROPVARIANT var;
	PropVariantInit(&var);

	HRESULT hr = m_pReader->GetPresentationAttribute(	(DWORD)MF_SOURCE_READER_MEDIASOURCE,
														MF_SOURCE_READER_MEDIASOURCE_CHARACTERISTICS,
														&var);
	ULONG flags = 0;
	if (SUCCEEDED(hr))
	{
		hr = PropVariantToUInt32(var, &flags);
	}

	if (SUCCEEDED(hr))
	{
		*pbCanSeek = FALSE;

		if ((flags & MFMEDIASOURCE_CAN_SEEK) && !(flags & MFMEDIASOURCE_HAS_SLOW_SEEK))
		{
			*pbCanSeek = TRUE;
		}
	}

	return hr;
}



/*

RECT CorrectAspectRatio(const RECT& src, const MFRatio& srcPAR);

const LONGLONG SEEK_TOLERANCE = 10000000;
const LONGLONG MAX_FRAMES_TO_SKIP = 10;

//-------------------------------------------------------------------
// GetDuration: Finds the duration of the current video file.
//-------------------------------------------------------------------

HRESULT ThumbnailGenerator::GetDuration(LONGLONG *phnsDuration)
{
	PROPVARIANT var;
	PropVariantInit(&var);

	HRESULT hr = S_OK;

	if (m_pReader == NULL)
	{
		return MF_E_NOT_INITIALIZED;
	}

	hr = m_pReader->GetPresentationAttribute(
		(DWORD)MF_SOURCE_READER_MEDIASOURCE,
		MF_PD_DURATION,
		&var
	);

	if (SUCCEEDED(hr))
	{
		assert(var.vt == VT_UI8);
		*phnsDuration = var.hVal.QuadPart;
	}

	PropVariantClear(&var);

	return hr;
}

//-------------------------------------------------------------------
// CreateBitmaps
//
// Creates an array of thumbnails from the video file.
//
// pRT:      Direct2D render target. Used to create the bitmaps.
// count:    Number of thumbnails to create.
// pSprites: An array of Sprite objects to hold the bitmaps.
//
// Note: The caller allocates the sprite objects.
//-------------------------------------------------------------------

HRESULT ThumbnailGenerator::CreateBitmaps(
	ID2D1RenderTarget *pRT,
	DWORD count,
	Sprite pSprites[]
)
{
	HRESULT hr = S_OK;
	BOOL bCanSeek = 0;

	LONGLONG hnsDuration = 0;
	LONGLONG hnsRangeStart = 0;
	LONGLONG hnsRangeEnd = 0;
	LONGLONG hnsIncrement = 0;

	hr = CanSeek(&bCanSeek);

	if (FAILED(hr)) { return hr; }

	if (bCanSeek)
	{
		hr = GetDuration(&hnsDuration);

		if (FAILED(hr)) { return hr; }

		hnsRangeStart = 0;
		hnsRangeEnd = hnsDuration;

		// We have the file duration , so we'll take bitmaps from
		// several positions in the file. Occasionally, the first frame 
		// in a video is black, so we don't start at time 0.

		hnsIncrement = (hnsRangeEnd - hnsRangeStart) / (count + 1);

	}

	// Generate the bitmaps and invalidate the button controls so
	// they will be redrawn.
	for (DWORD i = 0; i < count; i++)
	{
		LONGLONG hPos = hnsIncrement * (i + 1);

		hr = CreateBitmap(
			pRT,
			hPos,
			&pSprites[i]
		);
	}

	return hr;
}


//
/// Private methods
//

//-------------------------------------------------------------------
// CreateBitmap
//
// Creates one video thumbnail.
//
// pRT:      Direct2D render target. Used to create the bitmap.
// hnsPos:   The seek position.
// pSprite:  A Sprite object to hold the bitmap.
//-------------------------------------------------------------------

HRESULT ThumbnailGenerator::CreateBitmap(
	ID2D1RenderTarget *pRT,
	LONGLONG& hnsPos,
	Sprite *pSprite
)
{
	HRESULT     hr = S_OK;
	DWORD       dwFlags = 0;

	BYTE        *pBitmapData = NULL;    // Bitmap data
	DWORD       cbBitmapData = 0;       // Size of data, in bytes
	LONGLONG    hnsTimeStamp = 0;
	BOOL        bCanSeek = FALSE;       // Can the source seek?  
	DWORD       cSkipped = 0;           // Number of skipped frames

	IMFMediaBuffer *pBuffer = 0;
	IMFSample *pSample = NULL;
	ID2D1Bitmap *pBitmap = NULL;

	hr = CanSeek(&bCanSeek);
	if (FAILED(hr))
	{
		return hr;
	}

	if (bCanSeek && (hnsPos > 0))
	{
		PROPVARIANT var;
		PropVariantInit(&var);

		var.vt = VT_I8;
		var.hVal.QuadPart = hnsPos;

		hr = m_pReader->SetCurrentPosition(GUID_NULL, var);

		if (FAILED(hr)) { goto done; }

	}


	// Pulls video frames from the source reader.

	// NOTE: Seeking might be inaccurate, depending on the container
	//       format and how the file was indexed. Therefore, the first
	//       frame that we get might be earlier than the desired time.
	//       If so, we skip up to MAX_FRAMES_TO_SKIP frames.

	while (1)
	{
		IMFSample *pSampleTmp = NULL;

		hr = m_pReader->ReadSample(
			(DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			NULL,
			&dwFlags,
			NULL,
			&pSampleTmp
		);

		if (FAILED(hr)) { goto done; }

		if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		if (dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
		{
			// Type change. Get the new format.
			hr = GetVideoFormat(&m_format);

			if (FAILED(hr)) { goto done; }
		}

		if (pSampleTmp == NULL)
		{
			continue;
		}

		// We got a sample. Hold onto it.

		SafeRelease(&pSample);

		pSample = pSampleTmp;
		pSample->AddRef();

		if (SUCCEEDED(pSample->GetSampleTime(&hnsTimeStamp)))
		{
			// Keep going until we get a frame that is within tolerance of the
			// desired seek position, or until we skip MAX_FRAMES_TO_SKIP frames.

			// During this process, we might reach the end of the file, so we
			// always cache the last sample that we got (pSample).

			if ((cSkipped < MAX_FRAMES_TO_SKIP) &&
				(hnsTimeStamp + SEEK_TOLERANCE < hnsPos))
			{
				SafeRelease(&pSampleTmp);

				++cSkipped;
				continue;
			}
		}

		SafeRelease(&pSampleTmp);

		hnsPos = hnsTimeStamp;
		break;
	}

	if (pSample)
	{
		UINT32 pitch = 4 * m_format.imageWidthPels;

		// Get the bitmap data from the sample, and use it to create a
		// Direct2D bitmap object. Then use the Direct2D bitmap to 
		// initialize the sprite.

		hr = pSample->ConvertToContiguousBuffer(&pBuffer);

		if (FAILED(hr)) { goto done; }

		hr = pBuffer->Lock(&pBitmapData, NULL, &cbBitmapData);

		if (FAILED(hr)) { goto done; }

		assert(cbBitmapData == (pitch * m_format.imageHeightPels));

		hr = pRT->CreateBitmap(
			D2D1::SizeU(m_format.imageWidthPels, m_format.imageHeightPels),
			pBitmapData,
			pitch,
			D2D1::BitmapProperties(
				// Format = RGB32
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
			),
			&pBitmap
		);

		if (FAILED(hr)) { goto done; }

		pSprite->SetBitmap(pBitmap, m_format);
	}
	else
	{
		hr = MF_E_END_OF_STREAM;
	}

done:

	if (pBitmapData)
	{
		pBuffer->Unlock();
	}
	SafeRelease(&pBuffer);
	SafeRelease(&pSample);
	SafeRelease(&pBitmap);

	return hr;
}






const float WOBBLE_ANGLE = 10.0f;
const float WOBBLE_DECAY = 0.25f;



const D2D1_COLOR_F      BACKGROUND_COLOR = D2D1::ColorF(D2D1::ColorF::DarkSlateGray);
const DWORD             MAX_SPRITES = 4;
const float             ANIMATION_DURATION = 0.4f;


// Global variables

ThumbnailGenerator      g_ThumbnailGen;
Timer                   g_Timer;

Sprite                  g_pSprites[MAX_SPRITES];
int                     g_Selection = -1;   // Which sprite is selected (-1 = no selection)

ID2D1HwndRenderTarget   *g_pRT = NULL;      // Render target for D2D animation

//
//  DPI Scaling logic - helper functions to allow us to respect DPI settings.
//
float g_fDPIScaleX = 1.0f;
float g_fDPIScaleY = 1.0f;



// The following rectangles define the small (unselected) and 
// big (selected) locations for the sprites.

const D2D1_RECT_F g_rcSmall[] = {

	//    left   top    right  bottom
	D2D1::RectF(0.05f, 0.0f,  0.2f,  0.25f),
	D2D1::RectF(0.05f, 0.25f, 0.2f,  0.50f),
	D2D1::RectF(0.05f, 0.50f, 0.2f,  0.75f),
	D2D1::RectF(0.05f, 0.75f, 0.2f,  1.00f)
};

const D2D1_RECT_F g_rcBig = D2D1::RectF(0.25f, 0.05f, 0.95f, 0.95f);



void InitializeDPIScale(HWND hWnd)
{
	//
	//  Create the font to be used in the OSD, making sure that
	//  the font size is scaled system DPI setting.
	//
	HDC hdc = GetDC(hWnd);
	g_fDPIScaleX = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
	g_fDPIScaleY = GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
	ReleaseDC(hWnd, hdc);
}

int DPIScaleX(int iValue)
{
	return static_cast<int>(iValue / g_fDPIScaleX);
}
int DPIScaleY(int iValue)
{
	return static_cast<int>(iValue / g_fDPIScaleY);
}

*/




