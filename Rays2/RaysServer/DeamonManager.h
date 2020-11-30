/***************************************************************************/
/*                                                                         */
/*  DeamonManager.h                                                        */
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

#if !defined(AFX_DEAMONMANAGER_H__F7EF715A_5E86_4C65_B6E7_2751FAE87A91__INCLUDED_)
#define AFX_DEAMONMANAGER_H__F7EF715A_5E86_4C65_B6E7_2751FAE87A91__INCLUDED_


#include <string>
#include <vector>
#include <stdint.h>
using namespace std;

#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/ClientSocket.h"
#endif
#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Client.h"
#endif

#include "..\Messages.h"


namespace RaysServer
{
	class CDeamonManager
	{
	public:
		class CDeamonClient : public CClient<CClientSocket>
		{
		public:
			CDeamonClient()
			{  };

			// TODO: handle connection disconnect
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
		} deamon_struct;

	public:
		CDeamonManager();
		virtual ~CDeamonManager();

		//!	Request Deamon stop.
		void requestExit(void);

		//!	Return exit status.
		bool doExit(void) const { return m_bExit; };

		//!
		//! Deamons management.
		//!

		//!	Updates the delay for periodic deamon activation.
		void Start(unsigned int delay_in_seconds);
		
		unsigned int getPollingDelay(void) const { return m_pollingDelay;  }

		//!	Create a new deamon: return true if registered deamon successfuly connected
		bool registerDeamon(const std::string& deamonIP, uint16_t port);

		//!	Release a known deamon: return true if registered deamon successfuly disconnected
		bool unregisterDeamon(size_t numRegWU);

		size_t getNbDeamons(void) const { return m_Deamons.size(); };
		
		//!	Returns the deamon descritor structure.
		const deamon_struct* getDeamon(size_t numDeamon) const;

		
		//!	Activate all deamons
		bool UpdateDeamons(void) const;

		//!
		//! Workunits management.
		//!

		//	Allocate workunits as requested and if possible.
		//	Workunits are based on the registered workunits model set
		//	@return the number of workunits actually reserved
		int AllocateWorkUnits(unsigned int requestedWU, deamon_struct* &pListWU);

		//	A workunit used for a job has finished
		//	and then returns to the pool of available
		//	workunit for next job
		//bool ReleaseWorkUnit(unsigned short WUID);

		//!	Registered deamons will launch workunits to work on job jobID.
		//! @return false if any error from registered deamons.
		bool DispatchJobToWorkunits(uint32_t jobID, int nbWU, CDeamonManager::deamon_struct *pWU);

		

		//bool InstallPlugin(	unsigned int IP,unsigned int port,unsigned int validDate,
		//					CString name,unsigned int size,unsigned char* plugin) const;


	private:
		std::vector<deamon_struct*>	m_Deamons;	// array of registered deamons
		uint32_t				m_counter;		// unique work Unit ID counter
		uint32_t				m_pollingDelay;
		HANDLE					m_deamonPoller;
		bool					m_bExit;

		//!	Update deamon status.
		bool DeamonStatus(size_t numDeamon) const;

		//!	Choose workunits from registered deamons.
		int* SelectWorkUnits(unsigned int requestedWU);
	};
}

#endif // !defined(AFX_DEAMONMANAGER_H__F7EF715A_5E86_4C65_B6E7_2751FAE87A91__INCLUDED_)
