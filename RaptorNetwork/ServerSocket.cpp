// ServerSocket.cpp : implementation file
//

#include "stdafx.h"

#if !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "ServerSocket.h"
#endif
#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "ClientSocket.h"
#endif

#include <sstream>

size_t	CServerSocket::readBufferSize = 2048;
size_t	CServerSocket::writeBufferSize = 65536;
struct timeval iosock_collection_t::timeout;


#ifdef WIN32
	DWORD
#else
	void*
#endif
waitConnectionEvent(void* pParam)
{
	CServerSocket *pIOSock = (CServerSocket*)pParam;
	//CServer<CServerSocket,CClientSocket> *pServer = (CServer<CServerSocket,CClientSocket>*)pIOSock->getServer();
	server_base_t *pServer = pIOSock->getServer();

	while (pIOSock->getPort() != 0)
	{
		if (pIOSock->areReadable())
		{
			if (pIOSock->iosock_collection_t::isReadable(0))
				pServer->newClient();

			for (size_t i=1;i<pIOSock->getCount();i++)
			{
				if (pIOSock->iosock_collection_t::isReadable(i))
				{
					const iosock_base_t* io = pIOSock->getCollectionItem(i);
					pServer->readClient(const_cast<iosock_base_t&>(*io));
				}
			}
		}
	}

#ifdef WIN32
	ExitThread(0);
#else	// Linux environment
	pthread_exit(0);
#endif
}

iosock_collection_t::iosock_collection_t()
{
	timeout.tv_sec = 0;
	timeout.tv_usec = Network::getSelectTimeout();
}

bool iosock_collection_t::addToCollection(const iosock_base_t* io)
{
	size_t i=0;
	for (i=0;i<m_collection.size();i++)
	{
		const iosock_base_t* pIo = m_collection[i].iosock;
		if ((io == pIo) || (*io == *pIo))
			break;
	}

	if (i>= m_collection.size())
	{
		iosock_collection_item_t item;
		item.iosock = io;
		item.readable = false;
		item.writable = false;
		m_collection.push_back(item);
		return true;
	}
	else
		return false;
}

bool iosock_collection_t::removeFromCollection(const iosock_base_t* io)
{
	size_t i=0;
	for (i=0;i<m_collection.size();i++)
	{
		const iosock_base_t* pIo = m_collection[i].iosock;
		if ((io == pIo) || (*io == *pIo))
			break;
	}

	if (i < m_collection.size())
	{
		m_collection.erase(m_collection.begin() + i);
		return true;
	}
	else
		return false;
}

bool iosock_collection_t::areReadable(void)
{
	fd_set	readfds;
	FD_ZERO(&readfds);

	for (size_t i=0;i<m_collection.size();i++)
#ifdef WIN32
		readfds.fd_array[i] = m_collection[i].iosock->m_socket;
	readfds.fd_count = (u_int)m_collection.size();
#else // Linux environment
		FD_SET(m_collection[i].iosock->m_socket, &readfds);
#endif

	int res = select((int)m_collection.size(),&readfds,NULL,NULL,&timeout);

	if (res > 0)
		for (size_t i=0;i<m_collection.size();i++)
			m_collection[i].readable = (0 != FD_ISSET(m_collection[i].iosock->m_socket,&readfds));

	return (1 <= res);
}

bool iosock_collection_t::areWritable(void)
{
	fd_set	writefds;
	FD_ZERO(&writefds);

	for (size_t i=0;i<m_collection.size();i++)
#ifdef WIN32
		writefds.fd_array[i] = m_collection[i].iosock->m_socket;
	writefds.fd_count = (u_int)m_collection.size();
#else // Linux environment
		FD_SET(m_collection[i].iosock->m_socket, &writefds);
#endif

	int res = select((int)m_collection.size(),NULL,&writefds,NULL,&timeout);

	if (res > 0)
		for (size_t i=0;i<m_collection.size();i++)
			m_collection[i].writable = (0 != FD_ISSET(m_collection[i].iosock->m_socket,&writefds));
			//m_collection[i].readable = (readfds.fd_array[i] != 0);

	return (1 <= res);
}

CServerSocket::CServerSocket()
	:m_threadID(0), m_bConnected(false)
#ifdef WIN32
	,m_thread(NULL)
#endif
{
}

