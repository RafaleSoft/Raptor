// ShadowVolumeJob.h: interface for the CShadowVolumeJob class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWVOLUMEJOB_H__3C3EB9C3_88DF_4C32_B66E_5265B65D8648__INCLUDED_)
#define AFX_SHADOWVOLUMEJOB_H__3C3EB9C3_88DF_4C32_B66E_5265B65D8648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ENGINEJOB_H__12B4FC75_2A2F_42CD_8DB5_4A8E605532AE__INCLUDED_)
    #include "Engine/EngineJob.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CObject3DShadow;


//! This class defines an engine job to compute shadows in the engine pipeline 
//! rather than in the graphic pipeline
class CShadowVolumeJob : public CEngineJob  
{
public:
    //! Constructor : the owner shadow is passed in
	CShadowVolumeJob(CObject3DShadow* owner,unsigned int batchId);
	virtual ~CShadowVolumeJob();

    //! The handler derived from CEngineJob
    virtual bool computeJob(CGenericMatrix<float> &transform);

private:
    CObject3DShadow *shadow;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADOWVOLUMEJOB_H__3C3EB9C3_88DF_4C32_B66E_5265B65D8648__INCLUDED_)

