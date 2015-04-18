// EngineJob.h: interface for the CEngineJob class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINEJOB_H__12B4FC75_2A2F_42CD_8DB5_4A8E605532AE__INCLUDED_)
#define AFX_ENGINEJOB_H__12B4FC75_2A2F_42CD_8DB5_4A8E605532AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN


//! This class implements an asynchronous engine job element.
//! Internal classes ( 3DEngineTaskManager ) will process them
//! in a separate thread ( math pipe ) to keep more CPU power
//! to the rendering thread.
//! User must derive this class to implement the ComputeJob method
class RAPTOR_API CEngineJob  
{
public:
	//!	Engine job construction :
	//!	An id can be supplied to identified a batch
	//!	to which the job belongs. The default value of 0
	//!	means no batch.
	//!	Batch execution is controlled by the EngineTaskManager
	//!	batchJobs method.
	CEngineJob(unsigned int batchId = 0);
	virtual ~CEngineJob();

	//!	Job status
	//!	@return true is job has been computed
    bool isJobDone(void) const;

	//!	Returns the job's batch id.
	unsigned int getBatchId(void) const { return m_id; };

	//!	Reset job state to not computed.
	void resetJob(void);

    //! This method must be derived, it is the job itself.
    //! The transform parameter is given by the framework, it
    //! is the current modelview ( assuming you use Raptor Classes
    //! such as 3DEngine, which will handle the modelview )
    virtual bool computeJob(CGenericMatrix<float> &transform) = 0;

	//!	User interface : this method registers the job with the 
	//!	current EngineTaskManager.
	void registerJob();

	//!	User interface : this methods provokes the execution of
	//!	all engine jobs having the batchId passed as only parameter.
	static void run(unsigned int batchId);


protected:
	//!	Derived classes will position the appropriate job state
	void jobDone(void);


private:
	unsigned int	m_id;
    bool			jobComputed;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_ENGINEJOB_H__12B4FC75_2A2F_42CD_8DB5_4A8E605532AE__INCLUDED_)

