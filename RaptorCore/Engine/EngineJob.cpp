// EngineJob.cpp: implementation of the CEngineJob class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_ENGINEJOB_H__12B4FC75_2A2F_42CD_8DB5_4A8E605532AE__INCLUDED_)
    #include "EngineJob.h"
#endif

#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif

#ifndef __GLOBAL_H__
    #include "System/Global.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineJob::CEngineJob(unsigned int batchId)
    :jobComputed(false),m_id(batchId)
{

}

CEngineJob::~CEngineJob()
{
	// Remove the job in case it has been registered.
	Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    C3DEngineTaskManager *taskManager = st.engineTaskMgr;

	taskManager->unregisterJob(this);
}

void CEngineJob::resetJob(void)
{
	jobComputed = false;
}

bool CEngineJob::isJobDone(void) const
{ 
	return jobComputed; 
}

void CEngineJob::jobDone(void)
{
	jobComputed = true;
}


void CEngineJob::registerJob()
{
	Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    C3DEngineTaskManager *taskManager = st.engineTaskMgr;

	taskManager->registerJob(this);
}

void CEngineJob::run(unsigned int batchId)
{
	Global::RAPTOR_CURRENT_STATUS &st = Global::GetInstance().getCurrentStatus();
    C3DEngineTaskManager *taskManager = st.engineTaskMgr;

	taskManager->batchJobs(batchId);
}


