/***************************************************************************/
/*                                                                         */
/*  DeamonManager.cpp                                                      */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "stdafx.h"

#if !defined(AFX_DEAMONMANAGER_H__F7EF715A_5E86_4C65_B6E7_2751FAE87A91__INCLUDED_)
	#include "DeamonManager.h"
#endif

using namespace RaysServer;


DWORD __stdcall DeamonProcessor( LPVOID pParam )
{
	CDeamonManager *deamon = (CDeamonManager*)pParam;
	while (!deamon->doExit())
	{
		deamon->UpdateDeamons();
		Sleep(deamon->getPollingDelay() * 1000);
	}
	return 0;
}

bool CDeamonManager::UpdateDeamons(void) const
{
	bool res = true;
	for (unsigned int i = 0; i < m_Deamons.size(); i++)
		res = res && DeamonStatus(i);

	return res;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeamonManager::CDeamonManager()
	:m_counter(0), m_deamonPoller(0),
	m_pollingDelay(10), m_bExit(false)
{
}

CDeamonManager::~CDeamonManager()
{
}

void CDeamonManager::requestExit()
{
	m_bExit = true;

	if (NULL != m_deamonPoller)
	{
		WaitForSingleObject(m_deamonPoller, INFINITE);
		CloseHandle(m_deamonPoller);
	}
}

void CDeamonManager::Start(unsigned int delay_in_seconds)
{
	if (delay_in_seconds > 0)
		m_pollingDelay = delay_in_seconds;

	m_deamonPoller = CreateThread(	NULL, 
									0, 
									DeamonProcessor, 
									(void*)this, 
									CREATE_SUSPENDED, 0);
	ResumeThread(m_deamonPoller);
}

bool CDeamonManager::unregisterDeamon(size_t numWU)
{
	if (numWU >= m_Deamons.size())
		return false;

	deamon_struct *lpWU = m_Deamons[numWU];
	//!	Chage this condition: check owned jobs instead of available procs.
	if (lpWU->nbProcsAvailable != lpWU->nbProcs)
		return false;

	m_Deamons.erase(m_Deamons.begin() + numWU);
	bool bDisconnect = true;

	if (lpWU->connection != NULL)
	{
		bDisconnect = lpWU->connection->disconnectServer();
		delete lpWU->connection;
	}

	delete lpWU;
	return bDisconnect;
}

bool CDeamonManager::registerDeamon(const std::string& deamonIP, uint16_t port)
{
	CDeamonManager::deamon_struct *WU;

	unsigned int pos = 0;
	while (pos<m_Deamons.size())
	{
		WU = m_Deamons[pos++];

		if (WU->deamonIP == deamonIP)
			return false;
	}

	CDeamonClient *connection = new CDeamonClient();
	if (connection->connectToServer(deamonIP, port))
	{
		m_counter++;

		WU = new CDeamonManager::deamon_struct;
		WU->deamonID = m_counter;
		WU->jobDone = 0;
		WU->nbProcs = 0;
		WU->nbProcsAvailable = 0;
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

bool CDeamonManager::DeamonStatus(size_t numDeamon) const
{
	if (numDeamon >= m_Deamons.size())
		return false;

	CDeamonManager::deamon_struct * WU = m_Deamons[numDeamon];
	if (NULL == WU)
		return false;

	if (NULL == WU->connection)
		return false;

	MSGSTRUCT msg;
	msg.msg_id = DMN_STATUS;
	msg.msg_data[0] = 0;
	msg.msg_data[1] = 0;
	msg.msg_data[2] = 0;
	msg.msg_data[3] = 0;
	msg.msg_data[4] = 0;
	WU->connection->write(&msg, MSGSIZE);

	void * new_msg = NULL;
	size_t size = 0;
	WU->connection->read(new_msg, size);
	// check returned IP in msg.msg_data[4] for full safe query
	if ((size != MSGSIZE) || (NULL == new_msg))
	{
		WU->active = false;
		return false;
	}
	
	msg = *(LPMSGSTRUCT)new_msg;
	WU->nbProcs = msg.msg_data[0];
	WU->nbProcsAvailable = msg.msg_data[1];
	WU->jobDone = msg.msg_data[2];
	WU->active = true;
	return true;
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

const CDeamonManager::deamon_struct * CDeamonManager::getDeamon(size_t numDeamon) const
{
	if (numDeamon < m_Deamons.size())
		return m_Deamons[numDeamon];
	else
		return NULL;
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

bool CDeamonManager::DispatchJobToWorkunits(uint32_t jobID, int nbWU, CDeamonManager::deamon_struct *pWU)
{
	for (unsigned int i = 0; i<nbWU; i++)
	{
		CDeamonManager::deamon_struct *lpWUReg = m_Deamons[i];
		/*
		unsigned char *buffer = new unsigned char[size + MSGSIZE];
		MSGSTRUCT msg;

		msg.msg_id = DMN_DISPATCHJOB;
		msg.msg_data[0] = lpJob->workUnits[i]->workUnitID;
		msg.msg_data[1] = lpWUReg->nbProcs;
		msg.msg_data[2] = m_Server->GetAddr();
		msg.msg_data[3] = m_Server->GetPort();
		msg.msg_data[4] = m_wUnitPriority;

		memcpy(buffer, &msg, MSGSIZE);
		memcpy(buffer + MSGSIZE, LPCTSTR(lpWUReg->source), size);

		CClient<CDeamonSocket>* deamon = (CClient<CDeamonSocket>*)(lpWUReg->connection);
		deamon->Write(buffer, size + MSGSIZE);
		*/
	}

	return false;
}

int CDeamonManager::AllocateWorkUnits(unsigned int requestedWU, deamon_struct* &pListWU)
{
	int *sWU = SelectWorkUnits(requestedWU);
	if (sWU == NULL)
	{
		pListWU = NULL;
		return 0;
	}

	int max = sWU[0];
	deamon_struct* pWUs = new deamon_struct[max];
	
	for (int i=0;i<max;i++)
	{
		deamon_struct* pDeamon = m_Deamons[sWU[i + 1]];

		// creating Work Unit job structure
		pWUs[i].nbProcs = pDeamon->nbProcs;
		pWUs[i].deamonID = pDeamon->deamonID;
		pWUs[i].active = true;
		pWUs[i].nbProcsAvailable = 1; //? pDeamon->nbProcsAvailable;
		pWUs[i].connection = NULL;
		pWUs[i].deamonIP = "127.0.0.1";

		// setting extra parameters
		pWUs[i].jobDone = 0.0f;
	}

	//	The deamon manager is the only one allowed
	//	to modify a work unit, so even if not advised,
	//	this cast is safe here
	pListWU = pWUs;

	delete [] sWU;

	return max;
}

int* CDeamonManager::SelectWorkUnits(unsigned int requestedWU)
{
	if (m_Deamons.size()==0)
		return NULL;
	else
	{
		int *sWU = new int[requestedWU+1];
		int pos=0;
		unsigned int nb=0;

		deamon_struct* pDeamon = m_Deamons[pos];
		int available = pDeamon->nbProcsAvailable;
		bool active = pDeamon->active;
		
		while ((pos < m_Deamons.size()) && (nb < requestedWU))
		{
			if ((available > 0) && active)
			{
				available--;
				sWU[++nb] = pos;
			}
			else
			{
				pos++;
				pDeamon = m_Deamons[pos];
				available = pDeamon->nbProcsAvailable;
				active = pDeamon->active;
			}
		}

		sWU[0] = nb;
		return sWU;
	}
}


