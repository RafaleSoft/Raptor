#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
#define AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "Server.h"
#endif

using std::string;

class RAPTORNETWORK_API CClientSocket : public iosock_base_t
{
public:
	CClientSocket() {};
	CClientSocket(const iosock_base_t&);
	virtual ~CClientSocket() {};

	//!	Compare connections
	bool operator==(const iosock_base_t& rsh) const;

	//! Implements connect
	virtual bool connect(const std::string& address,unsigned int port);

	//! Implements accept
	virtual bool connect(const iosock_base_t& rsh);

	//!	Implement data received
	virtual bool onDataReceived(const void *data,size_t size);

	//! Implement on reply request
	virtual bool onReplyRequest(const void *&data,size_t &size);

private:
	static	size_t	readBufferSize;
	static	size_t	writeBufferSize;
};

#endif // !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
