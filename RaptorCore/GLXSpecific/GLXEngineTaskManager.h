// GLXEngineTaskManager.h: interface for the CGLXEngineTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLXENGINETASKMANAGER_H__3BCAF192_565C_4D13_81FF_F63FFD989303__INCLUDED_)
#define AFX_GLXENGINETASKMANAGER_H__3BCAF192_565C_4D13_81FF_F63FFD989303__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_THREADMUTEX_H__E12D440F_070C_4B70_B424_AFAF2B1B458D__INCLUDED_)
    #include "Subsys/ThreadMutex.h"
#endif

#include <pthread.h>

RAPTOR_NAMESPACE_BEGIN

class CGLXEngineTaskManager   : public C3DEngineTaskManager  
{
public:
	CGLXEngineTaskManager();
	virtual ~CGLXEngineTaskManager();

    virtual bool initEngine(void);
    virtual bool run(void);
	virtual unsigned int generateBatchId(void);
	virtual bool batchJobs(unsigned int batchId = 0);
    virtual bool synchronizeCore(unsigned long timeLimit = ~0);
    
private:
    pthread_t  syncJobs;
    pthread_t  asyncJobs;

    pthread_cond_t synchroEvent;
    pthread_cond_t frameEvent;
    pthread_cond_t jobsEvent;

	CThreadMutex     mutex;
	static unsigned int	BATCH_ID;

    friend void* engineSyncThread(void* pParam);
    friend void* engineAsyncThread(void* pParam);

};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_GLXENGINETASKMANAGER_H__3BCAF192_565C_4D13_81FF_F63FFD989303__INCLUDED_)
