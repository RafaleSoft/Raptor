// Win32EngineTaskManager.cpp: implementation of the CWin32EngineTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#ifndef __GLOBAL_H__
    #include "System/Global.h"
#endif

#if !defined(AFX_WIN32ENGINETASKMANAGER_H__8A94C7C2_7A88_4A75_94F4_4FE32FF31BA2__INCLUDED_)
    #include "Win32EngineTaskManager.h"
#endif

#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Engine/Animator.h"
#endif

#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#ifdef RAPTOR_SSE_CODE_GENERATION
	#if !defined(AFX_SSE_3DENGINE_H__247539DD_1572_4C73_86BB_CD4C1DD82614__INCLUDED_)
		#include "SSE_Engine/SSE_3DEngine.h"
	#endif
#endif


RAPTOR_NAMESPACE_BEGIN

unsigned int	CWin32EngineTaskManager::BATCH_ID = 1;


/////////////////////////////////////////////////////////////////////////////
//	engine thread + dll
//
//	T H E  C O R E
//
#if defined(RAPTOR_SMP_CODE_GENERATION)
DWORD WINAPI engineSyncThread(void* pParam)
{
    CWin32EngineTaskManager* manager = (CWin32EngineTaskManager*)pParam;
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();

	//	I know this should mutex protected,
	//	but is it really important here ?
    manager->engineStarted = true;
	while (!status.terminate)
	{
		WaitForSingleObject(manager->processFrameEvt,INFINITE);
		ResetEvent(manager->processFrameEvt);
		if (status.terminate)
			break;

		ResetEvent(manager->synchroFrameEvt);
		
		if (status.currentAnimator != NULL )
		{
			status.currentAnimator->initSynchro();
			status.currentAnimator->animate();
		}

		SetEvent(manager->synchroFrameEvt);

		if (status.currentAnimator != NULL )
			status.currentAnimator->asyncAnimate();
	}
    manager->engineStarted = false;
	ExitThread( 0 );
	return 0;
}

DWORD WINAPI engineAsyncThread(void* pParam)
{
    CWin32EngineTaskManager* manager = (CWin32EngineTaskManager*)pParam;

    // find stack number if possible:
	DWORD id = GetCurrentThreadId();
	manager->computeAsyncJobs(id);

	ExitThread( 0 );
	return 0;
}
#endif

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWin32EngineTaskManager::CWin32EngineTaskManager()
{
    engineStarted = false;

#if defined(RAPTOR_SMP_CODE_GENERATION)
    engine = NULL;
	processFrameEvt = NULL;
	synchroFrameEvt = NULL;
#endif
}

CWin32EngineTaskManager::~CWin32EngineTaskManager()
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	if (processFrameEvt != NULL)
		SetEvent(processFrameEvt);

    cancelJobs();
    removeAllJobs();

	//	1 second should far enough to terminate thread...
    if ((0 != engine) && (engineStarted))
	{
		WaitForSingleObject(engine,1000);
		CloseHandle(engine);
	}
    for (unsigned int nb = 0; nb<asyncEngines.size();nb++)
	{
		SetEvent(asyncEngines[nb].hEvent);
        if (asyncEngines[nb].started)
			WaitForSingleObject(asyncEngines[nb].handle,1000);
		CloseHandle(asyncEngines[nb].handle);
		CloseHandle(asyncEngines[nb].hEvent);
	}

	if (processFrameEvt != NULL)
		CloseHandle(processFrameEvt);
	if (synchroFrameEvt != NULL)
		CloseHandle(synchroFrameEvt);
#endif
}

unsigned int CWin32EngineTaskManager::generateBatchId(void)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	CRaptorLock lock(mutex);
#endif

	return BATCH_ID++;
}


