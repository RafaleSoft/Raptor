// ServerTransport.cpp: implementation of the CServerTransport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
    #include "ServerTransport.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
    #include "RaptorInstance.h"
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
	CRaptorInstance *pHandler = CRaptorInstance::getInstance();
	return *pHandler;
}

bool CServerTransport::stopServer(void)
{
	CRaptorInstance *pHandler = CRaptorInstance::getInstance();
	
	if (pHandler->stop())
	{
		return CServer<CServerSocket,CClientSocket>::stopServer();
	}
	else
		return false;
}

bool CServerTransport::onClientClose(const CClientSocket &client)
{
	CRaptorInstance *pHandler = CRaptorInstance::getInstance();
	return pHandler->closeSession((request_handler_t::request_id)&client);
}
