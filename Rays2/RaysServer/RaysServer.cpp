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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_)
	#include "RaysServer.h"
#endif
#if !defined(AFX_RAYSUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "../RaysUtils.h"
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
using namespace Rays;

static RaysServer::CRaysServerApp *pApp = NULL;


class CRaysLogger : public Rays::RaysUtils::ILogger
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
	:m_started(false), m_bExit(false), m_counter(0), m_globalJobDone(0.0f),
	m_pTransport(NULL), m_pDeamonManager(NULL), m_msgManager(NULL),
	m_fileManager(NULL), m_nbWUperJOB(0), m_wUnitPriority(0),
	m_deamonDelay(0)
{
	CRaysLogger *plogger = new CRaysLogger();
	RaysUtils::ILogger *ilogger = plogger;
	
	RaysUtils::setLog(ilogger);

	m_baseFrequency.QuadPart = 0;
}

bool RaysServer::CRaysServerApp::Quit(void)
{
	if (NULL != m_pDeamonManager)
	{
		m_pDeamonManager->requestExit();
		for (size_t i = 0; i < m_pDeamonManager->getNbDeamons(); i++)
			m_pDeamonManager->unregisterDeamon(i);

		delete m_pDeamonManager;
	}

	if (m_pTransport->stopServer())
		m_started = !m_started;
	if (!m_started)
	{
		RaysUtils::getLog().Log("Server stopped.");
		if (NULL != m_pTransport)
			delete m_pTransport;
	}
	else
		RaysUtils::getLog().Log("Server unable to stop !");

	return !m_started;
}

bool RaysServer::CRaysServerApp::Start(const std::string &addrStr, uint16_t port)
{
	if (m_started)
		return true;

	if (NULL == m_pTransport)
		m_pTransport = new CServerTransport();

	if (NULL == m_pDeamonManager)
		m_pDeamonManager = new CDeamonManager(m_pTransport);
	
	bool res = m_pTransport->startServer(addrStr, port);
	if (!res)
	{
		RaysUtils::getLog().Log("Server network cannot connect.");
		return false;
	}
	
	const CRaysSettings &settings = RaysUtils::getSettings();
	
	uint16_t deamon_port = port + 1;
	std::vector<std::string> ips;
	if (settings.getValue("deamon", ips) && settings.getValue("deamon_port", port))
	{
		for (size_t i = 0; i < ips.size(); i++)
			res = res && m_pDeamonManager->registerDeamon(ips[i], deamon_port);
	}

	if (m_pDeamonManager->getNbDeamons() == ips.size())
		RaysUtils::getLog().Log("Server initialized.");
	else
		RaysUtils::getLog().Log("Server not ready, some Work Units are not registered !");

	uint32_t delay = 0;
	if (settings.getValue("deamon_delay", delay))
		m_pDeamonManager->Start(delay);

	m_started = true;
	return res;
}


BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal.
		case CTRL_C_EVENT:
		{
			RaysUtils::getLog().Log("Rays Server user exit requested by Ctrl-C. Exiting, bye!");
			pApp->requestExit();
			return(TRUE);
			break;
		}
		// CTRL-CLOSE: confirm that the user wants to exit.
		case CTRL_CLOSE_EVENT:
		{
			RaysUtils::getLog().Log("Rays Server user exit requested by Ctrl-close. Exiting, bye!");
			pApp->requestExit();
			return(TRUE);
			break;
		}
		// Pass other signals to the next handler.
		case CTRL_BREAK_EVENT:
			return FALSE;
		case CTRL_LOGOFF_EVENT:
			return FALSE;
		case CTRL_SHUTDOWN_EVENT:
			return FALSE;
		default:
			return FALSE;
	}
}

