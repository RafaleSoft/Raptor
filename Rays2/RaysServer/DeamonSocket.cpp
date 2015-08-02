// DeamonSocket.cpp: implementation of the CDeamonSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysServer.h"
#include "DeamonSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeamonSocket::CDeamonSocket()
{

}

CDeamonSocket::~CDeamonSocket()
{

}

void CDeamonSocket::UserOutput(CString msg) const
{
	Server.WriteMessage(IDS_DEAMON_STRING,msg);
}

void CDeamonSocket::OnReceive( int nErrorCode )
{
	unsigned char *buffer=NULL;
	CString msgStr;
	MSGSTRUCT msg;

	int r = Receive( &msg, MSGSIZE);	// receive header first
	
	if (r<MSGSIZE)
		UserOutput("Io link with deamon was broken");
	else
	{
		if (msg.msg_size>0)
			if (msg.msg_tail==MSG_DATA)
			{
				buffer = new unsigned char[msg.msg_size];
				r = Receive( buffer, msg.msg_size);	// receive data
				if (r<(int)msg.msg_size)
					UserOutput("message data truncated");
			}
			else
				UserOutput("Invalid message tail");

		if (
			(msg.msg_header==MSG_START)&&
			((msg.msg_tail==MSG_END)||(msg.msg_tail==MSG_DATA))
			)
			Server.ManageMsg(msg,buffer);
			//Server.ProcessMsg(msg,buffer);
		else
			UserOutput("Unrecognised message format from Deamon");
	}
	
	CSocket::OnReceive(nErrorCode);
}

void CDeamonSocket::OnClose(int /*nErrorCode*/)
{
	UserOutput("Deamon terminated unexpectedly...");

	CString sname;
	unsigned int port;

	GetSockName(sname,port);

	const CDeamonManager * const pDeamon = Server.GetDeamonManager();
	pDeamon->DisconnectDeamon(sname,port);
}