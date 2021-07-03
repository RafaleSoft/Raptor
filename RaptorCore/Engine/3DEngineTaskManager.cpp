// 3DEngineTaskManager.cpp: implementation of the C3DEngineTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif

#if !defined(AFX_ENGINEJOB_H__12B4FC75_2A2F_42CD_8DB5_4A8E605532AE__INCLUDED_)
    #include "Engine/EngineJob.h"
#endif

#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif

#ifndef __3DENGINEATTRIBUTES_H__
    #include "Subsys/3DEngineAttributes.h"
#endif

#ifdef WIN32
    #if !defined(AFX_WIN32ENGINETASKMANAGER_H__8A94C7C2_7A88_4A75_94F4_4FE32FF31BA2__INCLUDED_)
        #include "Win32Specific/Win32EngineTaskManager.h"
    #endif
#endif

#ifdef LINUX
    #if !defined(AFX_GLXENGINETASKMANAGER_H__3BCAF192_565C_4D13_81FF_F63FFD989303__INCLUDED_)
        #include "GLXSpecific/GLXEngineTaskManager.h"
    #endif
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C3DEngineTaskManager *C3DEngineTaskManager::Create(void)
{
#if defined(WIN32)
    return new CWin32EngineTaskManager();
#elif defined(LINUX)
    return new CGLXEngineTaskManager();
#endif
}


C3DEngineTaskManager::C3DEngineTaskManager()
{
	batchMutex = new CRaptorMutex();
	stackMutex = new CRaptorMutex();

    //! At least one job stack must exist
    createJobStack();
}

C3DEngineTaskManager::~C3DEngineTaskManager()
{
	delete batchMutex;
	delete stackMutex;
}

void C3DEngineTaskManager::removeAllJobs(void)
{
	CRaptorLock lock(*stackMutex);

    jobStackPool.clear();
}

void C3DEngineTaskManager::createJobStack(void)
{
	CRaptorLock lock(*stackMutex);
	CRaptorLock lock2(*batchMutex);

    JOBSTACK js;
    jobStackPool.push_back(js);
}

size_t C3DEngineTaskManager::getNbRegisteredJobs(void) const
{
	CRaptorLock lock(*stackMutex);

    size_t nb = 0;
    for (size_t i=0; i < jobStackPool.size(); i++)
		nb += jobStackPool[i].jobStack.size();

    return nb;
}

bool C3DEngineTaskManager::registerJob(CEngineJob *job)
{
	CRaptorLock lock(*stackMutex);
	CRaptorLock lock2(*batchMutex);

    if (job == NULL)
        return false;

    if (jobStackPool.size() == 1)
		jobStackPool[0].jobStack.push_back(job);
    else
    {
        vector<JOBSTACK>::iterator it = jobStackPool.begin();
		size_t sz = (*it).jobStack.size();
        size_t sz2 = sz;
        while ((sz == sz2) && (++it != jobStackPool.end()))
            sz2 = (*it).jobStack.size();

        if (it == jobStackPool.end())
			jobStackPool[0].jobStack.push_back(job);
        else
			(*it).jobStack.push_back(job);
    }

    job->resetJob();

    return true;
}

bool C3DEngineTaskManager::unregisterJob(CEngineJob *job)
{
	CRaptorLock lock(*stackMutex);
	CRaptorLock lock2(*batchMutex);

	if (job == NULL)
        return false;

	vector<JOBSTACK>::iterator it = jobStackPool.begin();
	while (it != jobStackPool.end())
	{
		JOBSTACK& js = (*it++);
		vector<CEngineJob*>::iterator it2end = js.jobStack.end();
		vector<CEngineJob*>::iterator it2 = js.jobStack.begin();
		while (it2 != it2end)
		{
			if ((*it2) == job)
			{
				js.jobStack.erase(it2);
				return true;
			}
			it2++;
		}
	}

	return false;
}

void C3DEngineTaskManager::addJobBatch(unsigned int batchId)
{
	CRaptorLock lock(*batchMutex);

	vector<JOBSTACK>::iterator it = jobStackPool.begin();
	while (it != jobStackPool.end())
		(*it++).jobBatches.push(batchId);
}

void C3DEngineTaskManager::computeJobs(size_t stack)
{
    C3DEngine *engine = C3DEngine::Get3DEngine();
    if (engine == NULL)
        return;

    CGenericMatrix<float> modelview = engine->getModelview();

	CRaptorLock *lock = new CRaptorLock(*stackMutex);
    JOBSTACK &js = jobStackPool[stack];
	vector<CEngineJob*> jobStack = js.jobStack;
	delete lock;

    m_bCancelJobs = false;

	unsigned int batchId = 0;
	bool doNextBatch = false;

	do
	{
		{
			CRaptorLock batchLock(*batchMutex);
			if (js.jobBatches.size() > 0)
			{
				batchId = js.jobBatches.front();
				js.jobBatches.pop();
				doNextBatch = (js.jobBatches.size() > 0);
			}
		}

		//	A first loop to clear job status
		vector<CEngineJob*>::const_iterator it = jobStack.begin();
		while (it != jobStack.end())
		{
			CEngineJob *job = (*it++);
			if (batchId == job->getBatchId())
				job->resetJob();
		}

		//	A second loop to do the real job
		it = jobStack.begin();
		while (!m_bCancelJobs && (it != jobStack.end()))
		{
			CEngineJob *job = (*it++);
			if (batchId == job->getBatchId())
				job->computeJob(modelview);
		}
	}
	while (doNextBatch);
}

unsigned int C3DEngineTaskManager::getNbJobsToProceed(void) const
{
	CRaptorLock lock(*stackMutex);

    unsigned int remaining = 0;

    vector<JOBSTACK>::const_iterator it = jobStackPool.begin();
    while (it != jobStackPool.end())
    {
        const JOBSTACK &js = (*it++);
        size_t stack_size = js.jobStack.size();
    
        for (size_t job_index=0; job_index < stack_size; job_index++)
            remaining += ( js.jobStack.at(job_index)->isJobDone() ? 0 : 1 );
    }

    return remaining;
}

