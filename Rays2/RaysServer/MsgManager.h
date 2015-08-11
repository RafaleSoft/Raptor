// MsgManager.h: interface for the CMsgManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGMANAGER_H__72E4E9A9_6CAF_419F_B381_BB70F1462945__INCLUDED_)
#define AFX_MSGMANAGER_H__72E4E9A9_6CAF_419F_B381_BB70F1462945__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMsgManager
{
public:
	enum 
	{ 
		NB_ATTEMPTS = 999, 
		TIMEOUT = 1000 
	};

public:

	CMsgManager(CRaysServerApp *owner);
	virtual ~CMsgManager();

	void Run();
	void ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[]);

	CDeamonManager::LPMSGREGSTRUCT getRegMsg(void) const { return regMsg; };

private:
	friend UINT MsgProcessor( LPVOID pParam );

	CPtrArray		Pool;
	CMutex			*msgMutex;
	CSingleLock		*lock;
	
	CEvent			*msgEvent;

	CRaysServerApp	*owner_server;

	CWinThread		*msgProcessor;
	CDeamonManager::LPMSGREGSTRUCT regMsg;


};

#endif // !defined(AFX_MSGMANAGER_H__72E4E9A9_6CAF_419F_B381_BB70F1462945__INCLUDED_)
