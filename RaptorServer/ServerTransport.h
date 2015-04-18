// ServerTransport.h: interface for the CServerTransport class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
#define AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/Server.h"
#endif
#if !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ServerSocket.h"
#endif
#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ClientSocket.h"
#endif


class CServerTransport : public CServer<CServerSocket,CClientSocket>
{
public:
	CServerTransport();
	virtual ~CServerTransport();

	virtual request_handler_t &getRequestHandler(const iosock_base_t& client) const;

	virtual bool stopServer(void);

	virtual bool onClientClose(const CClientSocket &client);

private:
};

#endif // !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
