#include "stdafx.h"
#include "DeamonManager.h"
#include "Subsys/CodeGeneration.h"

using namespace RaysServer;

static const char* POLLING_EVENT = "POLLING_EVENT";
DWORD __stdcall DeamonProcessor( LPVOID pParam )
{
	CDeamonManager *deamon = (CDeamonManager*)pParam;
	HANDLE m_pollerEvent = CreateEventA(NULL, TRUE, TRUE, POLLING_EVENT);

	while (!deamon->doExit())
	{
		deamon->UpdateDeamons();
		WaitForSingleObject(m_pollerEvent, deamon->getPollingDelay() * 1000);
	}

	CloseHandle(m_pollerEvent);
	return 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeamonManager::CDeamonManager(server_base_t *server)
	:m_pServer(server), m_counter(0), m_pollingDelay(10), m_bExit(false)
{
	m_deamonPoller = CreateThread(NULL, 0, DeamonProcessor, (void*)this, CREATE_SUSPENDED, 0);
	m_pollerEvent = CreateEventA(NULL, FALSE, FALSE, POLLING_EVENT);

	//	Place this in start... ?
	ResumeThread(m_deamonPoller);
}

CDeamonManager::~CDeamonManager()
{
	if (NULL != m_deamonPoller)
	{
		requestExit();
		SetEvent(m_pollerEvent);
		WaitForSingleObject(m_deamonPoller, INFINITE);
		CloseHandle(m_deamonPoller);
		CloseHandle(m_pollerEvent);
	}

	//	Clear Registered workunits
	int nbRWU = m_Deamons.size();
	while (nbRWU > 0)
	{
		destroyDeamon(0);
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

bool CDeamonManager::destroyDeamon(unsigned int numWU)
{
	if (numWU >= m_Deamons.size())
		return false;

	LPDEAMONSTRUCT lpWU = m_Deamons[numWU];
	//!	Chage this condition: check owned jobs instead of available procs.
	if (lpWU->nbProcsAvailable != lpWU->nbProcs)
		return false;

	m_Deamons.erase(m_Deamons.begin() + numWU);
	bool bDisconnect = true;

	if (lpWU->connection != NULL)
	{
		//! todo: cancel all registered jobs.
		/*
		MSGSTRUCT msg;
		msg.msg_header = MSG_START;
		msg.msg_id = DMN_INACTIVE;
		msg.msg_size = 0;
		msg.msg_tail = MSG_END;
		lpWU->connection->write(&msg,MSGSIZE);
		*/
		bDisconnect = lpWU->connection->disconnectServer();
		delete lpWU->connection;
	}

	delete lpWU;
	return bDisconnect;
}

bool CDeamonManager::registerDeamon(const std::string& deamonIP)
{
	CDeamonManager::LPDEAMONSTRUCT WU;

	unsigned int pos = 0;
	while (pos<m_Deamons.size())
	{
		WU = m_Deamons[pos++];

		if (WU->deamonIP == deamonIP)
			return false;
	}

	CDeamonClient *connection = new CDeamonClient(m_pServer);
	if (connection->connectToServer(deamonIP,PORTBASE+1))
	{
		WU = new CDeamonManager::DEAMONSTRUCT;
		WU->deamonID = m_counter++;
		WU->jobDone = 0;
		WU->nbProcs = 1;
		WU->nbProcsAvailable = 1;
		WU->active = false;
		WU->deamonIP = deamonIP;
		WU->connection = connection;
		m_Deamons.push_back(WU);
		return true;
	}
	else
	{
		delete connection;
		return false;
	}
}

bool CDeamonManager::DeamonStatus(unsigned int numDeamon) const
{
	/*
	CDeamonManager::LPDEAMONSTRUCT WU = NULL;
	unsigned int pos = 0;
	while (pos<m_Deamons.size() && (NULL == WU))
	{
		WU = m_Deamons[pos++];
		if (deamonIP != WU->deamonIP)
			WU = NULL;
	}
	*/

	if (numDeamon >= m_Deamons.size())
		return false;
	CDeamonManager::LPDEAMONSTRUCT WU = m_Deamons[numDeamon];

	if (NULL == WU)
		return false;
	WU->active = false;
	if (NULL == WU->connection)
		return false;

	MSGSTRUCT msg;
	msg.msg_header = MSG_START;
	msg.msg_id = DMN_STATUS;
	msg.msg_size = 0;
	msg.msg_tail = MSG_END;
	WU->connection->write(&msg, MSGSIZE);

	void * new_msg = NULL;
	unsigned int size = 0;
	WU->connection->read(new_msg, size);
	// check returned IP in msg.msg_data[4] for full safe query
	if ((size != MSGSIZE) || (NULL == new_msg))
		return false;
	
	msg = *(LPMSGSTRUCT)new_msg;
	WU->nbProcs = msg.msg_data[0];
	WU->nbProcsAvailable = msg.msg_data[1];
	WU->jobDone = msg.msg_data[2];
	WU->active = true;
	return true;
}

bool CDeamonManager::UpdateDeamons(void) const
{
	bool res = true;
	for (unsigned int i = 0; i < m_Deamons.size(); i++)
	{
		res = res && DeamonStatus(i);
	}
	return res;
}

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

const CDeamonManager::LPDEAMONSTRUCT CDeamonManager::getDeamon(unsigned int WUID) const
{
	if (WUID >= m_Deamons.size())
		return NULL;
	else
		return m_Deamons[WUID];
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

