// Win32EngineTaskManager.h: interface for the CWin32EngineTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIN32ENGINETASKMANAGER_H__8A94C7C2_7A88_4A75_94F4_4FE32FF31BA2__INCLUDED_)
#define AFX_WIN32ENGINETASKMANAGER_H__8A94C7C2_7A88_4A75_94F4_4FE32FF31BA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CWin32EngineTaskManager : public C3DEngineTaskManager  
{
public:
	CWin32EngineTaskManager();
	virtual ~CWin32EngineTaskManager();

    virtual bool initEngine(void);
    virtual bool run(void);
	virtual unsigned int generateBatchId(void);
    virtual bool batchJobs(unsigned int batchId = 0);
    virtual bool synchronizeCore(unsigned long timeLimit = ~0);

#if defined(RAPTOR_SMP_CODE_GENERATION)
    //!	Called by each async engines to process its own jobs stack
	void computeAsyncJobs(DWORD id);


	//! The engine threads to process off-rendering tasks.
    //! A boolean value ( though it requires a Mutex ) helps to determine
    //! thread status to avoid waiting if it has excited.
    bool	engineStarted;

	//! An event for the sync engine awakening
	HANDLE			processFrameEvt;
	
	//!	An event object to unlock the synchronizeCore method.
	HANDLE			synchroFrameEvt;


private:
	HANDLE	engine;

	typedef struct ENGINE_STATE_t
	{
		HANDLE	hEvent;
		HANDLE	handle;
		DWORD	id;
		bool	started;
	} ENGINE_STATE;
	vector<ENGINE_STATE>	asyncEngines;

	CRaptorMutex     mutex;
#endif

	static unsigned int	BATCH_ID;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_WIN32ENGINETASKMANAGER_H__8A94C7C2_7A88_4A75_94F4_4FE32FF31BA2__INCLUDED_)

