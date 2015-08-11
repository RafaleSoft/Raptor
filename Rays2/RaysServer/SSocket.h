// SSocket.h: interface for the CSSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSOCKET_H__082B5767_2740_4F82_A2C6_05769C837AA6__INCLUDED_)
#define AFX_SSOCKET_H__082B5767_2740_4F82_A2C6_05769C837AA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\RaysClientSocket.h"

class CSSocket : public CRaysClientSocket,public CServerSocket  
{
public:
	CServer<CSSocket>	*m_server;

public:
	CSSocket();
	virtual ~CSSocket();

	virtual void UserOutput(CString msg) const;

public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive( int nErrorCode );
	virtual void ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[]);
};

#endif // !defined(AFX_SSOCKET_H__082B5767_2740_4F82_A2C6_05769C837AA6__INCLUDED_)
