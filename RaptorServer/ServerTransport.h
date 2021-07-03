/***************************************************************************/
/*                                                                         */
/*  ServerTransport.h                                                      */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


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

	//!	Returns this
	virtual request_handler_t &getRequestHandler(const iosock_base_t& client) const;

	//!	Implement base class.
	virtual bool stopServer(void);

	//!	Implement base class.
	virtual bool onClientClose(const CClientSocket &client);

	//!	Implement base class.
	virtual size_t onNewClient(const CClientSocket &client);

private:
};

#endif // !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
