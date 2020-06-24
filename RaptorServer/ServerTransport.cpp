/***************************************************************************/
/*                                                                         */
/*  ServerTransport.cpp                                                    */
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

#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
    #include "ServerTransport.h"
#endif
#if !defined(AFX_RAPTORSERVERINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
    #include "RaptorServerInstance.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServerTransport::CServerTransport()
{

}

CServerTransport::~CServerTransport()
{

}

server_base_t::request_handler_t &CServerTransport::getRequestHandler(const iosock_base_t& client) const
{
	CRaptorServerInstance *pHandler = CRaptorServerInstance::GetInstance();
	return *pHandler;
}

bool CServerTransport::stopServer(void)
{
	CRaptorServerInstance *pHandler = CRaptorServerInstance::GetInstance();
	
	if (pHandler->stop())
	{
		return CServer<CServerSocket,CClientSocket>::stopServer();
	}
	else
		return false;
}

bool CServerTransport::onClientClose(const CClientSocket &client)
{
	std::cout << "Client disconnected" << std::endl;

	CRaptorServerInstance *pHandler = CRaptorServerInstance::GetInstance();
	return pHandler->closeSession((request_handler_t::request_id)&client);
}


size_t CServerTransport::onNewClient(const CClientSocket &client)
{
	std::cout << "New client connected" << std::endl;

	return getNumClients();
}
