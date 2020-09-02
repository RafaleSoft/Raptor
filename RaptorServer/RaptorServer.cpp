/***************************************************************************/
/*                                                                         */
/*  RaptorServer.cpp                                                       */
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

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
    #include "RaptorServer.h"
#endif
#if !defined(AFX_RAPTORSERVERINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
    #include "RaptorServerInstance.h"
#endif
#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
    #include "ServerTransport.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorServer::CRaptorServer()
{
    m_pInstance = NULL;
    m_pTransport = NULL;
}

CRaptorServer::~CRaptorServer()
{
	//! Server should have been stopped at earlier stage.
	Stop();

	if (m_pInstance != NULL)
	{
		m_pInstance->stop();
		delete m_pInstance;
	}

	std::cout << "Raptor Server exit. " << std::endl;
}


bool CRaptorServer::Start(const CCmdLineParser& cmdline)
{
    if (m_pInstance == NULL)
		m_pInstance = CRaptorServerInstance::GetInstance();

	unsigned short width = 256;
	unsigned short height = 256;
	cmdline.getValue("width",width);
	cmdline.getValue("height",height);

	bool res = m_pInstance->start(width,height);
    if (res)
		std::cout << "Raptor Renderer initialized. " << std::endl;
    else
		std::cout << "Failed to initialize Raptor. " << std::endl;


	if (m_pTransport == NULL)
        m_pTransport = new CServerTransport();
	
	string addrStr = "127.0.0.1";
	unsigned short port = 2048;
	cmdline.getValue("port",port);
	cmdline.getValue("host_addr",addrStr);
	res = m_pTransport->startServer(addrStr,port);
    if (res)
    {
		stringstream str;
		str << "Raptor Server ready on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(),str.str());
		std::cout << str.str() << std::endl;
    }
    else
    {
		stringstream str;
		str << "Raptor Server couldn't be started on port ";
		str << port;
		str << " at host ";
		str << addrStr;
		RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),str.str());
		std::cout << str.str() << std::endl;
    }

    return m_pInstance->run();
}

bool CRaptorServer::Stop(void)
{
	if (NULL == m_pTransport)
		return true;	// already stopped

	bool stopped = true;
	if (m_pTransport->stopServer())
	{
		delete m_pTransport;
		m_pTransport = NULL;
	}
	else
		stopped = false;

	if (stopped)
		std::cout << "Raptor Renderer stopped. " << std::endl;
	else
		std::cout << "Raptor Renderer failed to stop. " << std::endl;

	return stopped;
}

bool CRaptorServer::CloseWindow()
{
	if (NULL != m_pInstance)
		return m_pInstance->closeWindow();
	else
		return true;
}

