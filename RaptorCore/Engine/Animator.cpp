/***************************************************************************/
/*                                                                         */
/*  Animator.cpp                                                           */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
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

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Animator.h"
#endif

#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif

#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "TimeObject.h"
#endif

#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

#if !defined(AFX_VIDEOSTREAM_H__F651DAEA_2E78_45B6_B06E_BBAD563283E5__INCLUDED_)
    #include "Subsys/VideoStream.h"
#endif

#if !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)
    #include "Subsys/AnimatorStream.h"
#endif

#include "Subsys/Interpreters.h"


RAPTOR_NAMESPACE


static CAnimator::CAnimatorClassID animatorId;
const CPersistence::CPersistenceClassID& CAnimator::CAnimatorClassID::GetClassId(void)
{
	return animatorId;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAnimator::CAnimator(const std::string& name)
    :CPersistence(animatorId,name),
    m_sFrameDelay(0),deltat(0.0f)
{
	m_pStream = new CAnimatorStream();
	m_sCount = 0;
}

CAnimator::~CAnimator()
{
	delete m_pStream;
}

void CAnimator::SetAnimator(CAnimator *Animator)
{
	CRaptorInstance::GetInstance().pAnimator = Animator;
}

CAnimator *CAnimator::GetAnimator(void)
{
	return CRaptorInstance::GetInstance().pAnimator;
}


void CAnimator::animate(void)
{
	float dt = 0.0f;
	if (deltat > 0.0f)
		dt = CTimeObject::GetTime() - deltat;
	deltat = CTimeObject::GetTime();

	if (m_sCount == m_sFrameDelay)
	{
		//! Lock only the section manipulating timed objects
		{
			CRaptorLock lock(CTimeObject::getLock());

			const vector<CTimeObject*>& root = CTimeObject::getTimeObjects();
			
			vector<CTimeObject*>::const_iterator itr = root.begin();
			while (itr != root.end())
			{
				CTimeObject *value = (*itr++);
				value->synchronize(dt, true);
			}
		}

		m_pStream->playFrames(dt);
		m_sCount = 0;
	}
	else
		m_sCount++;
}

void CAnimator::animateAll(bool animate)
{
	const vector<CTimeObject*>& root = CTimeObject::getTimeObjects();
	//	Update animation objects
	vector<CTimeObject*>::const_iterator itr = root.begin();
	while (itr != root.end())
	{
		CTimeObject *value = (*itr++);
		value->animate(animate);
	}
}

void CAnimator::asyncAnimate(void)
{
}

void CAnimator::initSynchro(void)
{
	if (m_sCount == m_sFrameDelay)
	{
		CRaptorLock lock(CTimeObject::getLock());

		const vector<CTimeObject*>& root = CTimeObject::getTimeObjects();
		vector<CTimeObject*>::const_iterator itr = root.begin();
		while (itr != root.end())
		{
			CTimeObject *value = (*itr++);
			value->synchronize(0.0f, false);
		}

		m_sCount = 0;
	}
}

bool CAnimator::startRecord(const std::string& fname,unsigned int width,unsigned int height)
{
	return m_pStream->startRecord(fname,width,height);
}

void CAnimator::endRecord(void)
{
	m_pStream->endRecord();
}

bool CAnimator::recordFrame(void)
{
	return m_pStream->recordFrame();

	CATCH_GL_ERROR
}

ITextureGenerator * CAnimator::glStartPlayBack(const std::string& fname,bool loop) const
{
    ITextureGenerator *res = NULL;

    long nStream = -1;
    if (0 <= (nStream = m_pStream->glStartPlayBack(fname,loop)))
        res = new CVideoStream(m_pStream,nStream);

    return res;
}

void CAnimator::endPlayBack(unsigned int nStream )
{
	m_pStream->endPlayBack(nStream);
}

bool CAnimator::getFrameSizes(unsigned int stream, unsigned int &width, unsigned int &height) const
{
    return m_pStream->getFrameSizes(stream, width, height);
}

void CAnimator::setVideoKindIO(CAnimator::IVideoIO *streamer)
{
    m_pStream->setVideoKindIO(streamer);
}

CAnimator::IVideoIO* const CAnimator::getVideoKindIO(const std::string& extension) const
{
    return m_pStream->getVideoKindIO(extension);
}

