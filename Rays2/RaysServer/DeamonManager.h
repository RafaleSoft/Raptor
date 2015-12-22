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
		typedef struct work_unit_struct_t
		{
			unsigned int	workUnitID; // outer identifier ( server counter )
			float			jobDone;	// percentage of job actually done
			unsigned int	nbProcs;
			unsigned int	nbProcsAvailable;
			bool			active;
			std::string		deamonIP;
			CClient<CClientSocket> *connection;
		} WORKUNITSTRUCT;
		typedef WORKUNITSTRUCT* LPWORKUNITSTRUCT;
	/*
		typedef struct Message_reg_t
		{
			MSGSTRUCT		msg;
			unsigned char	*raw_data;
		} MSGREGSTRUCT;
		typedef MSGREGSTRUCT* LPMSGREGSTRUCT;
	*/


	public:
		//void Run(void);
		//void DisconnectDeamon(CString sname, unsigned int port) const;

		//bool ActivateDeamon(unsigned int IPAddr) const;
		//bool DeActivateDeamon(unsigned int IPAddr) const;

		//bool InstallPlugin(	unsigned int IP,unsigned int port,unsigned int validDate,
		//					CString name,unsigned int size,unsigned char* plugin) const;

		CDeamonManager();
		virtual ~CDeamonManager();

		//	Create a new deamon (and retrieves its workunits)
		bool registerDeamon(const std::string& deamonIP);

		unsigned int getNbWorkUnits(void) const { return m_WorkUnits.size(); };
		
		const LPWORKUNITSTRUCT getWorkUnit(unsigned int WUID) const;

		//	Return false if WU update failed, true otherwize.
		//bool setWUNbCPU(unsigned int nbWU,unsigned int nbCPU) const;
		//	Return false if WU update failed, true otherwize.
		bool setWUDeamon(unsigned int nbWU,const std::string& deamonIP) const;
		//	Return false if WU update failed, true otherwize.
		bool SetWUClient(unsigned short WUID,unsigned int clientID,void* connection) const;
		//	Return false if WU update failed, true otherwize.
		bool SetWUPercentage(unsigned short WUID,float percentage) const;

		//	Allocate workunits as requested and if possible.
		//	Workunits are based on the registered workunits model set
		//	Returns the number of workunits actually reserved
		//int AllocateWorkUnits(unsigned int requestedWU,const CDeamonManager::WORKUNITSTRUCT **&pListWU);

		//	Return true if workunit actually deleted, false otherwise.
		//bool DestroyWorkUnit(unsigned short WUID);

		//	A workunit used for a job has finished
		//	and then returns to the pool of available
		//	workunit for next job
		//bool ReleaseWorkUnit(unsigned short WUID);

		//	Return true if registered workunit successfuly created
		bool destroyWorkUnit(unsigned int numRegWU);

	private:
		//friend UINT DeamonProcessor( LPVOID pParam );

		vector<LPWORKUNITSTRUCT>	m_WorkUnits;	// array of registered work units
		unsigned int				m_counter;		// unique work Unit ID counter

		//int* SelectWorkUnits(unsigned int requestedWU);
	};
}