void CServerSocket::shutdown()
{
	iosock_base_t::shutdown();

	if (0 != m_threadID)
	{
#ifdef WIN32
		/*DWORD res = */WaitForSingleObject(m_thread,INFINITE);
		CloseHandle(m_thread);
		m_thread = NULL;
#else // Linux environment
		void* ret = NULL;
		pthread_join(m_threadID, &ret);
#endif
	}

	m_bConnected = false;

	m_threadID = 0;
}

bool CServerSocket::onDataReceived(const void *,size_t ) 
{ 
	return false; 
}

bool CServerSocket::onReplyRequest(const void *&,size_t &) 
{ 
	return false; 
}

bool CServerSocket::setBuffersSizes(size_t readBuffer,size_t writeBuffer)
{
	// relevant checks to perform ?
	if (readBuffer > 16)
		readBufferSize = readBuffer;
	if (writeBuffer > 16)
		writeBufferSize = writeBuffer;

	return true;
}

bool CServerSocket::connect(const std::string& address,unsigned short port)
{
	if (getServer() == NULL)
		return false;

	m_port = port;

	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket)
	{
		Network::userOutput(INetworkLogger::NETWORK_ERROR,
							Network::networkErrors("Server socket create failed"));
		return false;
	}

	bool b = true;
	if (0 != ::setsockopt(m_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&b,sizeof(bool)))
	{
		Network::userOutput(INetworkLogger::NETWORK_WARNING,
							Network::networkErrors("Server socket tcp_nodelay failed"));
		close();
		return false;
	}

	sockaddr_in s_in;
	s_in.sin_addr.s_addr	= Network::sockNameToAddr(address); // INADDR_ANY;
	s_in.sin_family			= AF_INET;
	s_in.sin_port			= htons(m_port);

	if (SOCKET_ERROR == bind(m_socket, (sockaddr*)&s_in, sizeof(s_in)))
	{
		Network::userOutput(INetworkLogger::NETWORK_ERROR,
							Network::networkErrors("Server socket bind failed"));
		close();
		return false;
	}

	m_ip = getServer()->getHostAddr(0);

	bool reportError = (0 != setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char *)&readBufferSize, sizeof(size_t)));
	if (!reportError)
	{
		size_t tmpRead = 0;
#ifdef WIN32
		int tmpSize = sizeof(size_t);
#else // Linux environment
		unsigned int tmpSize = sizeof(size_t);
#endif
		if (0 != getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&tmpRead,&tmpSize))
			Network::userOutput(INetworkLogger::NETWORK_WARNING,
								Network::networkErrors("Server socket failed to update read buffer size"));
		if (tmpRead != readBufferSize)
		{
			std::stringstream msg;
			msg << "Server socket read buffer size could not be set to ";
			msg << readBufferSize;
			msg << ". Actual size is: ";
			msg << tmpRead;
			Network::userOutput(INetworkLogger::NETWORK_WARNING,
								Network::networkErrors(msg.str()));
		}
	}

	reportError = reportError && (0 != setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char *)&writeBufferSize, sizeof(size_t)));
	if (!reportError)
	{
		size_t tmpWrite = 0;
#ifdef WIN32
		int tmpSize = sizeof(size_t);
#else // Linux environment
		unsigned int tmpSize = sizeof(size_t);
#endif
		if (0 != getsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char *)&tmpWrite,&tmpSize))
			Network::userOutput(INetworkLogger::NETWORK_WARNING,
								Network::networkErrors("Server socket failed to update write buffer size"));
		if (tmpWrite != writeBufferSize)
		{
			std::stringstream msg;
			msg << "Server socket write buffer size could not be set to ";
			msg << writeBufferSize;
			msg << ". Actual size is: ";
			msg << tmpWrite;
			Network::userOutput(INetworkLogger::NETWORK_WARNING,
								Network::networkErrors(msg.str()));
		}
	}

	if (reportError)
		Network::userOutput(INetworkLogger::NETWORK_WARNING,
		Network::networkErrors("Server socket failed to update read or write buffer size"));

	listen(m_socket, 0);

	clearCollection();
	addToCollection(this);

#ifdef WIN32
	m_thread = CreateThread(NULL, 
							0, 
							(LPTHREAD_START_ROUTINE) waitConnectionEvent,
							this,
							0,
							&m_threadID);
	if (0 == m_thread)
	{

#else // Linux environment
	if (0 != pthread_create(&m_threadID, NULL, waitConnectionEvent, this))
	{
#endif
		Network::userOutput(INetworkLogger::NETWORK_WARNING,
							"Failed to start listening thread");
	}

	m_bConnected = (0 != m_threadID);

	return m_bConnected;
}


