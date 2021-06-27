// PlayFrameJob.h: interface for the CPlayFrameJob class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYFRAMEJOB_H__0A9F1B5E_0BB5_4FA1_BFAB_4244514E6C75__INCLUDED_)
#define AFX_PLAYFRAMEJOB_H__0A9F1B5E_0BB5_4FA1_BFAB_4244514E6C75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ENGINEJOB_H__12B4FC75_2A2F_42CD_8DB5_4A8E605532AE__INCLUDED_)
    #include "Engine/EngineJob.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CAnimatorStream;

//! This class defines an engine job to read video frames in the engine pipeline 
//! rather than in the graphic pipeline
class CPlayFrameJob : public CEngineJob  
{
public:
	CPlayFrameJob(CAnimatorStream *stream,unsigned int nStream,unsigned int batchId);
	virtual ~CPlayFrameJob();

    //! The handler derived from CEngineJob
    virtual bool computeJob(CGenericMatrix<float> &transform);

    //! Sets the current frame delta time (in seconds) issued from animator
    //! This delta time will be used to play the next frame.
    static void SetJobTime(float dt) { jobTime = dt; };


private:
    CAnimatorStream *m_pStream;
    unsigned int    m_nStream;

    static float jobTime;
};

RAPTOR_NAMESPACE_END


#endif // !defined(AFX_PLAYFRAMEJOB_H__0A9F1B5E_0BB5_4FA1_BFAB_4244514E6C75__INCLUDED_)
