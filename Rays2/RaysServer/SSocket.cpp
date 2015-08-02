// SSocket.cpp: implementation of the CSSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysServer.h"
#include "SSocket.h"
#include <iostream.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CRaysServerApp Server;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSSocket::CSSocket()
{
}

CSSocket::~CSSocket()
{
}

void CSSocket::UserOutput(CString msg) const
{
	CString output;
	CListBox *pOutput;

	if (Server.m_pMainWnd != NULL)
	{
		pOutput = (CListBox*)(Server.m_pMainWnd->GetDlgItem(IDC_LIST1));
	
		output.LoadString(IDS_CORE_STRING);
		output+=msg;
		if (pOutput!=NULL)
			pOutput->InsertString(0,output);
	}
}

void CSSocket::OnAccept(int /*nErrorCode*/) 
{
	int newClient = m_server->ProcessAccept();
	
	iosock_t<CSSocket> *io;
	if (newClient>=0)
	{
		m_server->GetClient(newClient,io);
		Server.AddNewConnection(io);
	}

//	Not necessary : all job is done here
//	CServerSocket::OnAccept(nErrorCode);
}

void CSSocket::OnClose(int nErrorCode)
{
	if (nErrorCode == WSAENETDOWN)
		UserOutput("Network subsystem failed");
	else if (nErrorCode == WSAECONNRESET)
		UserOutput("Remote client has reset connection");
	else if (nErrorCode == WSAECONNABORTED)
		UserOutput("Remote connection timed out");
	else if (nErrorCode != 0)
		UserOutput("Client closed connection unexpectedly");

	if (nErrorCode != 0)
	{
		iosock_t<CSSocket> *io = NULL;
		m_server->GetClient(this,io);

		if (io != NULL)
			Server.CloseConnection(io);
	}

//	Not necessary : all job is done here
//	CServerSocket::OnClose(nErrorCode);

	m_server->ProcessClose(this);
}

#include "..\RaysClientSocket.cxx"

void CSSocket::OnReceive( int nErrorCode )
{
	ReceiveMessage(this);

	CServerSocket::OnReceive(nErrorCode);
}

void CSSocket::ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[])
{
	CString str;
	str.Format("Socket received msg: 0x%x",msg.msg_id);
	UserOutput(str);

	Server.ManageMsg(msg,raw_data);
}
