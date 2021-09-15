// 3DEngineTaskManager.h: interface for the C3DEngineTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
#define AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <queue>

RAPTOR_NAMESPACE_BEGIN

class CEngineJob;
class CRaptorMutex;
class CRaptorInstance;


class C3DEngineTaskManager  
{
public:
    //! Factory model, engine tasks depend on OS capabilities
	static C3DEngineTaskManager *Create(const CRaptorInstance& instance);

	virtual ~C3DEngineTaskManager();

    //! This method initialises the job stak and the pump threads
    //! for the math engine pipe.
    virtual bool initEngine(void) = 0;

    //! This method renders synchronous tasks for the current frame
    //! Such as animator tasks, timed objects updates, ...
    //! It must be called once per frame before any rendering.
    virtual bool run(void) = 0;

	//! This method waits for active engine tasks to terminate
	//!	and then release all used resources.
	//! In order to run the engine again, a new call to initEngine 
	//!	must be called.
	virtual bool stopEngine(void) = 0;

	//!	This method generates a unique batch Id for user batch jobs.
	virtual unsigned int generateBatchId(void) = 0;

    //! This method triggers all asynchronous registered jobs.
    //! It can be called as many times as needed to computed
    //! a batch of enginejobs.
	//! If batchId is 0, all jobs are processed. Otherwise, 
	//! only jobs which have the requested batchId are processed
    virtual bool batchJobs(unsigned int batchId = 0) = 0;

    //!	Multiprocessor synchronisation function for
	//!	core engine & rendering pipelines syncronization.
	//!	Beware, it is a blocking call ( for timeLimit milliseconds )
    virtual bool synchronizeCore(unsigned long timeLimit = ~0) = 0;



    //!
    //! Engine pipe processing
    //!

    //! Create a new job stack
    void createJobStack(void);

    //! Push a job into the job stack. The job will
    //! processed within the next call to ComputeJobs
    bool registerJob(CEngineJob *job);

	//! Push a job into the job stack. The job will
    //! processed within the next call to ComputeJobs
    bool unregisterJob(CEngineJob *job);

    //! Returns the number of jobs registered using the method here above.
    size_t getNbRegisteredJobs(void) const;

    //! Returns the numbre of registered jobs that remain to proceed for the
    //! current call to ProcessJobs.
    unsigned int getNbJobsToProceed(void) const;

    //! Abort the current stack queue processing in ProcessJobs.
    //! The current job is completed before ProcessJobs returns.
    void cancelJobs(void) { m_bCancelJobs = true; };

    //! Remove all registered jobs. Jobs are not deleted.
    void removeAllJobs(void);


protected:
    //!
    //! Async job processing. All jobs in the stack are processed if
	//! batchId is -1. Otherwise, only jobs which have the requested
	//!	batchId are processed
    void computeJobs(size_t stack = 0);

	//!	Safely adds a batch to all job stacks batches.
	void addJobBatch(unsigned int batchId);

	//!	Constructor for subclasses.
	C3DEngineTaskManager(const CRaptorInstance& instance);

	//! The Raptor instance ,that this engine will work with.
	const CRaptorInstance& m_instance;

    typedef struct JOBSTACK_t
	{
		queue<unsigned int> jobBatches;
		vector<CEngineJob*> jobStack;
	} JOBSTACK;
    vector<JOBSTACK> jobStackPool;

	CRaptorMutex     *batchMutex;
	CRaptorMutex     *stackMutex;

    bool m_bCancelJobs;

private:
	C3DEngineTaskManager();
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)

