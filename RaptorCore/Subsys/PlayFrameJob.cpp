// PlayFrameJob.cpp: implementation of the CPlayFrameJob class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PLAYFRAMEJOB_H__0A9F1B5E_0BB5_4FA1_BFAB_4244514E6C75__INCLUDED_)
    #include "PlayFrameJob.h"
#endif
#if !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)
	#include "AnimatorStream.h"
#endif

RAPTOR_NAMESPACE

float CPlayFrameJob::jobTime = 0.0f;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayFrameJob::CPlayFrameJob(CAnimatorStream *stream,unsigned int nStream,unsigned int batchId)
    :CEngineJob(batchId),m_pStream(stream),m_nStream(nStream)
{

}

CPlayFrameJob::~CPlayFrameJob()
{

}

bool CPlayFrameJob::computeJob(CGenericMatrix<float> &transform)
{
    if (m_pStream != NULL)
        m_pStream->playFrame(jobTime,m_nStream,-1);

	jobDone();

    return true;
}

