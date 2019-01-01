// CltSocket.cpp: implementation of the CCltSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniRaysClient.h"
#include "MiniRaysClientDlg.h"
#include "CltSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCltSocket::CCltSocket()
{
	output = miniRaysClient.output;
}

CCltSocket::~CCltSocket()
{

}

void CCltSocket::UserOutput(CString msg) const
{
	output->Output("# System message: " + msg);
}

#include "..\RaysClientSocket.cxx"

void CCltSocket::OnReceive( int nErrorCode )
{
	ReceiveMessage(this);

	CClientSocket::OnReceive(nErrorCode);
}

void CCltSocket::ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[])
{
	switch(msg.msg_id)
	{
		case JOB_STATUS:
		{
			miniRaysClient.Process_JOB_STATUS(msg,raw_data);
			break;
		}
		case ACK_NONE:
		{
			UserOutput("Image transfer failed !");
		}
		case ACK_IMG:
		{
			UserOutput("Image query successfull");
		}
		case ACK_JOB:
		case ACK_OBJ:
		case JOB_ID:
		{
			miniRaysClient.Process_ACKNOWLEDGE(msg,raw_data);
			break;
		}
		default:
		{
			UserOutput("Unsupported message id");
			break;
		}
	}

	if (raw_data != NULL)
		delete [] raw_data;
}

void CCltSocket::OnClose(int nErrorCode)
{
	UserOutput("Server closed connection unexpectedly !");

	CClientSocket::OnClose(nErrorCode);
}