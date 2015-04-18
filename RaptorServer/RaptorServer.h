// RaptorServer.h: interface for the CRaptorServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
#define AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRaptorInstance;
class CServerTransport;
class ServerCmdLine;


#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

RAPTOR_NAMESPACE


class CRaptorServer
{
public:
	CRaptorServer();
	virtual ~CRaptorServer();

    bool Start(	const ServerCmdLine& cmdline );
	
    bool Stop(void);

private:
    CRaptorInstance *m_pInstance;
    CServerTransport *m_pTransport;
};

#endif // !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
