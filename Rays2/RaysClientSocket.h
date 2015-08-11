#ifndef __RAYS_CLIENT_SOCKET_H__
#define __RAYS_CLIENT_SOCKET_H__

class CSocket;

class CRaysClientSocket
{
public:
	CRaysClientSocket() {};
	virtual ~CRaysClientSocket() {};

	virtual void ReceiveMessage( CSocket* s );

	//	The implementation of this method is responsible
	//	of the destruction of the allocated buffer raw_data
	//	to allow delayed multithread message processing
	virtual void ProcessMsg(MSGSTRUCT msg,unsigned char raw_data[]) = 0;

	virtual void UserOutput(CString msg) const = 0;
};

#endif