// RaysDeamon.h : main header file for the RAYSDEAMON application
//

#if !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
#define AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_

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

RAPTOR_NAMESPACE


class RaysCmdLine;

class CRaysDeamon : public CServer<CServerSocket,CClientSocket>,
					public server_base_t::request_handler_t
{
public:
	CRaysDeamon();
	virtual ~CRaysDeamon();


	bool start(	const CCmdLineParser& cmdline );
	
	virtual server_base_t::request_handler_t &getRequestHandler(const iosock_base_t& client) const;

	virtual bool stopServer(void);

	virtual bool onClientClose(const CClientSocket &client);


private:
	//!	Implements Server request callback
	virtual bool handleRequest(request_handler_t::request_id id,const void *data,size_t size);

	//! Implements Server reply request
	virtual bool handleReply(request_handler_t::request_id id, const void *&data,size_t &size);
};

#endif // !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