bool CWin32EngineTaskManager::initEngine(void)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	synchroFrameEvt = CreateEvent(NULL,TRUE,TRUE,"SYNCHREVT");
	processFrameEvt = CreateEvent(NULL,TRUE,FALSE,"THREADEVT");

	engine = CreateThread( NULL, 0, engineSyncThread, this , CREATE_SUSPENDED, NULL );
	if (0 == engine)
		return false;

	SetThreadPriority(engine,THREAD_PRIORITY_ABOVE_NORMAL);
	ResumeThread(engine);

    const CPU_INFO &info = getCPUINFO();
	unsigned int nbProcs = info.numCores;

	ENGINE_STATE state;
	state.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	state.started = false;
	state.handle = CreateThread(NULL, 
								0, 
								engineAsyncThread, 
								this , 
								CREATE_SUSPENDED, 
								&state.id );
	if (0 == state.handle)
		return false;
	asyncEngines.push_back(state);

	SetThreadPriority(state.handle,THREAD_PRIORITY_ABOVE_NORMAL);
	ResumeThread(state.handle);

    if (nbProcs > 2)
    {
		for (unsigned int nbStack = 2; nbStack < nbProcs; nbStack++)
		{
			createJobStack();

			state.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
			state.started = false;
			state.handle = CreateThread(NULL, 
										0, 
										engineAsyncThread, 
										this , 
										CREATE_SUSPENDED, 
										&state.id );
			if (0 == state.handle)
				return false;
			asyncEngines.push_back(state);

			SetThreadPriority(state.handle,THREAD_PRIORITY_ABOVE_NORMAL);
			ResumeThread(state.handle);
		}
    }
#endif

    return true;
}


bool CWin32EngineTaskManager::run(void)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	//	Animation & engine computations are done in 
	//	the core thread, so that SMP can be used.
	//	Besides, some computations can be done
	//	while waiting for the frame rate, or
	//	while waiting for the GPU to be available
	//
	//	The prioritized object has to be rendered first
	//PulseEvent(processFrameEvt);
	SetEvent(processFrameEvt);
#else
	Global::RAPTOR_CURRENT_STATUS& status = Global::get()->getCurrentStatus();

	if (status.currentAnimator != NULL )
	{
		status.currentAnimator->animate();
		status.currentAnimator->initSynchro();
	}

    if (status.currentAnimator != NULL )
		status.currentAnimator->asyncAnimate();
#endif

    return true;
}

bool CWin32EngineTaskManager::batchJobs(unsigned int batchId)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	addJobBatch(batchId);

	vector<ENGINE_STATE>::const_iterator it = asyncEngines.begin();
	while (it != asyncEngines.end())
		SetEvent((*it++).hEvent);
#else
    computeJobs(0);
#endif

    return true;
}


void CWin32EngineTaskManager::computeAsyncJobs(DWORD id)
{
	unsigned int stack = 0;
    for (unsigned int pos = 0; pos < asyncEngines.size(); pos++)
    {
		if (asyncEngines[pos].id == id)
            stack = pos;
    }

	//	I know this should mutex protected,
	//	but is it really important here ?
	asyncEngines[stack].started = true;

	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
	while (!status.terminate)
	{
		DWORD res = WaitForSingleObject(asyncEngines[stack].hEvent,INFINITE);
		if (status.terminate)
			break;

		if (res == WAIT_FAILED)
		{
			res = GetLastError();
			status.errorMgr->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_FATAL,
													"Engine Async Thread will terminate, failed to synchronize");
			break;
		}
		else
		{
			unsigned int sz = jobStackPool.size();
			if (stack < sz)
				computeJobs(stack);
		}
    }
	asyncEngines[stack].started = false;
}


bool CWin32EngineTaskManager::synchronizeCore(unsigned long timeLimit)
{
    bool res = false;

#if defined(RAPTOR_SMP_CODE_GENERATION)
    //  
    //  First step : synchronize on frame time objects
    //
	unsigned long wait = WaitForSingleObject(synchroFrameEvt,timeLimit);
    Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();

	switch(wait)
	{
		case WAIT_ABANDONED:
		{
	        
			st.errorMgr->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
											 CRaptorErrorManager::RAPTOR_FATAL,
											 "GLSynchronize core exited on Core Termination");
			break;
		}
		
		case WAIT_OBJECT_0:
		{
			//	The event must remain signaled to
			//	ensure that 'redisplays' will not
			//	stall
			SetEvent(synchroFrameEvt);
			res = true;
			break;
		}
		case WAIT_TIMEOUT:
		{
			st.errorMgr->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
                                             CRaptorErrorManager::RAPTOR_WARNING,
                                             "GLSynchronize core exited on timeout");
		}
		break;
	}
#endif

    return res;
}

