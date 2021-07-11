/***************************************************************************/
/*                                                                         */
/*  ServerTransport.cpp                                                    */
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
#if !defined(AFX_DEAMONMANAGER_H__F7EF715A_5E86_4C65_B6E7_2751FAE87A91__INCLUDED_)
	#include "DeamonManager.h"
#endif
#if !defined(AFX_JOBMANAGER_H__4E78312A_6362_46AF_A327_07208468529A__INCLUDED_)
	#include "JobManager.h"
#endif


#include <iostream>

using namespace RaysServer;
using namespace Rays;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServerTransport::CServerTransport(CDeamonManager *pDeamon, CJobManager *pJob)
	:m_pHandler(NULL), m_sessionManager(NULL), m_pDeamon(pDeamon), m_pJob(pJob)
{
	m_sessionManager = new CServerSession();

	m_processors[SES_OPEN] = &CServerTransport::Process_SES_OPEN;
	m_processors[SES_CLOSE] = &CServerTransport::Process_SES_CLOSE;
	m_processors[JOB_DATA] = &CServerTransport::Process_JOB_DATA;
	m_processors[JOB_START] = &CServerTransport::Process_JOB_START;
	m_processors[JOB_WUNIT] = &CServerTransport::Process_JOB_WUNIT;
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

bool CServerTransport::Process_JOB_START(request &rq)
{
	std::cout << "Rays Server starting job rendering for client: " << rq.id << std::endl;

	CServerSession::session_t session = m_sessionManager->getSession(rq.id);
	if (session.id == rq.id)
	{
		rays_config_t* config = (rays_config_t*)((uint8_t*)(rq.msg) + sizeof(MSGSTRUCT));

		const CRaysSettings &settings = RaysUtils::getSettings();
		uint32_t wuPerJob = 1;
		settings.getValue<uint32_t>("nb_wu_per_job", wuPerJob);

		uint32_t jobID = 0;
		CDeamonManager::deamon_struct *pWU = NULL;
		int nbWU = m_pDeamon->AllocateWorkUnits(wuPerJob, pWU);
		if ((NULL == pWU) || (0 == nbWU))
		{
			std::cout << "No more registered work units available" << std::endl;
			std::cout << "Rendering job cancelled !" << std::endl;
		}
		else
		{
			if (nbWU != wuPerJob)
			{
				std::cout << "Not enough work units available" << std::endl;
				std::cout << "Rendering with lower capabilities !" << std::endl;
			}

			jobID = m_pJob->createJob(rq.id, config->width, config->height);

			uint16_t port = getPort();
			uint32_t addr = getHostAddr(0);
			if (false == m_pDeamon->DispatchJobToWorkunits(jobID, port, addr, nbWU, pWU))
			{
				std::cout << "Server failed to dispatch job to available work units" << std::endl;
			}
		}

		rq.reply = true;
		rq.size = sizeof(MSGSTRUCT);	// this is the minimum size, so no need for reallocation.
		rq.msg->msg_data[0] = jobID;
		rq.msg->msg_data[1] = 0;
		rq.msg->msg_data[2] = 0;
		rq.msg->msg_data[3] = 0;
		rq.msg->msg_data[4] = 0;
		rq.msg->msg_id = JOB_ID;

		CRaptorLock lock(m_mutex);
		m_replies.push_back(rq);

		delete[] pWU;
	}

	return true;
}

bool CServerTransport::Process_JOB_WUNIT(request &rq)
{
	std::cout << "Rays Server receiving Work Unit client connection: " << rq.id << std::endl;

	//CServerSession::session_t session = m_sessionManager->getSession(rq.id);

	//bool bdata = false;
	//if (session.id == rq.id)
	//{
	//	uint8_t* raw_data = (uint8_t*)(rq.msg) + sizeof(MSGSTRUCT);
	//	bdata = m_sessionManager->saveSessionFile(rq.id, "RaysData.pck", raw_data, rq.size - sizeof(MSGSTRUCT));
	//}

	//! No reply, delete allocated bloc because processing ends here.
	delete[] rq.msg;

	return false;
}
