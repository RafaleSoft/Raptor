// RaptorServer.cpp: implementation of the CRaptorServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if !defined(AFX_SERVERCMDLINE_H__3FAF402E_EBC9_4BEF_B369_8D2CBD677141__INCLUDED_)
    #include "ServerCmdLine.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
    #include "RaptorServer.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
    #include "RaptorInstance.h"
#endif
#if !defined(AFX_SERVERTRANSPORT_H__AC7E8C33_37A1_4BE2_8B73_B463DA99E328__INCLUDED_)
    #include "ServerTransport.h"
#endif

using std::cout;
using std::endl;

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
    if (m_pInstance != NULL)
        delete m_pInstance;
}


bool CRaptorServer::Start(const ServerCmdLine& cmdline)
{
    if (m_pInstance == NULL)
        m_pInstance = CRaptorInstance::getInstance();

	bool res = m_pInstance->start(cmdline.width,cmdline.height);
    if (res)
    {
		RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(),
						"Raptor Renderer initialized. ");
    }
    else
    {
		RAPTOR_FATAL(	CPersistence::CPersistenceClassID::GetClassId(),
						"Failed to initialize Raptor. ");
    }


	if (m_pTransport == NULL)
        m_pTransport = new CServerTransport();

	res = m_pTransport->startServer(cmdline.addrStr,cmdline.port);
    if (res)
    {
		stringstream str;
		str << "Raptor Server ready on port ";
		str << cmdline.port;
		str << " at host ";
		str << cmdline.addrStr.data();
		RAPTOR_NO_ERROR(CPersistence::CPersistenceClassID::GetClassId(),str.str());
    }
    else
    {
		stringstream str;
		str << "Raptor Server couldn't be started on port ";
		str << cmdline.port;
		str << " at host ";
		str << cmdline.addrStr.data();
		RAPTOR_FATAL(CPersistence::CPersistenceClassID::GetClassId(),str.str());
    }

    return m_pInstance->run();
}

bool CRaptorServer::Stop(void)
{
	if (m_pTransport->stopServer())
	{
		delete m_pTransport;
		m_pTransport = NULL;
		return  true;
	}
	else
		return false;
}


