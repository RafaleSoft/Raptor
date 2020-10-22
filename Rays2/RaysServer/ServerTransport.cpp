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
#if !defined(AFX_SERVERSESSION_H__CF5E6774_178C_4DF6_BB48_44B6AF2AB163__INCLUDED_)
	#include "ServerSession.h"
#endif

#include <iostream>

using namespace RaysServer;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServerTransport::CServerTransport()
	:m_pHandler(NULL), m_sessionManager(NULL)
{
	m_sessionManager = new CServerSession();

	m_processors[SES_OPEN] = &CServerTransport::Process_SES_OPEN;
	m_processors[SES_CLOSE] = &CServerTransport::Process_SES_CLOSE;
	m_processors[JOB_DATA] = &CServerTransport::Process_JOB_DATA;
}

CServerTransport::~CServerTransport()
{
	if (NULL != m_sessionManager)
		delete m_sessionManager;
	m_sessionManager = NULL;
}


bool CServerTransport::stopServer(void)
{
	return CServer<CServerSocket, CClientSocket>::stopServer();
}

size_t CServerTransport::onNewClient(const CClientSocket &client)
{
	Network::userOutput(INetworkLogger::NETWORK_INFO, "New client connected");
	return getNumClients();
}

bool CServerTransport::onClientClose(const CClientSocket &client)
{
	Network::userOutput(INetworkLogger::NETWORK_INFO, "Client disconnected");

	std::cout << "Rays Server closing session for client: " << (request_id)&client << std::endl;

	return m_sessionManager->closeSession((request_id)&client);
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

	bool messageProcessed = true;
	std::map<RAYS_MSG_ID, request_processor_t>::const_iterator processor = m_processors.find(rq.msg->msg_id);
	if (m_processors.end() != processor)
	{
		request_processor_t p = (*processor).second;
		messageProcessed = (this->*p)(rq);
	}
	else
	{
		std::cout << "Rays Server unknown message id: " << rq.msg->msg_id << " from client: " << id << std::endl;
		messageProcessed = false;
	}

	return messageProcessed;
}

bool CServerTransport::handleReply(request_handler_t::request_id id, const void *&data,size_t &size)
{
	request r;
	r.id = 0;

	CRaptorLock lock(m_mutex);

	if (m_replies.size() > 0)
	{
		std::vector<request>::const_iterator it = m_replies.begin();
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

bool CServerTransport::Process_SES_OPEN(request &rq)
{
	std::cout << "Rays Server opening session for client: " << rq.id << std::endl;
	bool openSession = m_sessionManager->createSession(rq.id);

	rq.reply = true;
	rq.size = sizeof(MSGSTRUCT);	// this is the minimum size, so no need for reallocation.
	rq.msg->msg_data[0] = ((uint64_t)rq.id & 0xffffffff);
	rq.msg->msg_data[1] = ((uint64_t)rq.id >> 32) & 0xffffffff;
	rq.msg->msg_data[2] = 0;
	rq.msg->msg_data[3] = 0;
	rq.msg->msg_data[4] = 0;
	rq.msg->msg_id = SES_ID;

	CRaptorLock lock(m_mutex);
	m_replies.push_back(rq);

	return openSession;
}

bool CServerTransport::Process_SES_CLOSE(request &rq)
{
	std::cout << "Rays Server closing session for client: " << rq.id << std::endl;

	//! No reply, delete allocated bloc because processing ends here.
	delete[] rq.msg;

	return m_sessionManager->closeSession(rq.id);
}

bool CServerTransport::Process_JOB_DATA(request &rq)
{
	std::cout << "Rays Server receiving job data for client: " << rq.id << std::endl;
	
	CServerSession::session_t session = m_sessionManager->getSession(rq.id);

	bool bdata = false;
	if (session.id == rq.id)
	{
		uint8_t* raw_data = (uint8_t*)(rq.msg) + sizeof(MSGSTRUCT);
		bdata = m_sessionManager->saveSessionFile(rq.id, "RaysData.pck", raw_data, rq.size - sizeof(MSGSTRUCT));
	}

	//! No reply, delete allocated bloc because processing ends here.
	delete[] rq.msg;

	return bdata;
}
