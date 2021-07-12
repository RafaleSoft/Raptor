// Streaming.cpp: implementation of the CStreaming class.
//
//////////////////////////////////////////////////////////////////////

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
#if !defined(AFX_MFSTREAMING_H__25A33139_3FCF_477D_9509_1BA9A2F0F6B9__INCLUDED_)
	#include "Streaming/MFStreaming.h"
#endif
#if !defined(AFX_DSHOWSTREAMING_H__039A618A_339A_4BE8_B61F_E7DA513185AA__INCLUDED_)
	#include "Streaming/DShowStreaming.h"
#endif
#if !defined(AFX_NVSTREAMING_H__AC9C0954_0871_48E0_B487_07FC76C2CF47__INCLUDED_)
	#include "Streaming/NVStreaming.h"
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
	pAnimator->setVideoKindIO(new CDShowStreaming());
	pAnimator->setVideoKindIO(new CMFStreaming());
	pAnimator->setVideoKindIO(new CWMVStreaming());
	pAnimator->setVideoKindIO(new CNVStreaming());
	pAnimator->setVideoKindIO(new CAVIStreaming());
#endif

    return true;
}

bool CStreaming::installStreamer(const std::string& streamType)
{
    bool res = true;

    string ext = streamType;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

    CAnimator *pAnimator = CAnimator::GetAnimator();
    if (pAnimator == NULL)
        return false;

#if defined(_WIN32)
	if (CDShowStreaming::_isOfKind(ext))
		pAnimator->setVideoKindIO(new CDShowStreaming());
	else if (CMFStreaming::_isOfKind(ext))
		pAnimator->setVideoKindIO(new CMFStreaming());
	else if (CWMVStreaming::_isOfKind(ext))
        pAnimator->setVideoKindIO(new CWMVStreaming());
	else if (CNVStreaming::_isOfKind(ext))
		pAnimator->setVideoKindIO(new CNVStreaming());
	else if (CAVIStreaming::_isOfKind(ext))
		pAnimator->setVideoKindIO(new CAVIStreaming());
	else
		res = false;
#endif
    
    return res;
}




