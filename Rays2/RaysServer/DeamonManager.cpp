// DeamonManager.cpp: implementation of the CDeamonManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysServer.h"
#include "DeamonManager.h"
#include "DeamonSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


UINT DeamonProcessor( LPVOID pParam )
{
	CDeamonManager *deamon = (CDeamonManager*)pParam;

	while (1)
	{
		int pos=0,nb=0;
		int max = deamon->m_RegWorkUnits.GetSize();

		//	Scan each work unit
		while (pos<max)
		{
			const CDeamonManager::WUREGSTRUCT * const lpRegWU = 
				deamon->GetRegWorkUnit(pos);

			if (lpRegWU->connection == NULL)
			{
				CClient<CDeamonSocket> client;
				if (client.ConnectToServer(lpRegWU->deamonIP,PORTBASE+1))
				{
					BOOL post = 
					deamon->m_ownerServer->PostThreadMessage( regDmnMsgID , 0, (LPARAM)(pos));
				}
			}

			pos++;
		}

		Sleep(deamon->m_ownerServer->m_deamonDelay * 1000);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeamonManager::CDeamonManager(CRaysServerApp *server)
{
	m_counter = 0;
	m_ownerServer = server;

	m_deamonPoller = AfxBeginThread(DeamonProcessor, 
									(void*)this, 
									THREAD_PRIORITY_NORMAL, 
									0, 
									CREATE_SUSPENDED, 
									NULL );
}

CDeamonManager::~CDeamonManager()
{
	//	Clear Registered workunits
	int nbRWU = m_RegWorkUnits.GetSize();
	while (nbRWU > 0)
	{
		DestroyRegWorkUnit(0);
		nbRWU--;
	}

	if (nbRWU > 0)
	{
		AfxMessageBox("Some workunits were still working and could not be destroyed !");
	}

	// Clear workunits
	POSITION pos = m_WorkUnits.GetStartPosition();
	while (pos != NULL)
	{
		unsigned short WUID;
		CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;

		m_WorkUnits.GetNextAssoc(pos,WUID,(void*&)lpWU);
		if (lpWU != NULL)
		{
			m_WorkUnits.RemoveKey(WUID);
			delete lpWU;
		}
	}

	m_WorkUnits.RemoveAll();
}

bool CDeamonManager::DestroyRegWorkUnit(unsigned int numRegWU)
{
	if (numRegWU >= (unsigned int)m_RegWorkUnits.GetSize())
		return false;

	LPWUREGSTRUCT lpRegWU = (LPWUREGSTRUCT )m_RegWorkUnits.GetAt(numRegWU);
	if (lpRegWU->available != lpRegWU->nbProcs)
		return false;

	m_RegWorkUnits.RemoveAt(numRegWU);

	if (lpRegWU->connection != NULL)
	{
		MSGSTRUCT			msg;

		msg.msg_header = MSG_START;
		msg.msg_id = DMN_INACTIVE;
		msg.msg_size = 0;
		msg.msg_tail = MSG_END;

		CClient<CDeamonSocket> *client = (CClient<CDeamonSocket>*)(lpRegWU->connection);
		client->Write(&msg,MSGSIZE);
		delete client;
	}

	delete lpRegWU;
	return true;
}

const CDeamonManager::WUREGSTRUCT * const CDeamonManager::CreateRegWorkUnit(CString wu,CString deamon)
{
	CDeamonManager::WUREGSTRUCT *lpRegWU = NULL;

	int pos = 0;
	while (pos<m_RegWorkUnits.GetSize())
	{
		lpRegWU = (LPWUREGSTRUCT)m_RegWorkUnits.GetAt(pos++);

		if ((lpRegWU->source == wu) && (lpRegWU->deamonIP == deamon))
		{
			lpRegWU->nbProcs++;
			lpRegWU->available++;
			break;
		}
	}

	if (pos >= m_RegWorkUnits.GetSize())
	{
		lpRegWU = new WUREGSTRUCT;
		lpRegWU->source = wu;
		lpRegWU->deamonIP = deamon;
		lpRegWU->nbProcs = 1;
		lpRegWU->available = 1;
		lpRegWU->connection = NULL;
		lpRegWU->active = false;
		m_RegWorkUnits.Add(lpRegWU);
	}

	return lpRegWU;
}


void CDeamonManager::Run()
{
	m_deamonPoller->ResumeThread();
}

bool CDeamonManager::InstallPlugin(unsigned int IP,unsigned int port,unsigned int validDate,
				   CString name,unsigned int size,unsigned char* plugin) const
{
	// First, find deamon managing this workunit
	bool found = false;
	unsigned short WURegID = 0;
	POSITION pos = m_WorkUnits.GetStartPosition();

	while ((pos != NULL) && (!found))
	{
		LPWORKUNITSTRUCT pWu = NULL;	
		m_WorkUnits.GetNextAssoc(pos,WURegID,(void*&)pWu);

		iosock_t<CSSocket> *pConnection = (iosock_t<CSSocket> *)(pWu->connection);
		found =  ((pConnection->Port() == port) && (pConnection->IP() == IP));
		WURegID = pWu->regID;
	}

	if (found)
	{
		LPWUREGSTRUCT lpWUReg = (LPWUREGSTRUCT)m_RegWorkUnits.GetAt(WURegID);

		CClient<CDeamonSocket>* deamon = (CClient<CDeamonSocket>*)(lpWUReg->connection);
			
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = OBJ_PLUGIN;
		msg.msg_size = size+name.GetLength()+1;
		msg.msg_tail = MSG_DATA;
		msg.msg_data[0] = 0;
		msg.msg_data[1] = validDate;

		deamon->Write(&msg,MSGSIZE);
		deamon->Write((void*)LPCTSTR(name),name.GetLength()+1);
		deamon->Write(plugin,size);

		return true;
	}
	else
		return false;
}

void CDeamonManager::RegisterDeamon(int numDeamon)
{
	MSGSTRUCT	msg;
	msg.msg_header = MSG_START;
	msg.msg_id = DMN_ACTIVE;
	msg.msg_size = 0;
	msg.msg_tail = MSG_END;
	msg.msg_data[0] = 0;
	msg.msg_data[1] = 0;
	msg.msg_data[2] = 0;
	msg.msg_data[3] = 0;
	msg.msg_data[4] = 0;

	CClient<CDeamonSocket> *client;
	CDeamonManager::LPWUREGSTRUCT lpRegWU = (CDeamonManager::WUREGSTRUCT *)m_RegWorkUnits.GetAt(numDeamon);

	//	Turn work unit deamon inactive.
	//	If a response is received in return,
	//	then it is made active form the time 
	//	delay specified in options
	lpRegWU->active = false;

	if (lpRegWU->connection == NULL)
	{
		client = new CClient<CDeamonSocket>;
		if (client->ConnectToServer(lpRegWU->deamonIP,PORTBASE+1))
		{
			lpRegWU->connection = client;
			client->Write(&msg,MSGSIZE);
		}
		else
			delete client;
	}
	else
	{
		client = (CClient<CDeamonSocket>*)(lpRegWU->connection);
		client->Write(&msg,MSGSIZE);
	}
}

bool CDeamonManager::ActivateDeamon(unsigned int IPAddr) const
{
	int pos = 0;
	bool found = false;
	CDeamonManager::LPWUREGSTRUCT lpRegWU = NULL;

	while ((!found) && (pos < m_RegWorkUnits.GetSize()))
	{
		lpRegWU = ((LPWUREGSTRUCT)m_RegWorkUnits.GetAt(pos));

		CClient<CDeamonSocket> *connection = (CClient<CDeamonSocket> *)(lpRegWU->connection);

		if (CSystem::SockNameToAddr(lpRegWU->deamonIP) == IPAddr)
			found = lpRegWU->active = true;
		pos++;
	}

	return found;
}

bool CDeamonManager::DeActivateDeamon(unsigned int IPAddr) const
{
	int pos = 0;
	bool found = false;
	CDeamonManager::LPWUREGSTRUCT lpRegWU = NULL;

	while ((!found) && (pos < m_RegWorkUnits.GetSize()))
	{
		lpRegWU=((LPWUREGSTRUCT)m_RegWorkUnits.GetAt(pos));

		CClient<CDeamonSocket> *connection = (CClient<CDeamonSocket> *)(lpRegWU->connection);

		if (CSystem::SockNameToAddr(lpRegWU->deamonIP) == IPAddr)
		{
			found = false;
			lpRegWU->active = false;
		}
		pos++;
	}

	return found;
}


const CDeamonManager::WUREGSTRUCT * const CDeamonManager::GetRegWorkUnit(int numRegWU) const
{
	if (numRegWU >= m_RegWorkUnits.GetSize())
		return NULL;
	else
		return ((CDeamonManager::WUREGSTRUCT *)m_RegWorkUnits.GetAt(numRegWU));
}

const CDeamonManager::WORKUNITSTRUCT * const CDeamonManager::GetWorkUnit(unsigned short WUID) const
{
	CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;

	if (0 != m_WorkUnits.Lookup(WUID,(void*&)lpWU))
		return lpWU;
	else
		return NULL;
}

bool CDeamonManager::ReleaseWorkUnit(unsigned short WUID)
{
	CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;

	if (0 != m_WorkUnits.Lookup(WUID,(void*&)lpWU))
	{
		lpWU->jobDone = 100.0f;
		m_WorkUnits.RemoveKey(WUID);

		LPWUREGSTRUCT lpWUReg = (LPWUREGSTRUCT)m_RegWorkUnits.GetAt(lpWU->regID);
		lpWUReg->available++;
		return true;
	}
	else
		return false;
}

int CDeamonManager::AllocateWorkUnits(unsigned int requestedWU,const CDeamonManager::WORKUNITSTRUCT **&pListWU)
{
	int *sWU = SelectWorkUnits(requestedWU);
	if (sWU == NULL)
	{
		pListWU = NULL;
		return 0;
	}

	CDeamonManager::WORKUNITSTRUCT **pWUs = new CDeamonManager::LPWORKUNITSTRUCT[sWU[0]];

	int max = sWU[0];

	for (int i=0;i<max;i++)
	{
		// getting an available work unit
		CDeamonManager::LPWUREGSTRUCT lpWUReg = (CDeamonManager::LPWUREGSTRUCT)m_RegWorkUnits.GetAt(sWU[i+1]);

		// creating Work Unit job structure
		pWUs[i] = new CDeamonManager::WORKUNITSTRUCT;
		pWUs[i]->regID = sWU[i+1];
		pWUs[i]->workUnitID = m_counter++;
		m_WorkUnits.SetAt(pWUs[i]->workUnitID,pWUs[i]);

		// setting extra parameters
		pWUs[i]->jobDone = 0.0f;
	}

	//	The deamon manager is the only one allowed
	//	to modify a work unit, so even if not advised,
	//	this cast is safe here
	pListWU = (const CDeamonManager::WORKUNITSTRUCT **)pWUs;

	delete [] sWU;

	return max;
}

bool CDeamonManager::DestroyWorkUnit(unsigned short WUID)
{
	CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;

	if (0 != m_WorkUnits.Lookup(WUID,(void*&)lpWU))
	{
		m_WorkUnits.RemoveKey(WUID);
		delete lpWU;
		return true;
	}
	else
		return false;
}

int* CDeamonManager::SelectWorkUnits(unsigned int requestedWU)
{
	if (m_RegWorkUnits.GetSize()==0)
		return NULL;
	else
	{
		int *sWU = new int[requestedWU+1];
		int pos=0;
		unsigned int nb=0;
		CDeamonManager::LPWUREGSTRUCT lpRegWU;

		while ((pos<m_RegWorkUnits.GetSize())&&(nb<requestedWU))
		{
			lpRegWU=((CDeamonManager::LPWUREGSTRUCT)m_RegWorkUnits.GetAt(pos));

			if ((lpRegWU->available>0) && (lpRegWU->active))
			{
				lpRegWU->available--;
				sWU[++nb]=pos;
			}
			else
				pos++;
		}
		sWU[0]=nb;
		return sWU;
	}
}


void CDeamonManager::DisconnectDeamon(CString sname, unsigned int port) const
{
	int pos=0;
	int nb=0;
	CDeamonManager::LPWUREGSTRUCT lpRegWU;
	bool found = false;

	CClient<CDeamonSocket> *connection = NULL;

	while ((!found) && (pos < m_RegWorkUnits.GetSize()))
	{
		lpRegWU=((CDeamonManager::LPWUREGSTRUCT)m_RegWorkUnits.GetAt(pos));

		connection = (CClient<CDeamonSocket> *)(lpRegWU->connection);

		if ((connection->GetSockName() == sname) && (connection->GetPort() == port))
			found = true;
		else
			pos++;
	}

	if (found)
	{	
		connection->DisconnectServer();
		delete connection;
		lpRegWU->connection = NULL;
	}
}

bool CDeamonManager::SetRegWUNbCPU(unsigned int nbWU,unsigned char nbCPU) const
{
	if (nbWU < (unsigned int)m_RegWorkUnits.GetSize())
	{
		LPWUREGSTRUCT lpRegWU = ((LPWUREGSTRUCT)m_RegWorkUnits.GetAt(nbWU));

		int libre = lpRegWU->available;

		if (nbCPU<(lpRegWU->nbProcs-libre))
			return false;
		else if ((nbCPU>0)&&(nbCPU<17))
		{
			lpRegWU->available = (unsigned char)(libre+(nbCPU-lpRegWU->nbProcs));
			if (lpRegWU->available < 0)
				lpRegWU->available = 0;
			lpRegWU->nbProcs = nbCPU;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool CDeamonManager::SetRegWUDeamon(unsigned int nbWU,CString deamonIP) const
{
	if (nbWU < (unsigned int)m_RegWorkUnits.GetSize())
	{
		LPWUREGSTRUCT lpRegWU = ((LPWUREGSTRUCT)m_RegWorkUnits.GetAt(nbWU));
		lpRegWU->deamonIP = deamonIP;
		return true;
	}
	else
		return false;
}

bool CDeamonManager::SetWUClient(unsigned short WUID,unsigned int clientID,void* connection) const
{
	CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;

	if (0 != m_WorkUnits.Lookup(WUID,(void*&)lpWU))
	{
		lpWU->clientID = clientID;
		lpWU->connection = connection;
		return true;
	}
	else
		return false;
}

bool CDeamonManager::SetWUPercentage(unsigned short WUID,float percentage) const
{
	CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;

	if (0 != m_WorkUnits.Lookup(WUID,(void*&)lpWU))
	{
		lpWU->jobDone = percentage;
		return true;
	}
	else
		return false;
}