/***************************************************************************/
/*                                                                         */
/*  AVIStreaming.cpp                                                       */
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
#if !defined(AFX_AVISTREAMING_H__BA673CA4_15E6_4B9E_8AF0_EC0184F57FDD__INCLUDED_)
	#include "Streaming/AVIStreaming.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

#if defined(_WIN32)

std::vector<std::string> CAVIStreaming::getKind(void) const
{
	std::vector<std::string> result;

	result.push_back("AVI");

	return result;
}

bool CAVIStreaming::isOfKind(const std::string &kind) const
{
	return _isOfKind(kind);
}

bool CAVIStreaming::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return ("AVI" == ext);
}

bool CAVIStreaming::openReader(const std::string &fname)
{
    if (locked)
        return false;

    AVIFileInit();

	if (AVIERR_OK != AVIFileOpen(&AVIFile,fname.data(),OF_SHARE_DENY_WRITE|OF_READ,NULL))
	{
		AVIFile = NULL;
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"File Open Failed");
		return false;
	}

	AVIFILEINFO	aviInfo;
	if (AVIERR_OK != AVIFileInfo(AVIFile,&aviInfo,sizeof(AVIFILEINFO)))
		return false;

	if (AVIERR_OK==AVIFileGetStream(AVIFile,&AVIStream,streamtypeVIDEO,0))
	{
		AVISTREAMINFO psi;

		if (AVIERR_OK==AVIStreamInfo(AVIStream,&psi,sizeof(AVISTREAMINFO)))
		{
            frameLength = (float)psi.dwScale / (float)psi.dwRate;
            streamPos = psi.dwStart;
			DWORD dwSize = sizeof(BITMAPINFOHEADER);

			AVIBuffer = (LPBITMAPINFOHEADER)malloc(dwSize);
			if (NULL == AVIBuffer)
				return false;

			AVIBuffer->biSize = sizeof(BITMAPINFOHEADER) ; 
			AVIBuffer->biWidth = psi.rcFrame.right - psi.rcFrame.left;
			AVIBuffer->biHeight = psi.rcFrame.bottom - psi.rcFrame.top;
			AVIBuffer->biPlanes = 1 ; 
			AVIBuffer->biBitCount = 24 ; 
			AVIBuffer->biCompression = BI_RGB ; 
			AVIBuffer->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER); 
			AVIBuffer->biXPelsPerMeter = 0 ; 
			AVIBuffer->biYPelsPerMeter = 0 ; 
			AVIBuffer->biClrUsed = 0 ; 
			AVIBuffer->biClrImportant = 0 ;  

			if (NULL == (AVIFrame = AVIStreamGetFrameOpen(AVIStream,AVIBuffer)))
				AVIFrame = AVIStreamGetFrameOpen(AVIStream,NULL);
			if (AVIFrame == NULL)
			{
				RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"No Get Frame into Stream !");
				return false;
			}
		}
		else
		{
			RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Stream Info failed !");
			return false;
		}
	}
	else
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Unable to Get a interface to the Stream !");
		return false;
	}

    locked = true;
	return true;
}


bool CAVIStreaming::readFrame(unsigned char *& readBuffer, float timestamp)
{
	if (AVIFile == NULL)
		return false;

	LPVOID pDIB =  AVIStreamGetFrame(AVIFrame, streamPos++) ;
	if (pDIB != NULL)
	{
        //  Need to solve : is this pointer still available if playing 2 or more streams at the same time ?
		LPBYTE pDIBBits = (LPBYTE)pDIB + AVIBuffer->biSize;

        readBuffer = pDIBBits;

		return true;
	}
    else
        return false;
}
 
bool CAVIStreaming::closeReader(void)
{
	if (AVIFile != NULL)
    {
        AVIFileExit();
		free(AVIBuffer);
        AVIStreamRelease(AVIStream);
        AVIStreamGetFrameClose(AVIFrame);
        AVIFileRelease(AVIFile);
        locked = false;
        return true;
    }
    else
        return false;
}

