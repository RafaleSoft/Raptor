/***************************************************************************/
/*                                                                         */
/*  Win32EngineTaskManager.cpp                                             */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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

#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
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
	const CRaptorInstance &instance = manager->getInstance();

	//	I know this should mutex protected,
	//	but is it really important here ?
    manager->engineStarted = true;
	
	while (!manager->isStopRequested())
	{
		WaitForSingleObject(manager->processFrameEvt,INFINITE);
		ResetEvent(manager->processFrameEvt);
		if (manager->isStopRequested())
			break;

		ResetEvent(manager->synchroFrameEvt);
		
		if (instance.pAnimator != NULL )
		{
			instance.pAnimator->initSynchro();
			instance.pAnimator->animate();
		}

		SetEvent(manager->synchroFrameEvt);

		if (instance.pAnimator != NULL)
			instance.pAnimator->asyncAnimate();
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
CWin32EngineTaskManager::CWin32EngineTaskManager(const CRaptorInstance& instance)
	:C3DEngineTaskManager(instance)
{
    engineStarted = false;

#if defined(RAPTOR_SMP_CODE_GENERATION)
	stopRequested = false;
    engine = NULL;
	processFrameEvt = NULL;
	synchroFrameEvt = NULL;
#endif
}

CWin32EngineTaskManager::~CWin32EngineTaskManager()
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	stopEngine();
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

	stopRequested = false;
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
	const CRaptorInstance &instance = getInstance();

	if (instance.pAnimator != NULL )
	{
		instance.pAnimator->initSynchro();
		instance.pAnimator->animate();
		instance.pAnimator->asyncAnimate();
	}
#endif

    return true;
}


bool CWin32EngineTaskManager::stopEngine(void)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	stopRequested = true;
	if (processFrameEvt != NULL)
		SetEvent(processFrameEvt);

	cancelJobs();
	removeAllJobs();

	//	1 second should far enough to terminate thread...
	while ((NULL != engine) && (engineStarted))
		WaitForSingleObject(engine, 1000);
	if (NULL != engine)
	{
		CloseHandle(engine);
		engine = NULL;
	}

	for (unsigned int nb = 0; nb<asyncEngines.size(); nb++)
	{
		SetEvent(asyncEngines[nb].hEvent);
		while (asyncEngines[nb].started)
			WaitForSingleObject(asyncEngines[nb].handle, 1000);
		CloseHandle(asyncEngines[nb].handle);
		CloseHandle(asyncEngines[nb].hEvent);
	}

	asyncEngines.clear();

	if (processFrameEvt != NULL)
	{
		CloseHandle(processFrameEvt);
		processFrameEvt = NULL;
	}
	if (synchroFrameEvt != NULL)
	{
		CloseHandle(synchroFrameEvt);
		synchroFrameEvt = NULL;
	}
	stopRequested = false;
	return true;
#else
	return false;
#endif
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
	size_t stack = 0;
    for (size_t pos = 0; pos < asyncEngines.size(); pos++)
    {
		if (asyncEngines[pos].id == id)
            stack = pos;
    }

	//	I know this should mutex protected,
	//	but is it really important here ?
	asyncEngines[stack].started = true;

	while (!isStopRequested())
	{
		DWORD res = WaitForSingleObject(asyncEngines[stack].hEvent,INFINITE);
		if (isStopRequested())
			break;

		if (res == WAIT_FAILED)
		{
			res = GetLastError();
			RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),
						 "Engine Async Thread will terminate, failed to synchronize");
			break;
		}
		else
		{
			size_t sz = jobStackPool.size();
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

	switch(wait)
	{
		case WAIT_ABANDONED:
		{
			RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),
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
			RAPTOR_WARNING(CPersistence::CPersistenceClassID::GetClassId(),
                           "GLSynchronize core exited on timeout");
		}
		break;
	}
#endif

    return res;
}

