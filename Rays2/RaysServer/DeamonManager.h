#pragma once

#include <windows.h>
#include "..\Messages.h"

#include <string>
#include <vector>
using namespace std;

#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/ClientSocket.h"
#endif
#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Client.h"
#endif

namespace RaysServer {

	class CDeamonManager
	{
	public:
		class CDeamonClient : public CClient<CClientSocket>
		{
		public:
			CDeamonClient(server_base_t *m_pServer)
			{ m_socket.setServer(m_pServer); };
		};

		typedef struct deamon_struct_t
		{
			unsigned int	deamonID; // outer identifier ( server counter )
			float			jobDone;	// percentage of job actually done
			unsigned int	nbProcs;
			unsigned int	nbProcsAvailable;
			bool			active;
			std::string		deamonIP;
			CDeamonClient	*connection;
		} DEAMONSTRUCT;
		typedef DEAMONSTRUCT* LPDEAMONSTRUCT;
	/*
		typedef struct Message_reg_t
		{
			MSGSTRUCT		msg;
			unsigned char	*raw_data;
		} MSGREGSTRUCT;
		typedef MSGREGSTRUCT* LPMSGREGSTRUCT;
	*/


	public:
		//bool InstallPlugin(	unsigned int IP,unsigned int port,unsigned int validDate,
		//					CString name,unsigned int size,unsigned char* plugin) const;

		CDeamonManager(server_base_t *server);
		virtual ~CDeamonManager();

		//!	Request Deamon stop.
		void requestExit() { m_bExit = true; };

		//!	Return exit status.
		bool doExit(void) const { return m_bExit; };

		//!	Updates the delay for periodic deamon activation.
		void setPollingDelay(unsigned int delay_in_seconds)
		{
			if (delay_in_seconds > 0)
				m_pollingDelay = delay_in_seconds;
		}
		unsigned int getPollingDelay(void) const
		{
			return m_pollingDelay; 
		}

		//!	Create a new deamon
		bool registerDeamon(const std::string& deamonIP);

		unsigned int getNbDeamons(void) const { return m_Deamons.size(); };
		
		//!	Returns the deamon descritor structure.
		const LPDEAMONSTRUCT getDeamon(unsigned int WUID) const;

		//!	Update deamon status.
		bool DeamonStatus(unsigned int numDeamon) const;

		//!	Activate all deamons
		bool UpdateDeamons(void) const;

		//	Allocate workunits as requested and if possible.
		//	Workunits are based on the registered workunits model set
		//	Returns the number of workunits actually reserved
		//int AllocateWorkUnits(unsigned int requestedWU,const CDeamonManager::WORKUNITSTRUCT **&pListWU);

		//	A workunit used for a job has finished
		//	and then returns to the pool of available
		//	workunit for next job
		//bool ReleaseWorkUnit(unsigned short WUID);

		//	Return true if registered workunit successfuly created
		bool destroyDeamon(unsigned int numRegWU);

	private:
		server_base_t			*m_pServer;
		vector<LPDEAMONSTRUCT>	m_Deamons;	// array of registered work units
		unsigned int			m_counter;		// unique work Unit ID counter
		unsigned int			m_pollingDelay;
		HANDLE					m_deamonPoller;
		HANDLE					m_pollerEvent;
		bool					m_bExit;

		//int* SelectWorkUnits(unsigned int requestedWU);
	};
}