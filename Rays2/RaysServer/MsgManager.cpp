// MsgManager.cpp: implementation of the CMsgManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysServer.h"
#include "MsgManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



UINT MsgProcessor( LPVOID pParam )
{
	CMsgManager *manager = (CMsgManager *)pParam;
	CMutex *msgMutex = new CMutex(FALSE,"ManagerMsgLock",NULL);
	CEvent *msgEvent = new CEvent(FALSE,TRUE,"MessageEvent",NULL);
	CSingleLock	*lock = new CSingleLock(msgMutex);
	CString msg;

	CEvent *msgProcessorEvent = new CEvent(FALSE,TRUE,"ThreadEvent",NULL);
	CSingleLock *serverLock = new CSingleLock(msgProcessorEvent);

	while (1)
	{
		//	wait for an incoming message in the pool
		msg.Format("waiting for msg lock...(pool size:%d)",manager->Pool.GetSize());
		manager->owner_server->WriteMessage(IDS_CORE_STRING,msg);
		msgEvent->Lock();

		BOOL res = FALSE;
		int nbAttempts = 0;
		//	attempt processing before forcing msgprocessor
		while ((res == FALSE) && (nbAttempts < CMsgManager::NB_ATTEMPTS))
		{
			//	wait for server readiness
			msg.Format("waiting for server lock...(pool size:%d)",manager->Pool.GetSize());
			manager->owner_server->WriteMessage(IDS_CORE_STRING,msg);

			res = serverLock->Lock(CMsgManager::TIMEOUT);

			if (!manager->owner_server->MsgProcessorAvailable())
			{
				msg.Format("message processor unavailable... attempt %d",nbAttempts);
				manager->owner_server->WriteMessage(IDS_CORE_STRING,msg);
			}
			nbAttempts++;
		}

		if (res == FALSE)
		{
			msg = "Forcing message processor !";
			manager->owner_server->WriteMessage(IDS_CORE_STRING,msg);
			//manager->owner_server->MsgProcessorAvailable(true);
		}

		//	pick a message
		lock->Lock();
		manager->regMsg = (CDeamonManager::LPMSGREGSTRUCT)(manager->Pool[0]);
		manager->Pool.RemoveAt(0);
		lock->Unlock();

		//	process the message
		msgProcessorEvent->ResetEvent();
		serverLock->Unlock();

		BOOL post = 
			manager->owner_server->PostThreadMessage( regMsgID , 0, (LPARAM)(&manager->regMsg));


		//	check if new messages are pending
		lock->Lock();
		if (manager->Pool.GetSize() == 0)
			msgEvent->ResetEvent();
		lock->Unlock();

		if (post == FALSE)
		{
			msg = "failed to notify server...";
			manager->owner_server->WriteMessage(IDS_CORE_STRING,msg);
		}
	}

	delete msgMutex;
	delete msgEvent;
	delete lock;
	delete msgProcessorEvent;

	return 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgManager::CMsgManager(CRaysServerApp *owner):owner_server(owner)
{
	//	initialise locks
	msgMutex = new CMutex(FALSE,"ManagerMsgLock",NULL);
	lock = new CSingleLock(msgMutex,TRUE);

	//	set a manual event for messages processing
	msgEvent = new CEvent(FALSE,TRUE,"MessageEvent",NULL);

	lock->Unlock();


	msgProcessor = AfxBeginThread(	MsgProcessor, 
									(void*)this, 
									THREAD_PRIORITY_NORMAL, 
									0, 
									CREATE_SUSPENDED, 
									NULL );
}

CMsgManager::~CMsgManager()
{
	delete msgEvent;
	delete msgMutex;
	delete lock;
}

void CMsgManager::ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[])
{
	lock->Lock();

	CDeamonManager::LPMSGREGSTRUCT rMsg = new CDeamonManager::MSGREGSTRUCT;
	rMsg->msg = msg;
	rMsg->raw_data = raw_data;
	Pool.Add(rMsg);

	CString str = "Incoming request from client...";
	owner_server->WriteMessage(IDS_CORE_STRING,str);

	msgEvent->SetEvent();

	lock->Unlock();
}

void CMsgManager::Run()
{
	msgProcessor->ResumeThread();
}