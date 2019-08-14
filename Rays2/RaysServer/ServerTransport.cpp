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
#include <windows.h>

#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
    #include "ServerTransport.h"
#endif
#if !defined(AFX_RAYSUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "../RaysUtils.h"
#endif

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

server_base_t::request_handler_t& CServerTransport::getRequestHandler(const iosock_base_t& client) const
{
	const server_base_t::request_handler_t* rq = this;
	server_base_t::request_handler_t& handler = const_cast<server_base_t::request_handler_t&>(*rq);
	return handler;
}

bool CServerTransport::handleRequest(request_handler_t::request_id id,const void *data,size_t size)
{
	request rq;
	rq.id = id;
	rq.reply = false;
	rq.size = size;
	rq.msg = (MSGSTRUCT*)new unsigned char[size];
	memcpy(rq.msg,data,size);

	unsigned char* raw_data = (unsigned char*)(rq.msg)+rq.msg->msg_size;

	if (size > MSGSIZE)
		Network::userOutput(INetworkLogger::NETWORK_WARNING,"Extra data on message ignored !");

	switch(rq.msg->msg_id)
	{
		case JOB_ID:
			break;
		default:
			break;
	}

	return false;
}

bool CServerTransport::handleReply(request_handler_t::request_id id, const void *&data,size_t &size)
{
	request r;
	r.id = 0;

	if (m_replies.size() > 0)
	{
		vector<request>::const_iterator it = m_replies.begin();
		do
		{
			r = *it++;
		} while ((it != m_replies.end()) && (r.id != id));

		if (r.id == id)
		{
			it--;
			m_replies.erase(it);
		}
		else
			return false;
	}
	else
	{
		//	A small delay to wait for a reply ?
		//	A semaphore would be better
		Sleep(1);
		return false;
	}

	data = r.msg;
	size = r.size;
	return true;
}

bool CServerTransport::stopServer(void)
{
	return CServer<CServerSocket,CClientSocket>::stopServer();
}

int CServerTransport::onNewClient(const CClientSocket &client)
{
	return 0;
}

bool CServerTransport::onClientClose(const CClientSocket &client)
{
	return false;
}

