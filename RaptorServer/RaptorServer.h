// RaptorServer.h: interface for the CRaptorServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
#define AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRaptorServerInstance;
class CServerTransport;
class CCmdLineParser;


class CRaptorServer
{
public:
	CRaptorServer();
	virtual ~CRaptorServer();

    bool Start(	const CCmdLineParser& cmdline );
	
    bool Stop(void);

private:
	CRaptorServerInstance *m_pInstance;
    CServerTransport *m_pTransport;
};

#endif // !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
