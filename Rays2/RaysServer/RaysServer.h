/***************************************************************************/
/*                                                                         */
/*  RaysServer.h                                                           */
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


#if !defined(AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_)
#define AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif



#if !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ServerSocket.h"
#endif

#include "../Messages.h"			// io messages IDs and structs


/////////////////////////////////////////////////////////////////////////////
// CRaysServerApp:

RAPTOR_NAMESPACE

namespace RaysServer
{

	class CMsgManager;
	class CFileManager;
	class CServerTransport;
	class CDeamonManager;
	class CJobManager;

	class CRaysServerApp
	{
	public:
		//!	Constructor.
		CRaysServerApp();

		bool Start(const std::string &addrStr, uint16_t port);
		bool Quit(void);

		//!	Returns application deamon manager.
		//CDeamonManager* getDeamonManager(void) const { return m_pDeamonManager; };
		
		//!	Request Server stop.
		void requestExit() { m_bExit = true; };

		//!	Return Server status.
		bool doExit(void) const { return m_bExit; };

		// Implementation
		void DisconnectDeamon(const std::string &sname, unsigned int port);
		void WriteMessage(UINT id, const std::string &msg);
		void ManageMsg(MSGSTRUCT& msg, unsigned char raw_data[]);

		//void UpdateWorkUnitDeamon(const CDeamonManager::WUREGSTRUCT * const wu);

		void RegisterWorkUnit(const std::string &wu, const std::string &deamon);
		void SetRegWUNbCPU(int nbWU, unsigned char nbCPU);
		void UnregisterWorkUnit(int nbWU);


		void SaveFinishedJob(uint32_t jobID);

		//	Adds a new incoming connection from underlying
		//	server to a pool of available connections 
		//	( i.e. potential client connection requests 
		//	or work unit ready for work )
		//	this connection will be used later for a 
		//	job start request
		void AddNewConnection(iosock_base_t *io);

		//	Handles disconnection to abort running jobs
		void CloseConnection(iosock_base_t *io);

		bool MsgProcessorAvailable(bool force = false);

		const CDeamonManager * const GetDeamonManager() const { return m_pDeamonManager; };


	private:
		unsigned char				m_nbWUperJOB;
		int32_t						m_wUnitPriority;
		uint32_t					m_deamonDelay;
		LARGE_INTEGER				m_baseFrequency;
		bool						m_started;			//! Flag for server start & stop status.
		bool						m_bExit;			//!	Flag for server start & stop requests.
		CRaptorMutex				processLock;

		float		m_globalJobDone;	//!	Percentage of overall work performed.

		//	Client/Server IO variables
		CServerTransport *m_pTransport;
		CDeamonManager	*m_pDeamonManager;
		CMsgManager		*m_msgManager;
		CFileManager	*m_fileManager;
		CJobManager		*m_pJobManager;

		iosock_base_t *UseNewConnection(unsigned int addr, unsigned int port, bool capture = false);
		bool	InstallPlugin(const std::string &pname, iosock_base_t *connection);

		friend class CMsgManager;
		friend UINT MsgProcessor(void *pParam);


		//	Msg processor functions prototypes
		//	Thread callbacks:
		//	the thread msg is called when a Win32 message ( regMsgID ) is
		//	sent to the thread from the message manager.
		//	The message manager updates the pool of messages directly
		//	received from the socket, and when one message is ready to be
		//	processed, this callback is then called through win32.
		//	this callback fetches the message data and then call ProcessMsg.
		void Process_THREAD_MSG(UINT wParam, LONG lParam);
		void Process_DEAMON_MSG(UINT wParam, LONG lParam);


		//	The main server processing routine:
		//	every reuest is dispatched from this routine to
		//	one of the Process_XXX routines hereunder
		void ProcessMsg(MSGSTRUCT& msg, unsigned char raw_data[]);

		//	Specific server processing routines.
		void Process_JOB_REQUEST(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_JOB_PERCENT(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_JOB_RUN(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_JOB_STOP(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_JOB_WUNIT(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_JOB_STATUS(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_OBJ_DATA(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_OBJ_TEXTURE(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_OBJ_PLUGIN(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_IMG_REQUEST(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_ACK(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_DMN_ACTIVE(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_DMN_INACTIVE(MSGSTRUCT& msg, unsigned char raw_data[]);
		void Process_DMN_DISPATCHJOB(MSGSTRUCT& msg, unsigned char raw_data[]);
	};
}

#endif // !defined(AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_)
