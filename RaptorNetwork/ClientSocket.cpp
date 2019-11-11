// ClientSocket.cpp : implementation file
//

#include "stdafx.h"

#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "ClientSocket.h"
#endif

#include <iostream>		// cout

size_t	CClientSocket::readBufferSize = 65536;
size_t	CClientSocket::writeBufferSize = 2048;


/////////////////////////////////////////////////////////////////////////////
// CClientSocket
CClientSocket::CClientSocket(const iosock_base_t& sock)
	:iosock_base_t(sock)
{
}

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions
bool CClientSocket::connect(const std::string& address,unsigned short port)
{
	m_port = port;

	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket)
	{
		Network::userOutput(INetworkLogger::NETWORK_ERROR,
							Network::networkErrors("Client socket create failed"));
		return false;
	}

	bool b = true;
	if (0 != ::setsockopt(m_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&b,sizeof(bool)))
	{
		Network::userOutput(INetworkLogger::NETWORK_WARNING,
							Network::networkErrors("Client socket tcp_nodelay failed"));
		close();
		return false;
	}


	size_t tmpRead = 0;
	bool reportError = (0 != setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&readBufferSize,sizeof(size_t)));
	if (!reportError)
	{
#ifdef WIN32
		int tmpSize = sizeof(int);
#else
		unsigned int tmpSize = sizeof(unsigned int);
#endif
		reportError = (0 != getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&tmpRead,&tmpSize));
	}

	size_t tmpWrite = 0;
	reportError = reportError && (0 != setsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char *)&writeBufferSize,sizeof(size_t)));
	if (!reportError)
	{
#ifdef WIN32
		int tmpSize = sizeof(int);
#else
		unsigned int tmpSize = sizeof(unsigned int);
#endif
		if (0 != getsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char *)&tmpWrite,&tmpSize))
			Network::userOutput(INetworkLogger::NETWORK_WARNING,
								Network::networkErrors("Client socket failed to update write buffer size"));
	}

	if (reportError || (tmpRead != readBufferSize) || (tmpWrite != writeBufferSize))
		Network::userOutput(INetworkLogger::NETWORK_WARNING,
							Network::networkErrors("Client socket failed to update read/write buffer size"));

	sockaddr_in s_in;
	s_in.sin_family			= AF_INET;
	s_in.sin_port			= htons(m_port);

	if ((address.length()>0) && isalpha((unsigned char)address[0]))
	{
		struct hostent *remoteHost = gethostbyname(address.c_str());
		if (NULL != remoteHost)
			s_in.sin_addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
		else
		{
			close();
			return false;
		}
	}
	else
		s_in.sin_addr.s_addr	= inet_addr(address.c_str());

	if (SOCKET_ERROR == ::connect(m_socket,(sockaddr*) &s_in,sizeof(s_in)))
	{
		Network::userOutput(INetworkLogger::NETWORK_ERROR,
							Network::networkErrors("Client socket connect failed"));
		close();
		return false;
	}

	return true;
}

bool CClientSocket::connect(const iosock_base_t& rsh)
{
	sockaddr_in csin;
#ifdef WIN32
	int sinsize = sizeof(csin);
#else	// Linux environment
	unsigned int sinsize = sizeof(csin);
#endif

	CClientSocket cs(rsh);

	if ((m_socket = ::accept(cs.m_socket, (sockaddr*)&csin, &sinsize)) != INVALID_SOCKET)
	{
		m_port = ntohs(csin.sin_port);
		m_ip = csin.sin_addr.s_addr;
		m_pServer = cs.m_pServer;
		return true;
	}
	else
	{
		m_socket = 0;
		return false;
	}
}

bool CClientSocket::operator==(const iosock_base_t& rsh) const
{
	return (rsh == *this);
}

bool CClientSocket::onDataReceived(const void *data,size_t size)
{
	if (getServer() != NULL)
	{
		server_base_t::request_handler_t &handler = getServer()->getRequestHandler(*this);
		return handler.handleRequest(this,data,size);
	}
	else
		return false;
}

bool CClientSocket::onReplyRequest(const void *&data,size_t &size)
{
	if (getServer() != NULL)
	{
		server_base_t::request_handler_t &handler = getServer()->getRequestHandler(*this);
		return handler.handleReply(this,data,size);
	}
	else
		return false;
}
