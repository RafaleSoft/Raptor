// GLXEngineTaskManager.cpp: implementation of the CGLXEngineTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"


#if !defined(AFX_GLXENGINETASKMANAGER_H__3BCAF192_565C_4D13_81FF_F63FFD989303__INCLUDED_)
    #include "GLXEngineTaskManager.h"
#endif
#if !defined(AFX_ANIMATOR_H__077150E3_D826_11D3_9142_9866F8B4457F__INCLUDED_)
	#include "Engine/Animator.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

#include <iostream>
#include <pthread.h>
#include <errno.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RAPTOR_NAMESPACE_BEGIN

unsigned int	CGLXEngineTaskManager::BATCH_ID = 1;

/////////////////////////////////////////////////////////////////////////////
//	engine thread + dll
//
//	T H E  C O R E
//
#if defined(RAPTOR_SMP_CODE_GENERATION)
void* engineSyncThread(void* pParam)
{
	CGLXEngineTaskManager* manager = (CGLXEngineTaskManager*)pParam;
    CRaptorInstance& instance = CRaptorInstance::GetInstance();

    pthread_mutex_t frameLock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&frameLock);

    //	I know this should mutex protected,
	//	but is it really important here ?
	while (!instance.terminate)
	{
        pthread_cond_wait(&manager->frameEvent,&frameLock);

		if (instance.pAnimator != NULL )
		{
			instance.pAnimator->initSynchro();
			instance.pAnimator->animate();
		}

        pthread_cond_broadcast(&manager->synchroEvent);

		if (instance.pAnimator != NULL )
		{
			instance.pAnimator->asyncAnimate();
			//status.pAnimator->frameFinished = true;
		}
	}
    
    pthread_exit( (void*)0 );
	return (void*)0;
}


void* engineAsyncThread(void* pParam)
{
    CGLXEngineTaskManager* manager = (CGLXEngineTaskManager*)pParam;
	CRaptorInstance& instance = CRaptorInstance::GetInstance();

	pthread_mutex_t frameLock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&frameLock);

    while (!instance.terminate)
	{
        pthread_cond_wait(&manager->jobsEvent,&frameLock);

        manager->computeJobs();
    }

	pthread_exit( (void*)0 );
	return (void*)0;
}
#endif

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE


CGLXEngineTaskManager::CGLXEngineTaskManager()
    :syncJobs(0),asyncJobs(0)
{

}

CGLXEngineTaskManager::~CGLXEngineTaskManager()
{

}

unsigned int CGLXEngineTaskManager::generateBatchId(void)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	CRaptorLock lock(mutex);
#endif

	return BATCH_ID++;
}

bool CGLXEngineTaskManager::initEngine(void)
{
    bool res = true;

#if defined(RAPTOR_SMP_CODE_GENERATION)
    cout << "Init engine." << endl;

    res = (0 == pthread_create(&syncJobs, NULL, engineSyncThread , this));
    res = res && (0 ==  pthread_create(&asyncJobs, NULL, engineAsyncThread , this));

    res = res && (0 == pthread_cond_init(&jobsEvent,NULL));
    res = res && (0 == pthread_cond_init(&frameEvent,NULL));
    res = res && (0 == pthread_cond_init(&synchroEvent,NULL));

#endif

    return res;
}

bool CGLXEngineTaskManager::run(void)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
	//	Animation & engine computations are done in 
	//	the core thread, so that SMP can be used.
	//	Besides, some computations can be done
	//	while waiting for the frame rate, or
	//	while waiting for the GPU to be available
	//
	//	The prioritized object has to be rendered first
	pthread_cond_broadcast(&frameEvent);
#else
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance()->getCurrentStatus();

	if (status.currentAnimator != NULL )
		status.currentAnimator->animate();

    if (status.currentAnimator != NULL )
		status.currentAnimator->asyncAnimate();
#endif

    return true;
}

bool CGLXEngineTaskManager::batchJobs(unsigned int batchId)
{
#if defined(RAPTOR_SMP_CODE_GENERATION)
    pthread_cond_broadcast(&jobsEvent);
#else
    computeJobs();
#endif

    return true;
}

bool CGLXEngineTaskManager::synchronizeCore(unsigned long timeLimit)
{
    bool res = false;

#if defined(RAPTOR_SMP_CODE_GENERATION)
    CRaptorInstance& instance = CRaptorInstance::GetInstance();

    pthread_mutex_t synchro = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&synchro);

    struct timespec timeout;
    timeout.tv_sec = 0;
    timeout.tv_nsec = 1000000 * timeLimit;

    int wait = pthread_cond_timedwait(&synchroEvent,  &synchro, &timeout);

    switch(wait)
    {
        case ETIMEDOUT:
            instance.pErrorMgr->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_WARNING,
													"GLSynchronize core exited on timeout");
            break;
        case EINTR:
            instance.pErrorMgr->generateRaptorError(CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_FATAL,
													"GLSynchronize core exited on Interrupt");
            break;
        default:
            res = true;
    }
#endif

    return res;
}
