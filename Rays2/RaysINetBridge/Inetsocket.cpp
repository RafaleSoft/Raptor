// Inetsocket.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RaysINetBridgeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInetSocket

CInetSocket::CInetSocket(CRaysINetBridgeDlg *bridge):
	m_pBride(bridge)
{
}

CInetSocket::~CInetSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CInetSocket, CSocket)
	//{{AFX_MSG_MAP(CInetSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CInetSocket member functions

void CInetSocket::OnReceive(int nErrorCode) 
{
	int res = Receive(buffer,1024);

	if (SOCKET_ERROR == res)
	{
		int error = GetLastError();
		CString msg;

		msg.Format("Receive error: ",error);
		m_pBride->Output(msg);
	}
	else
	{
		buffer[res] = 0;

		m_pBride->Output(CString("> ") + CString(buffer));
	}
		
	CSocket::OnReceive(nErrorCode);
}