bool CAVIStreaming::openRecorder(const std::string& fname,unsigned int width,unsigned int height)
{
    if (locked)
        return false;

	AVIFileInit();

	//	Creating output file
	if (AVIERR_OK != AVIFileOpen(&AVIFile,fname.data(),OF_CREATE|OF_WRITE,NULL))
	{
		AVIFile = NULL;
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"File Open Failed");
		return false;
	}

	//	preparing format structures
	RECT r = { 0 , 0 , width , height };

	UINT wLineLen = (width*32 + 31)/32 * 3; 
	DWORD dwSize = sizeof(BITMAPINFOHEADER) + (DWORD)wLineLen * (DWORD)height;
	
    cVars.cbSize = sizeof(COMPVARS);
	AVIBuffer = (LPBITMAPINFOHEADER)malloc(dwSize);
	AVIBuffer->biSize = sizeof(BITMAPINFOHEADER) ; 
	AVIBuffer->biWidth = width;
	AVIBuffer->biHeight = height;
	AVIBuffer->biPlanes = 1 ; 
	AVIBuffer->biBitCount = 24 ; 
	AVIBuffer->biCompression = BI_RGB ; 
	AVIBuffer->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER); 
	AVIBuffer->biXPelsPerMeter = 0 ; 
	AVIBuffer->biYPelsPerMeter = 0 ; 
	AVIBuffer->biClrUsed = 0 ; 
	AVIBuffer->biClrImportant = 0 ;  

    streamPos = 0;

	//	creating a video compressor
	bool result=(TRUE==ICCompressorChoose(	NULL,
									0,//ICMF_CHOOSE_ALLCOMPRESSORS,
									AVIBuffer,    
									NULL,
									&cVars,   
									"Choose a compressor..."));	

	//	setting stream format
	AVISTREAMINFO psi;
	memset(&psi,0,sizeof(AVISTREAMINFO));
	psi.fccType = streamtypeVIDEO;
	psi.dwScale = 1; 
	psi.dwRate = 30; 
	psi.dwSuggestedBufferSize = width*height*3; 
	psi.rcFrame = r;
	psi.dwQuality = 10000;
	psi.dwSampleSize = 0;

	if (result)
	{
		//	AVI does not support 'whole' file compression with mmioFOURCC('v','i','d','c')
		//psi.fccType = cVars.fccType;
		psi.fccHandler = cVars.fccHandler;
		psi.dwQuality = cVars.lQ;
		ICOpen(cVars.fccType,cVars.fccHandler,ICMODE_COMPRESS);
	}
	else
		cVars.hic = NULL;
	
	if ( AVIERR_OK != AVIFileCreateStream(AVIFile,&AVIStream,&psi))
	{
		RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Create Stream Failed");
		return false;
	}

	if (result)
	{
		//	Set Stream format to compressor output format
		if (AVIERR_OK == AVIStreamSetFormat(AVIStream, 0,&(cVars.lpbiOut->bmiHeader),sizeof(BITMAPINFOHEADER)))
		{
			//	Init compressor resources and source format
			if (FALSE==ICSeqCompressFrameStart(&cVars,(LPBITMAPINFO)AVIBuffer))
			{
				RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Compress Frame Start Failed");
				return false;
			}
		}
		else
		{
			RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Stream Set Format Failed");
			return false;
		}
	}
	else
	{
		if (AVIERR_OK != AVIStreamSetFormat(AVIStream, 0,AVIBuffer,sizeof(BITMAPINFOHEADER)))
		{
			RAPTOR_ERROR(CAnimator::CAnimatorClassID::GetClassId(),"Stream Set Format Failed");
			return false;
		}
	}

    locked = true;
    return true;
}

bool CAVIStreaming::closeRecorder(void)
{
	if (AVIFile != NULL)
	{
        AVIFileExit();
		if (cVars.hic != NULL)
		{
			ICSeqCompressFrameEnd(&cVars);
			ICClose(cVars.hic);
			ICCompressorFree(&cVars);
		}
		free(AVIBuffer);
		AVIStreamRelease(AVIStream);
		AVIFileRelease(AVIFile);	
        
        locked = false;
        return true;
	}
    else
        return false;
}

bool CAVIStreaming::writeFrame(const unsigned char *writeBuffer)
{
	if (AVIFile == NULL)
		return false;

	LONG size = AVIBuffer->biSizeImage;
	DWORD fType = AVIIF_KEYFRAME;
    LPBYTE outBits = (LPBYTE)writeBuffer;

	if (cVars.hic != NULL)
	{
		BOOL fKey;
		LPBYTE pBits = (LPBYTE)ICSeqCompressFrame(	&cVars, 0,
													outBits,
													&fKey,
													&size);
		if (pBits != NULL) 
            outBits = pBits;

		if (!fKey) 
            fType = 0;
	}

	return ( AVIERR_OK == AVIStreamWrite(AVIStream,
			                            streamPos++, 1,
			                            outBits, size,
			                            fType,
			                            NULL,NULL));
}

#endif

