// WUSocket.h: interface for the CWUSocket class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_WUSOCKET_H__73F9E023_5890_4819_A657_F9B48C596BAA__INCLUDED_)
#define AFX_WUSOCKET_H__73F9E023_5890_4819_A657_F9B48C596BAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Systemdll\ClientSocket.h"
#include "..\RaysClientSocket.h"

class CWUSocket : public CRaysClientSocket,public CClientSocket  
{
public:
	CWUSocket();
	virtual ~CWUSocket();

	virtual void OnReceive( int nErrorCode );
	virtual void UserOutput(CString msg) const;
	virtual void ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[]);
};

#endif // !defined(AFX_WUSOCKET_H__73F9E023_5890_4819_A657_F9B48C596BAA__INCLUDED_)
