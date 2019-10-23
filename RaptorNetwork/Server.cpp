/***************************************************************************/
/*                                                                         */
/*  Server.cpp                                                             */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "stdafx.h"

#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "Server.h"
#endif
#if !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "ServerSocket.h"
#endif
#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "Network.h"
#endif

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

iosock_base_t::iosock_base_t()
{
	m_socket = 0;
	m_ip = 0;
	m_port = 0;
	m_pServer = NULL;
	m_useCRC = false;
}

iosock_base_t::iosock_base_t(const iosock_base_t& sock)
{
	m_socket = sock.m_socket;
	m_ip = sock.m_ip;
	m_port = sock.m_port;
	m_pServer = sock.m_pServer;
	m_useCRC = sock.m_useCRC;
}

bool iosock_base_t::isReadable(void) const
{
	fd_set	readfds;
	FD_ZERO(&readfds);
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = Network::getSelectTimeout();

	FD_SET(m_socket, &readfds);
	int res = select(1,&readfds,NULL,NULL,&delay);
	return (1 == res);
}

bool iosock_base_t::isWritable(void) const
{
	fd_set	writefds;
	FD_ZERO(&writefds);
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = Network::getSelectTimeout();

	FD_SET(m_socket, &writefds);
	int res = select(1,NULL,&writefds,NULL,&delay);
	return (1 == res);
}

bool iosock_base_t::isClosing(void) const
{
	char atLeastAChar = 0;
	int nb_read = recv(m_socket,&atLeastAChar,1,MSG_PEEK);
	
	if (nb_read == SOCKET_ERROR)
	{
		Network::userOutput(INetworkLogger::NETWORK_ERROR,
							Network::networkErrors("Client sock lost connection, cannot be read"));

		//	Indeed socket may not be closing ...
		return true;
	}

	return (nb_read == 0);	// client gracefully closed
}

bool iosock_base_t::safeWrite(const void *data, size_t size)
{
	unsigned long crc32 = 0;
	if (m_useCRC)
		Network::getCRC((unsigned char*)data,size);

	struct
	{
		unsigned long  crc;
		unsigned int	sz;
	} header;

	header.crc = crc32;
	header.sz = size;

	if (write(&header,sizeof(header)))
		return write(data,size);
	else
		return false;
}

bool iosock_base_t::write(const void *data, size_t size)
{
	unsigned int nb_sent = 0;
	while (nb_sent < size)
	{
		char *data2 = (char*)data + nb_sent;
		int size2 = size - nb_sent;

		int nb_s = ::send(m_socket,data2,size2,0);
		if (nb_s == 0)
		{
			return false;
		}
		else if (nb_s == SOCKET_ERROR)	// connection failed
		{
			Network::userOutput(INetworkLogger::NETWORK_ERROR,
								Network::networkErrors("Client sock lost connection, cannot be written"));
			shutdown();
			close();
			return false;
		}

		nb_sent += nb_s;
	}

	return (nb_sent == size);
}


