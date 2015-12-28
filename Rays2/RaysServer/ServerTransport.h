// ServerTransport.h: interface for the CServerTransport class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
#define AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Messages.h"			// io messages IDs and structs

#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/Server.h"
#endif
#if !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ServerSocket.h"
#endif
#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ClientSocket.h"
#endif


namespace RaysServer {

	class CServerTransport : public CServer<CServerSocket,CClientSocket>,
							 public server_base_t::request_handler_t
	{
	public:
		CServerTransport();
		virtual ~CServerTransport();

		//!	Returns this
		virtual request_handler_t &getRequestHandler(const iosock_base_t& client) const;

		//!	Stop Rays Server.
		virtual bool stopServer(void);


	private:
		typedef struct request_t
		{
			request_handler_t::request_id id;
			bool			reply;
			size_t			size;
			MSGSTRUCT		*msg;
		} request;

		//!	Implement base class.
		virtual int onNewClient(const CClientSocket &client);

		//!	Implement base class.
		virtual bool onClientClose(const CClientSocket &client);

		//!	Implements Server request callback
		virtual bool handleRequest(request_handler_t::request_id id,const void *data,size_t size);

		//! Implements Server reply request
		virtual bool handleReply(request_handler_t::request_id id, const void *&data,size_t &size);

		//!	A separate request handler for asynchronous processing.
		server_base_t::request_handler_t *m_pHandler;

		//!	Replies queue
		vector<request> m_replies;
	};
}

#endif // !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
