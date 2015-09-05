#ifndef __RAYS_CLIENT_SOCKET_H__
#include "RaysClientSocket.h"
#endif

void CRaysClientSocket::ReceiveMessage( CSocket* s )
{
	MSGSTRUCT msg;

	int r = s->Receive( &msg, MSGSIZE);	// receive header first
	
	if (r == SOCKET_ERROR)
	{
		UserOutput("Server connection subsystem error: Receive failed");
	}
	else if (r<MSGSIZE)
	{
		UserOutput("Io link with server was broken");
	}
	else
	{
		unsigned char *buffer=NULL;
		CString msgStr;
		int nRead = 0;

		if (msg.msg_size>0)
		{
			if (msg.msg_tail==MSG_DATA)
			{
				// Buffer is destroyed by the functions that
				//	processes this message.
				buffer = new unsigned char[msg.msg_size];
			
				r = 0;
				if (msg.msg_size > PACKET_SIZE)
				{
					bool failed = false;
					
					while ((!failed) && (r + PACKET_SIZE < (int)msg.msg_size))
					{
						nRead = s->Receive(buffer + r, PACKET_SIZE);

						if (nRead == SOCKET_ERROR)
						{
							UserOutput("Server connection subsystem error: Receive failed");
							failed = true;
						}
						else
							r += nRead;
					}

					if (r < (int)msg.msg_size)
						r += s->Receive(buffer + r, msg.msg_size - r);
				}

				nRead = 0;
				while ((nRead != SOCKET_ERROR) && (r < (int)msg.msg_size))
				{
					nRead = s->Receive(buffer + r, msg.msg_size - r);	// receive small data

					if (nRead == SOCKET_ERROR)
						UserOutput("Server connection subsystem error: Receive failed");
					else
						r += nRead;
				}

				CString str;
				if (r == (int)msg.msg_size)
				{
					str.Format("Data received : %d bytes",r);
				}
				else
				{
					str.Format("Message data truncated : %d < %d",r,msg.msg_size);
				}
				UserOutput(str);
			}
			else		
				UserOutput("Invalid message tail");
		}
		if (
			(msg.msg_header==MSG_START)&&
			((msg.msg_tail==MSG_END)||(msg.msg_tail==MSG_DATA))
			)
			ProcessMsg(msg,buffer);
		else
			UserOutput("Unrecognised message format");
	}
}