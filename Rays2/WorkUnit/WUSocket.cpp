// WUSocket.cpp: implementation of the CWUSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WorkUnit.h"
#include "WUSocket.h"
#include "Raytracer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWUSocket::CWUSocket()
{

}

CWUSocket::~CWUSocket()
{

}

void CWUSocket::UserOutput(CString msg) const
{
	((CWorkUnitApp*)AfxGetApp())->WriteMessage(msg);
}

#include "..\RaysClientSocket.cxx"

void CWUSocket::OnReceive( int nErrorCode )
{
	ReceiveMessage(this);

	CClientSocket::OnReceive(nErrorCode);
}

void CWUSocket::ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[])
{
	((CWorkUnitApp*)AfxGetApp())->ProcessMsg(msg,raw_data);
}
