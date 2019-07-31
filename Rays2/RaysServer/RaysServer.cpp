/***************************************************************************/
/*                                                                         */
/*  RaysServer.cpp                                                         */
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

#if defined(WIN32)
	#define VC_EXTRALEAN
	#include <Windows.h>
#endif


#if !defined(AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_)
	#include "RaysServer.h"
#endif
#if !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "RaysServerUtils.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
	#include "ServerTransport.h"
#endif
#if !defined(AFX_DEAMONMANAGER_H__F7EF715A_5E86_4C65_B6E7_2751FAE87A91__INCLUDED_)
	#include "DeamonManager.h"
#endif


RAPTOR_NAMESPACE
using namespace RaysServer;


class CRaysLogger : public RaysServer::RaysServerUtils::ILogger
{
public:
	CRaysLogger() :ILogger() {};
	virtual ~CRaysLogger() {};
	virtual void Log(const std::string& msg);
};

void CRaysLogger::Log(const std::string& msg)
{
	std::cout << msg << std::endl;
}

RaysServer::CRaysServerApp::CRaysServerApp()
	:m_started(false), m_counter(0), m_globalJobDone(0.0f),
	m_pTransport(NULL), m_pDeamonManager(NULL), m_msgManager(NULL),
	m_fileManager(NULL), m_nbWUperJOB(0), m_wUnitPriority(0),
	m_deamonDelay(0), m_nbProcs(0), m_Server(NULL)
{
	CRaysLogger *plogger = new CRaysLogger();
	RaysServerUtils::ILogger *ilogger = plogger;
	
	RaysServerUtils::setLog(ilogger);

	m_baseFrequency.QuadPart = 0;
}

bool RaysServer::CRaysServerApp::Quit(void)
{
	if (m_pTransport->stopServer())
		m_started = !m_started;
	if (!m_started)
		RaysServerUtils::getLog().Log("Server stopped.");
	else
		RaysServerUtils::getLog().Log("Server unable to stop !");

	if (NULL != m_pTransport)
		delete m_pTransport;
	if (NULL != m_pDeamonManager)
		delete m_pDeamonManager;

	return true;
}

bool RaysServer::CRaysServerApp::Start(const std::string &addrStr, uint16_t port)
{
	if (NULL == m_pTransport)
		m_pTransport = new CServerTransport();

	if (NULL == m_pDeamonManager)
		m_pDeamonManager = new CDeamonManager(m_pTransport);
	if (m_pDeamonManager->getNbDeamons() > 0)
		RaysServerUtils::getLog().Log("Server initialized.");
	else
		RaysServerUtils::getLog().Log("Server not ready, no Work Units registered !");

	bool res = m_pTransport->startServer(addrStr, port);
	if (NULL != getDeamonManager())
	{
		const CRaysettings &settings = RaysServerUtils::getSettings();
		uint32_t delay = 0;
		if (settings.getValue("deamon_delay", delay))
			getDeamonManager()->setPollingDelay(delay);
	}

	return true;
}

int main(int argc, char* argv[])
{
	unsigned long v = Raptor::GetVersion();
	stringstream title;
	title << "Raptor Rays Server ";
	title << ((v >> 24) & 0xFF) << "." << ((v >> 16) & 0xFF) << "." << ((v >> 8) & 0xFF);
	title << " Release test";

	RaysServer::CRaysServerApp *pApp = new RaysServer::CRaysServerApp();
	if (!RaysServerUtils::loadConfig())
		RaysServerUtils::getLog().Log("Rays Server configuration file failed to load.");

	Network::setNbConnectAttempts(1);
	if (!Network::initSocketLayer())
		RaysServerUtils::getLog().Log("Network layer not initialized properly.");

	CCmdLineParser parser;
	parser.addOption("port", "p", (uint16_t)2048);
	parser.addOption("width", "w", (uint16_t)256);
	parser.addOption("height", "h", (uint16_t)256);
	parser.addOption("host_addr", "a", std::string("127.0.0.1"));
	parser.parse(argc, argv);

	std::string addrStr = "127.0.0.1";
	uint16_t port = 2048;
	parser.getValue("port", port);
	parser.getValue("host_addr", addrStr);

	bool res = pApp->Start(addrStr, port);
	if (res)
	{
		stringstream str;
		str << "Raptor Server ready on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(), str.str());
	}
	else
	{
		stringstream str;
		str << "Raptor Server couldn't be started on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(), str.str());
	}

	return 0;
}

