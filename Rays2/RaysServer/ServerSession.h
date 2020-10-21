/***************************************************************************/
/*                                                                         */
/*  ServerSession.h                                                        */
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


#if !defined(AFX_SERVERSESSION_H__CF5E6774_178C_4DF6_BB48_44B6AF2AB163__INCLUDED_)
#define AFX_SERVERSESSION_H__CF5E6774_178C_4DF6_BB48_44B6AF2AB163__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Server.h"
#endif

#include "Raptordll.h"
RAPTOR_NAMESPACE

namespace RaysServer {

	class CServerSession
	{
	public:
		CServerSession();
		virtual ~CServerSession();


		typedef struct session_t
		{
			server_base_t::request_handler_t::request_id id;
		} session;

		//! Create a new session
		bool createSession(server_base_t::request_handler_t::request_id id);

		//! Delete a session and remove all associated files
		bool closeSession(server_base_t::request_handler_t::request_id id);

		//! Return an existing session
		session getSession(server_base_t::request_handler_t::request_id id);


	private:
		//!	Mutex to protect session accesses
		CRaptorMutex		m_mutex;

		//!	Sessions list
		std::vector<session> m_sessions;
	};
}

#endif // !defined(AFX_SERVERSESSION_H__CF5E6774_178C_4DF6_BB48_44B6AF2AB163__INCLUDED_)