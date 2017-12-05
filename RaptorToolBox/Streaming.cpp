// Streaming.cpp: implementation of the CStreaming class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif

#if !defined(AFX_STREAMING_H__F765F8BE_C681_4B49_8535_B4B8CD341886__INCLUDED_)
    #include "Streaming.h"
#endif

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
    #include "Engine/Animator.h"
#endif

#if !defined(AFX_AVISTREAMING_H__BA673CA4_15E6_4B9E_8AF0_EC0184F57FDD__INCLUDED_)
	#include "Streaming/AVIStreaming.h"
#endif
#if !defined(AFX_WMVSTREAMING_H__14310AA1_ECB0_48C7_B69F_C0816C83B1CC__INCLUDED_)
	#include "Streaming/WMVStreaming.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreaming::CStreaming()
{

}

CStreaming::~CStreaming()
{

}


bool CStreaming::installStreamers(void)
{
    CAnimator *pAnimator = CAnimator::GetAnimator();
    if (pAnimator == NULL)
        return false;

#if defined(_WIN32)
    pAnimator->setVideoKindIO(new CAVIStreaming());
	pAnimator->setVideoKindIO(new CWMVStreaming());
#endif

    return true;
}

bool CStreaming::installStreamer(const std::string& streamType)
{
    bool res = true;

    string ext;
	for (size_t pos=0;pos<streamType.size();pos++)
        ext[pos] = toupper(streamType[pos]);

    CAnimator *pAnimator = CAnimator::GetAnimator();
    if (pAnimator == NULL)
        return false;

#if defined(_WIN32)
    if (ext == "AVI")
        pAnimator->setVideoKindIO(new CAVIStreaming());
	if (ext == "WMV")
        pAnimator->setVideoKindIO(new CWMVStreaming());
#endif
    
    return res;
}




