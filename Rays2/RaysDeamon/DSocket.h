// DSocket.h: interface for the CDSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSOCKET_H__54F0256D_D23C_4034_93D5_65F4CD8FE2EF__INCLUDED_)
#define AFX_DSOCKET_H__54F0256D_D23C_4034_93D5_65F4CD8FE2EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\RaysClientSocket.h"

class CDSocket : public CRaysClientSocket,public CServerSocket
{
public:
	CServer<CDSocket>	*m_server;

public:
	CDSocket();
	virtual ~CDSocket();

	virtual void UserOutput(CString msg) const;

public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive( int nErrorCode );
	virtual void ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[]);
};

#endif // !defined(AFX_DSOCKET_H__54F0256D_D23C_4034_93D5_65F4CD8FE2EF__INCLUDED_)
