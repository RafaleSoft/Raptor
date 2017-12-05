// CltSocket.h: interface for the CCltSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLTSOCKET_H__B12A5F72_0493_49A6_80D3_E5A02B01B16F__INCLUDED_)
#define AFX_CLTSOCKET_H__B12A5F72_0493_49A6_80D3_E5A02B01B16F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Systemdll\ClientSocket.h"
#include "..\RaysClientSocket.h"

class CMiniRaysClientDlg;
class CCltSocket : public CRaysClientSocket,public CClientSocket
{
public:
	CCltSocket();
	virtual ~CCltSocket();

	virtual void OnReceive( int nErrorCode );
	virtual void OnClose(int nErrorCode);
	virtual void UserOutput(CString msg) const;
	virtual void ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[]);

public:
	CMiniRaysClientDlg	*output;
};

#endif // !defined(AFX_CLTSOCKET_H__B12A5F72_0493_49A6_80D3_E5A02B01B16F__INCLUDED_)
