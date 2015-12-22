#include "stdafx.h"
#include "DeamonManager.h"
#include "Subsys/CodeGeneration.h"

/*
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
*/

using namespace RaysServer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeamonManager::CDeamonManager()
{
	m_counter = 0;
/*
	m_deamonPoller = AfxBeginThread(DeamonProcessor, 
									(void*)this, 
									THREAD_PRIORITY_NORMAL, 
									0, 
									CREATE_SUSPENDED, 
									NULL );
*/
}

CDeamonManager::~CDeamonManager()
{
	//	Clear Registered workunits
	int nbRWU = m_WorkUnits.size();
	while (nbRWU > 0)
	{
		destroyWorkUnit(0);
		nbRWU--;
	}

	if (nbRWU > 0)
	{
		System::Windows::Forms::MessageBox::Show("Some workunits were still working and could not be destroyed !",
												 "Deamon Manager Error",
												 System::Windows::Forms::MessageBoxButtons::OK,
												 System::Windows::Forms::MessageBoxIcon::Exclamation );
	}
}

bool CDeamonManager::destroyWorkUnit(unsigned int numWU)
{
	if (numWU >= m_WorkUnits.size())
		return false;

	LPWORKUNITSTRUCT lpWU = m_WorkUnits[numWU];
	if (lpWU->nbProcsAvailable != lpWU->nbProcs)
		return false;

	m_WorkUnits.erase(m_WorkUnits.begin() + numWU);
	bool bDisconnect = true;

	if (lpWU->connection != NULL)
	{
		MSGSTRUCT			msg;
		msg.msg_header = MSG_START;
		msg.msg_id = DMN_INACTIVE;
		msg.msg_size = 0;
		msg.msg_tail = MSG_END;

		lpWU->connection->write(&msg,MSGSIZE);
		bDisconnect = lpWU->connection->disconnectServer();
		delete lpWU->connection;
	}

	delete lpWU;
	return bDisconnect;
}

bool CDeamonManager::registerDeamon(const std::string& deamonIP)
{
	CDeamonManager::LPWORKUNITSTRUCT WU;

	unsigned int pos = 0;
	while (pos<m_WorkUnits.size())
	{
		WU = m_WorkUnits[pos++];

		if (WU->deamonIP == deamonIP)
			return false;
	}

	CClient<CClientSocket> *connection = new CClient<CClientSocket>();
	if (connection->connectToServer(deamonIP,PORTBASE+1))
	{
		WU = new CDeamonManager::WORKUNITSTRUCT;
		WU->workUnitID = m_counter++;
		WU->jobDone = 0;
		WU->nbProcs = 1;
		WU->nbProcsAvailable = 1;
		WU->active = false;
		WU->deamonIP = deamonIP;
		WU->connection = connection;
		m_WorkUnits.push_back(WU);
		return true;
	}
	else
	{
		delete connection;
		return false;
	}
}

/*
void CDeamonManager::Run()
{
	m_deamonPoller->ResumeThread();
}
*/
/*
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
*/
/*
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
*/
/*
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
*/

const CDeamonManager::LPWORKUNITSTRUCT CDeamonManager::getWorkUnit(unsigned int WUID) const
{
	if (WUID >= m_WorkUnits.size())
		return NULL;
	else
		return m_WorkUnits[WUID];
}
/*
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
*/
/*
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
*/
/*
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
*/
/*
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
*/
/*
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
*/
/*
bool CDeamonManager::setWUNbCPU(unsigned int nbWU,unsigned int nbCPU) const
{
	if (nbWU < m_WorkUnits.size())
	{
		LPWORKUNITSTRUCT lpWU = m_WorkUnits[nbWU];

		if (nbCPU < (lpWU->nbProcs-lpWU->nbProcsAvailable))
			return false;
		else
		{
			lpWU->nbProcsAvailable = (lpWU->nbProcsAvailable+(nbCPU-lpWU->nbProcs));
			lpWU->nbProcs = nbCPU;
			return true;
		}
	}
	else
		return false;
}
*/
bool CDeamonManager::setWUDeamon(unsigned int nbWU,const std::string &deamonIP) const
{
	if (nbWU < m_WorkUnits.size())
	{
		LPWORKUNITSTRUCT lpWU = m_WorkUnits[nbWU];
		lpWU->deamonIP = deamonIP;
		return true;
	}
	else
		return false;
}

bool CDeamonManager::SetWUClient(unsigned short WUID,unsigned int clientID,void* connection) const
{
	CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;
	return false;
}

bool CDeamonManager::SetWUPercentage(unsigned short WUID,float percentage) const
{
	CDeamonManager::WORKUNITSTRUCT *lpWU = NULL;
	return false;
}
