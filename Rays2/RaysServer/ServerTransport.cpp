// ServerTransport.cpp: implementation of the CServerTransport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>

#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
    #include "ServerTransport.h"
#endif

#include "RaysServerUtils.h"
using namespace RaysServer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServerTransport::CServerTransport()
	:m_pHandler(NULL)
{

}

CServerTransport::~CServerTransport()
{

}

void CServerTransport::userOutput(const std::string& msg) const
{
	RaysServerUtils::ILogger^ logger = RaysServerUtils::getLog();
	System::String^ console_msg = gcnew System::String(msg.c_str());

	logger->Log(console_msg);
	System::Console::WriteLine(console_msg);
}

server_base_t::request_handler_t &CServerTransport::getRequestHandler(const iosock_base_t& client) const
{
	//CRaptorInstance *m_pHandler = CRaptorInstance::getInstance();
	return *m_pHandler;
}

bool CServerTransport::stopServer(void)
{
	//CRaptorInstance *pHandler = CRaptorInstance::getInstance();
	
	if (true) //pHandler->stop())
	{
		return CServer<CServerSocket,CClientSocket>::stopServer();
	}
	else
		return false;
}

bool CServerTransport::onClientClose(const CClientSocket &client)
{
	//CRaptorInstance *pHandler = CRaptorInstance::getInstance();
	//return pHandler->closeSession((request_handler_t::request_id)&client);
	return false;
}
