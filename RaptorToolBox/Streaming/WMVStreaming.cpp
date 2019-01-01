// AVIStreaming.cpp: implementation of the CAVIStreaming class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if _MSC_VER < 1201
    // Remove this define and use the platform SDK server 2003 R3 or higher
    #define __RPCNDR_H_VERSION__ (475)
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_WMVSTREAMING_H__14310AA1_ECB0_48C7_B69F_C0816C83B1CC__INCLUDED_)
	#include "Streaming/WMVStreaming.h"
#endif


#if defined(_WIN32)

void CWMVStreaming::seekFrame(unsigned int framePos)
{ 
	streamPos = framePos; 
    lastFrameTime = 0;
    HRESULT hr = m_pReader->SetRangeByFrame( m_wNumStream, streamPos, 0);
    if ( FAILED( hr ) )
    {
        // what should I do here ?
		return ;
    }
}

bool CWMVStreaming::readFrame(unsigned char *& readBuffer)
{
	if ((!locked) || (m_pReader == NULL))
		return false;

	if (m_pBuffer != NULL)
		m_pBuffer->Release();
	m_pBuffer = NULL;

	QWORD		cnsSampleTime = 0;
	QWORD		cnsDuration = 0;
	DWORD		dwFlags = 0;
	DWORD		dwOutputNum = 0;
	WORD		wStreamNum = 0;

	HRESULT hr = m_pReader->GetNextSample(	m_wNumStream,
											&m_pBuffer,
											&cnsSampleTime,
											&cnsDuration,
											&dwFlags,
											&dwOutputNum,
											&wStreamNum);
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

	return true;
}

