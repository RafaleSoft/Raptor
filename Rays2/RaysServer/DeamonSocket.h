// DeamonSocket.h: interface for the CDeamonSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEAMONSOCKET_H__5ADCFE05_B9D0_439E_823B_03E58A0FDA90__INCLUDED_)
#define AFX_DEAMONSOCKET_H__5ADCFE05_B9D0_439E_823B_03E58A0FDA90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\systemdll\systemdll.h"

class CDeamonSocket : public CClientSocket  
{
public:
	CDeamonSocket();
	virtual ~CDeamonSocket();

	virtual void OnReceive( int nErrorCode );
	virtual void UserOutput(CString msg) const;

	virtual void OnClose(int nErrorCode);
};

#endif // !defined(AFX_DEAMONSOCKET_H__5ADCFE05_B9D0_439E_823B_03E58A0FDA90__INCLUDED_)
