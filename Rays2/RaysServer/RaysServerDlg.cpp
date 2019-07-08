/***************************************************************************/
/*                                                                         */
/*  RaysServerDlg.cpp                                                      */
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


#include <windows.h>
#include "RaysServerDlg.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif

#include "ServerTransport.h"
#include "DeamonManager.h"

using namespace RaysServer;

RaysServerDlg::RaysServerDlg()
	:m_started(false),m_pTransport(NULL),m_pDeamonManager(NULL)
{
#ifdef WIN32
	Network::setNbConnectAttempts(1);
	if (!Network::initSocketLayer())
		AddLog("Network layer not initialized properly.");
#endif
	RaysServerUtils::setLog(gcnew RaysServerDlg::RaysLogger(this));

	m_pTransport = new CServerTransport();
	m_pDeamonManager = new CDeamonManager(m_pTransport);
	if (m_pDeamonManager->getNbDeamons() > 0)
		AddLog("Server initialized.");
	else
		AddLog("Server not ready, no Work Units registered !");
}

RaysServerDlg::~RaysServerDlg()
{
	if (NULL != m_pTransport)
		delete m_pTransport;
	if (NULL != m_pDeamonManager)
		delete m_pDeamonManager;
}

bool RaysServerDlg::Start(int argc,char *argv[])
{
	if (!m_started)
	{
		std::string addrStr = "127.0.0.1";
		unsigned short port = 2048;

		CCmdLineParser parser;
		parser.addOption("port","p",port);
		parser.addOption("host_addr","a",addrStr);
		parser.parse(argc,argv);

		parser.getValue("port",port);
		parser.getValue("host_addr",addrStr);
		if (m_pTransport->startServer(addrStr,port))
			m_started = !m_started;
		if (m_started)
			AddLog("Server started. Listening ...");
		else
			AddLog("Server unable to start !");
	}
	else
	{
		if (m_pTransport->stopServer())
			m_started = !m_started;
		if (!m_started)
			AddLog("Server stopped.");
		else
			AddLog("Server unable to stop !");
	}

	return m_started;
}


bool RaysServerDlg::Quit(void)
{
	return !m_started;
}