bool CWMVStreaming::openReader(const std::string &fname)
{
	if (locked)
        return false;

	HRESULT hr = CoInitialize( NULL );
    if( FAILED( hr ) )
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Failed to initialize COM layer");
		return false;
	}

	if ( NULL == m_pReader )
        hr = WMCreateSyncReader(  NULL, 0, &m_pReader );
    if ( FAILED( hr ) )
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Failed to create WMV Sync Reader");
		return false;
	}

	WCHAR wFname[MAX_PATH];
	MultiByteToWideChar( CP_ACP
                       , 0L
					   , fname.c_str()
					   , fname.length() + 1
                       , wFname
                       , MAX_PATH );

	hr = m_pReader->Open(wFname);
	if ( FAILED( hr ) )
	{
		string msg = "Failed to open file " + fname;
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),msg.c_str());
		return false;
	}

	IWMProfile *pProfile = NULL;
	hr = m_pReader->QueryInterface(IID_IWMProfile,(void**)&pProfile);
	if ( FAILED( hr ) )
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"WM Profile interface is not supported by WMV Sync Reader");
		return false;
	}

	WMT_VERSION  dwVersion;
	hr = pProfile->GetVersion(&dwVersion);
	if ( FAILED( hr ) )
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Unable to get WM Version");
		pProfile->Release();
		return false;
	}

	DWORD cchName = 0;
	hr = pProfile->GetName(NULL,&cchName);
	if (SUCCEEDED(hr))
	{
		WCHAR *name = new WCHAR[cchName];
		pProfile->GetName(name,&cchName);
		delete [] name;
	}
	hr = pProfile->GetDescription(NULL,&cchName);
	if (SUCCEEDED(hr))
	{
		WCHAR*  description = new WCHAR[cchName];
		pProfile->GetDescription(description,&cchName);
		delete [] description;
	}

	DWORD  cStreams = 0;
	hr = pProfile->GetStreamCount(&cStreams);
	if ( FAILED( hr ) )
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Unable to get stream count from profile");
		pProfile->Release();
		return false;
	}


	/*
	//	Attribute check : in case of need.
	//	Rq: this is only the file level
	//
	IWMHeaderInfo *pHeader = NULL;
	hr = m_pReader->QueryInterface(IID_IWMHeaderInfo,(void**)&pHeader);
	if ( FAILED( hr ) )
		return false;

	WORD cAttributes = 0;
	hr = pHeader->GetAttributeCount(0,&cAttributes);
	if ( FAILED( hr ) )
		return false;

	for (unsigned int i=0;i<cAttributes;i++)
	{
		WORD	wStreamNum = 0;
		WORD	cchNameLen = 0;
		WMT_ATTR_DATATYPE Type;
		WORD	cbLength = 0;

		hr = pHeader->GetAttributeByIndex(i,&wStreamNum,NULL,&cchNameLen,&Type,NULL,&cbLength);
		if (SUCCEEDED(hr))
		{
			WCHAR	wszName[1024];
			BYTE	Value[1024];

			hr = pHeader->GetAttributeByIndex(i,&wStreamNum,wszName,&cchNameLen,&Type,&Value[0],&cbLength);

			if (SUCCEEDED(hr))
			{
				hr = S_OK;
			}
		}
	}
	*/


	//! Access Stream config anf Media properties to retrieve
	//!	- video stream dimensions ( width / height )
	//! - video stream number to get samples.
	for (unsigned int i=0;i<cStreams;i++)
	{
		IWMStreamConfig* pStream = NULL;
		hr = pProfile->GetStream(i,&pStream);
		if (SUCCEEDED(pStream))
		{
			GUID  guidStreamType;
			hr = pStream->GetStreamType(&guidStreamType);
			if (SUCCEEDED(hr) && (WMMEDIATYPE_Video == guidStreamType))
			{
				IWMMediaProps *pProps = NULL;
				hr = pStream->QueryInterface(IID_IWMMediaProps,(void**)&pProps);

				if (SUCCEEDED(hr))
				{
					WM_MEDIA_TYPE	*pType = NULL;
					DWORD			cbType = 0;

					hr = pProps->GetMediaType(NULL,&cbType);
					if (SUCCEEDED(hr))
					{
						pType = (WM_MEDIA_TYPE*)(malloc(cbType));
						hr = pProps->GetMediaType(pType,&cbType);

						if (pType->formattype == WMFORMAT_VideoInfo)
						{
							WMVIDEOINFOHEADER *header = (WMVIDEOINFOHEADER *)(pType->pbFormat);
							frameLength = header->AvgTimePerFrame / 10000000;
							m_width = header->bmiHeader.biWidth;
							m_height = header->bmiHeader.biHeight;

							WORD wStreamNum = 0;
							hr = pStream->GetStreamNumber(&wStreamNum);
							if (SUCCEEDED(hr))
  								m_wNumStream = wStreamNum;
						}

						free(pType);
					}
				}

				if (pProps != NULL)
					pProps->Release();

				WMT_STREAM_SELECTION  selection = WMT_ON;
				hr = m_pReader->SetStreamsSelected(1,&m_wNumStream,&selection);
				hr = m_pReader->SetReadStreamSamples(m_wNumStream,FALSE);
			}
			else
			{
				WORD wStreamNum = 0;
				hr = pStream->GetStreamNumber(&wStreamNum);

				WMT_STREAM_SELECTION  selection = WMT_OFF;
				hr = m_pReader->SetStreamsSelected(1,&wStreamNum,&selection);
				hr = m_pReader->SetReadStreamSamples(wStreamNum,FALSE);
			}
		}

		if (pStream != NULL)
			pStream->Release();
	}

	if (pProfile != NULL)
		pProfile->Release();

	//!	This call seems to be mandatory, even if by default
	//! I only need to receive samples from the beginning of the file.
	//! If I do not call this, I receive a 'codec not installed' error 
	//! in GetNextSample
	hr = m_pReader->SetRange( 0, 0);
    if ( FAILED( hr ) )
		return false;

	//! Retrieve reader's outputs to configure precise frame durations
	DWORD	cOutputs = 0;
	hr = m_pReader->GetOutputCount(&cOutputs);
	if (SUCCEEDED(hr))
	{
		for (unsigned int i=0;i<cOutputs;i++)
		{
			IWMOutputMediaProps*	pOutput = NULL;
			hr = m_pReader->GetOutputProps(i,&pOutput);
			if (FAILED(hr))
				continue;

			GUID	guidType;
			hr = pOutput->GetType(&guidType);

			if ((SUCCEEDED(hr)) && (guidType == WMMEDIATYPE_Video))
			{
				BOOL value = TRUE;
				hr = m_pReader->SetOutputSetting(	i,g_wszVideoSampleDurations,
													WMT_TYPE_BOOL, 
													(BYTE*)&value,sizeof(BOOL));
			}

			pOutput->Release();
		}
	}

	locked = true;
	return true;
}

bool CWMVStreaming::closeReader(void)
{
	HRESULT hr = S_OK;
	if( NULL != m_pReader )
        hr = m_pReader->Close();
    
    if( FAILED ( hr ) )
        return false;

    if (m_pReader != NULL)
	{
		m_pReader->Release();
		m_pReader = NULL;
	}

	CoUninitialize( );

	locked = false;
    return true;
}

bool CWMVStreaming::openRecorder(const std::string& fname,unsigned int width,unsigned int height)
{
	return false;
}

bool CWMVStreaming::writeFrame(const unsigned char *writeBuffer)
{
	return false;
}

bool CWMVStreaming::closeRecorder(void)
{
	return false;
}

#endif