bool iosock_base_t::safeRead(void *&data,size_t &size)
{
	struct
	{
		unsigned long  crc;
		unsigned int	sz;
	} header;

	if (read(&header,sizeof(header)))
	{
		size = header.sz;
		data = new unsigned char[size];
		if (read(data,size))
		{
			if (m_useCRC)
			{
				unsigned long crc32 = Network::getCRC((unsigned char*)data,size);
				return header.crc == crc32;
			}
			else
				return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool iosock_base_t::read(void *data,size_t size)
{
	unsigned int nb_read = 0;
	while (nb_read < size)
	{
		char *data2 = (char*)data + nb_read;
		int size2 = size - nb_read;

		int nb_r = ::recv(m_socket,data2,size2,0);
		if (nb_r == 0)
		{
			return false;
		}
		else if (nb_r == SOCKET_ERROR)	// connection failed
		{
			Network::userOutput(INetworkLogger::NETWORK_ERROR,
								Network::networkErrors("Client sock lost connection, cannot be read"));
			shutdown();
			close();
			return false;
		}

		nb_read += nb_r;
	}

	return (nb_read == size);
}


iosock_base_t::~iosock_base_t() 
{
}

void iosock_base_t::shutdown()
{
	::shutdown(m_socket,SD_BOTH);
	m_ip = 0;
	m_port = 0;
}

void iosock_base_t::close()
{
	CLOSESOCKET(m_socket);
	m_socket = 0;
}

class server_base_attrs_t
{
public:
	server_base_attrs_t()
		:m_bWriteClients(true),
		h_WriteClients(0),
		hostname("") 
	{};

	//	Server datas
	bool						m_bWriteClients;
	std::vector<void*>			h_WriteClients;
	std::string					hostname;
	std::vector<std::string>	m_aliases;
	std::vector<unsigned int>	m_addrs;
};

server_base_t::~server_base_t()
{
	delete m_pAttrs;
}

const std::string& server_base_t::getHostName(void) const 
{ 
	return m_pAttrs->hostname; 
}

server_base_t::server_base_t()
{
	m_pAttrs = new server_base_attrs_t();

	char *HName = new char[Network::getHostnameSize()];
	memset(HName,0,Network::getHostnameSize());

	if (gethostname(HName,Network::getHostnameSize()) == 0)
	{
		HOSTENT *hostent = gethostbyname (HName);
		if (hostent != NULL)
		{
			m_pAttrs->hostname = HName;
			m_pAttrs->m_aliases.push_back(m_pAttrs->hostname);

			unsigned int buf;
			int nbaddr = 0;
			char *data = hostent->h_addr_list[nbaddr++];

			while (data != 0)
			{
				buf = 0;
				for (int i=0;i<hostent->h_length;i++)
					buf = (buf << 8) + *((unsigned char*)data++);

				long buf2 = htonl(buf);

				m_pAttrs->m_addrs.push_back(buf2);
				data = hostent->h_addr_list[nbaddr++];
			}

			nbaddr = 0;
			data = hostent->h_aliases[nbaddr++];
			while (data != 0)
			{
				m_pAttrs->m_aliases.push_back(string(data));
				data = hostent->h_aliases[nbaddr++];
			}
		}
	}

	delete [] HName;
};


const std::string& server_base_t::getHostName(unsigned int numAlias) const
{ 
	if(numAlias < m_pAttrs->m_aliases.size()) 
		return m_pAttrs->m_aliases[numAlias]; 
	else 
		return m_pAttrs->hostname; 
}

unsigned int server_base_t::getHostAddr(unsigned int numAlias) const 
{ 
	if(numAlias < m_pAttrs->m_addrs.size()) 
		return m_pAttrs->m_addrs[numAlias]; 
	else 
		return 0; 
}

bool server_base_t::stopServer(void)
{
	m_pAttrs->m_bWriteClients = false;

	if (0 < m_pAttrs->h_WriteClients.size())
	{
		for (size_t i=0;i<m_pAttrs->h_WriteClients.size();i++)
		{
			void* h = m_pAttrs->h_WriteClients[i];
			if (h != 0)
			{
#ifdef WIN32
				/*DWORD res = */WaitForSingleObject((HANDLE)h,INFINITE);
				CloseHandle((HANDLE)h);
				m_pAttrs->h_WriteClients[i] = 0;
#else // Linux environment
				void *ret = NULL;
				pthread_join((pthread_t)h,&ret);
#endif
			}
		}
	}

	return true;
}

bool server_base_t::readClient(iosock_base_t &client)
{
	bool doCloseClient = false;

	if (client.isClosing())
		doCloseClient = true;
	else
	{
		void *data = NULL;
		size_t size = 0;
		if (!client.safeRead(data,size))
			doCloseClient = true;
		else
		{
			client.onDataReceived(data,size);
			delete [] data;
		}
	}
			
	if (doCloseClient)
	{
		clientFailure(client);
		return false;
	}
	else
		return true;
}

#ifdef WIN32
	DWORD
#else
	void*
#endif
writeClientsThread(void* pParam)
{
	struct threadData_t
	{
		server_base_t* server;
		server_base_attrs_t* attrs;
		iosock_base_t* client;
	} *threadData = (threadData_t*)pParam;

	iosock_base_t* pClient = threadData->client;
	while ((threadData->attrs->m_bWriteClients) &&
			!pClient->isClosed())
	{
		const void *data = NULL;
		size_t size = 0;
		if (pClient->onReplyRequest(data,size))
		{
			bool failed = !pClient->safeWrite(data,size);
			delete [] data;

			if (failed)
			{
				threadData->server->clientFailure(*pClient);
				break;
			}
		}
	}
	delete threadData;

#ifdef WIN32
	ExitThread(0);
#else	// Linux environment
	pthread_exit(0);
#endif
}

bool server_base_t::startClientService(const iosock_base_t &client)
{
	struct threadData_t
	{
		server_base_t* server;
		server_base_attrs_t* attrs;
		const iosock_base_t* client;
	} * threadData = new threadData_t;
	threadData->server = this;
	threadData->attrs = m_pAttrs;
	threadData->client = &client;

	unsigned long int ui_threadID = 0;
#ifdef WIN32
	HANDLE h_write = CreateThread(NULL,
								  0, 
								  (LPTHREAD_START_ROUTINE) writeClientsThread,
								  threadData,
								  0,
								  &ui_threadID);

	if (0 != h_write)
		m_pAttrs->h_WriteClients.push_back(h_write);
#else // Linux environment
	int res = pthread_create(&ui_threadID, NULL, writeClientsThread, threadData);
	if (0 == res)
		m_pAttrs->h_WriteClients.push_back((void*)ui_threadID);
#endif

	return (0 != ui_threadID);
}

bool server_base_t::startServer(const std::string& /*address*/,unsigned short)
{
	return true;
}


