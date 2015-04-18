// Animator.cpp: implementation of the CAnimator class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Animator.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "TimeObject.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
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
    m_sFrameDelay(0),m_uiAnimateRemaining(0),deltat(0.0f)
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
	Global::GetInstance().getCurrentStatus().currentAnimator = Animator;
}

CAnimator *CAnimator::GetAnimator(void)
{
	return Global::GetInstance().getCurrentStatus().currentAnimator;
}


void CAnimator::animate(void)
{
	float dt = 0.0f;
	if (deltat > 0.0f)
		dt = CTimeObject::GetTime() - deltat;
	deltat = CTimeObject::GetTime();

	if (m_sCount == m_sFrameDelay)
	{
		const vector<CTimeObject*>& root = CTimeObject::getTimeObjects();
		//	Update animation objects
        m_uiAnimateRemaining = root.size(); 
		vector<CTimeObject*>::const_iterator itr = root.begin();
		while (itr != root.end())
		{
			CTimeObject *value = (*itr);
			itr++;
			value->synchronize(dt, true);

            m_uiAnimateRemaining--;
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
		CTimeObject *value = (*itr);
		itr++;

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
		const vector<CTimeObject*>& root = CTimeObject::getTimeObjects();
		vector<CTimeObject*>::const_iterator itr = root.begin();
		while (itr != root.end())
		{
			CTimeObject *value = (*itr);
			itr++;

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

CTextureGenerator * CAnimator::glStartPlayBack(const std::string& fname,bool loop) const
{
    CTextureGenerator *res = NULL;

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