void print_help(void)
{
	std::cout << "Rays Server command line help:" << std::endl;

	std::cout << "  --port|-p : defines the server listening port, by default 2048" << std::endl;
	std::cout << "  --host_addr|-a : defines the server listening IP address, by default 127.0.0.1" << std::endl;
	std::cout << "  --config_file|-f : the path to the deamon configuration file, by default RaysDeamon.config in the current execution folder" << std::endl;
	std::cout << "  --wu_priority|-u : the work unit execution priority when multiple jobs are in progress" << std::endl;
	std::cout << "  --deamon_delay|-t : the polling delay to the deamon" << std::endl;
	std::cout << "  --nb_wu_per_job|-j : the number of work units allocated for each job" << std::endl;
	std::cout << "  --deamon|-d : the host address of a workunit deamon manager. Any number of deamons can be provided" << std::endl;
	std::cout << "  --deamon_port|-P : the deamons (same for all deamons) listening port, by default 2049" << std::endl;
	std::cout << "  --help|-h : print this help and quit" << std::endl;
	std::cout << std::endl;
}


/////////////////////////////////////////////////////////////////////////////
//	Entry point
int main(int argc, char* argv[])
{
	std::cout << std::endl;
	std::cout << "        Raptor Rays Server.        " << std::endl;
	std::cout << "        version: " << RAPTOR_VERSION_STR << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	std::cout << std::endl;

	//!	 Initialise the one and only application.
	pApp = new RaysServer::CRaysServerApp();

	CCmdLineParser parser;
	parser.addOption("port", "p", (unsigned short)2048);
	parser.addOption("deamon_port", "P", (unsigned short)2049);
	parser.addOption("host_addr", "a", std::string("127.0.0.1"));
	parser.addOption("config_file", "f", std::string("RaysServer.config"));
	parser.addOption("help", "h", CCmdLineParser::NO_VALUE_OPTION);
	if (!parser.parse(argc, argv))
	{
		RaysUtils::getLog().Log("Server failed to parse command line. Exiting, bye!");
		return -1;
	}

	CCmdLineParser::NO_VALUE_OPTION_t help = CCmdLineParser::NO_VALUE_UNDEFINED;
	parser.getValue<CCmdLineParser::NO_VALUE_OPTION_t>("h", help);
	if (CCmdLineParser::NO_VALUE_VALUE == help)
	{
		print_help();
		return 0;
	}

	//!	Initialise settings from command line.
	CRaysSettings &settings = RaysUtils::getSettings();
	settings.setSettings(parser);


	//!	Initalise network layer.
	Network::setNbConnectAttempts(1);
	if (!Network::initSocketLayer())
	{
		RaysUtils::getLog().Log("Network layer not initialized properly.");
		return -1;
	}

	//!	Initialise settings from config file if provided or exist.
	std::string config_file = "";
	parser.getValue("config_file", config_file);
	if (!config_file.empty())
	{
		RaysUtils::getLog().Log("Rays Server will read and replace values from config_file: " + config_file);
		if (!RaysUtils::loadConfig(config_file))
			RaysUtils::getLog().Log("Rays Server configuration file failed to load.");
	}

	//! Minimal settings to start Rays Server : port and IP address.
	std::string addrStr = "127.0.0.1";
	uint16_t port = 2048;
	if (!settings.getValue("port", port) || !settings.getValue("host_addr", addrStr))
	{
		RaysUtils::getLog().Log("Impossible to know Rays Server settings to use. Exiting");
		return -1;
	}

	//! Run Rays server.
	bool res = pApp->Start(addrStr, port);
	if (res)
	{
		stringstream str;
		str << "Rays Server ready on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RaysUtils::getLog().Log(str.str());

		if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
			RaysUtils::getLog().Log("Failed to install server crtl handler!");

		while (!pApp->doExit())
			Sleep(500);
		res = pApp->Quit();
	}
	else
	{
		stringstream str;
		str << "Rays Server couldn't be started on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RaysUtils::getLog().Log(str.str());
	}
	
	delete pApp;

	std::cout << "Rays Server exiting with code " << res << ". Bye!" << std::endl;
	return (res ? 0 : 1);
}

