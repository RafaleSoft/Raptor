// DSocket.cpp: implementation of the CDSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysDeamon.h"
#include "DSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSocket::CDSocket()
{

}

CDSocket::~CDSocket()
{

}

void CDSocket::UserOutput(CString msg) const
{

	if (Deamon.m_pMainWnd != NULL)
	{
		CWnd *wnd = Deamon.m_pMainWnd->GetDlgItem(IDC_DEAMON_INFO);
	
		if (wnd != NULL)
			wnd->SetWindowText(msg);
	}
}

void CDSocket::OnAccept(int /*nErrorCode*/) 
{	
	if (Deamon.m_RaysServer == NULL)
	{
		int newClient = m_server->ProcessAccept();
		if (newClient>=0)
		{
			m_server->GetClient(newClient,Deamon.m_RaysServer);
		}
	}

//	Not necessary : all job is done here
//	CServerSocket::OnAccept(nErrorCode);
}

void CDSocket::OnClose(int nErrorCode)
{
	if (nErrorCode == WSAENETDOWN)
		UserOutput("Network subsystem failed");
	else if (nErrorCode == WSAECONNRESET)
		UserOutput("Remote client has reset connection");
	else if (nErrorCode == WSAECONNABORTED)
		UserOutput("Remote connection timed out");
	else if (nErrorCode != 0)
		UserOutput("Client closed connection unexpectedly");

	if (m_server->ProcessClose(this))
		Deamon.m_RaysServer = NULL;
	else
		UserOutput("Failed to shutdown Server connection");

//	Not necessary : all job is done here
//	CServerSocket::OnClose(nErrorCode);
}

#include "..\RaysClientSocket.cxx"

void CDSocket::OnReceive( int nErrorCode )
{
	ReceiveMessage(this);

	CServerSocket::OnReceive(nErrorCode);
}

void CDSocket::ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[])
{
	Deamon.ManageMsg(msg,raw_data);
}
