// Server.h: interface for the CServer template class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
#define AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <vector>
using std::string;
using std::vector;


#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "Network.h"
#endif

class server_base_t;
class server_base_attrs_t;

//!
//!	Socket layer abstraction
//!
class RAPTORNETWORK_API iosock_base_t
{
public:
	virtual ~iosock_base_t();

	//!	Shutdown socket: no read or write calls will succeed.
	virtual void shutdown();

	//!	The socket is closed, releasing all resources, 
	//!		it cannot be reused anymore.
	virtual void close();

	bool isReadable(void) const;
	bool isWritable(void) const;
	bool isClosing(void) const;
	bool isClosed(void) const { return (m_socket == 0); };

	//!	Accessors
	unsigned int getIP(void) const { return m_ip; };
	unsigned short getPort(void) const { return m_port; };

	server_base_t* getServer(void) const { return m_pServer; };
	void setServer(server_base_t* pServer) { m_pServer = pServer; };

	//!	Operations
	//!	Rq: safe operations allocate buffers and return data, 
	//! user must delete data pointer. When return false,
	//! pointer is unchanged ( may be invalid )
	virtual bool safeRead(void *&data,size_t &size);
	virtual bool safeWrite(const void *data,size_t size);

	//! Configure safe operations
	//! @param useCRC : compute a CRC for read/write ops. (default disabled).
	void safeConfig(bool useCRC);

	//! Overridable operations
	virtual bool onDataReceived(const void *data,size_t size) = 0;
	virtual bool onReplyRequest(const void *&data,size_t &size) = 0;
	virtual bool connect(const std::string& address,unsigned short port) = 0;

	//!	Compare connections
	bool operator==(const iosock_base_t& rsh) const { return m_socket == rsh.m_socket; };


protected:
	iosock_base_t();
	iosock_base_t(const iosock_base_t& sock);

	virtual bool read(void *data,size_t size);
	virtual bool write(const void *data,size_t size);

	friend class iosock_collection_t;
	bool			m_useCRC;
	unsigned int	m_socket;
	unsigned int	m_ip;
	unsigned short	m_port;
	server_base_t*	m_pServer;
};

//!
//!	Server side management abstract interface
//!
class RAPTORNETWORK_API server_base_t
{
public:
	//!
	//!	Server implementations must override a dedicated request handler
	//!
	class request_handler_t
	{
	public:
		typedef void* request_id;

	public:
		request_handler_t() {};
		virtual ~request_handler_t() {};

		//!	Main operations for client-server model
		virtual bool handleRequest(request_handler_t::request_id id,const void *data,size_t size) = 0;
		virtual bool handleReply(request_handler_t::request_id id, const void *&data,size_t &size) = 0;
	};

public:
	//!	Server naming
	const std::string& getHostName(void) const;
	const std::string& getHostName(unsigned int numAlias) const;
	unsigned int getHostAddr(unsigned int numAlias) const;

	//!	Server status
    virtual unsigned short  getPort() const = 0;
	virtual unsigned int  getAddr() const = 0;

	//! Operations
	virtual bool startServer(const std::string& address,unsigned short port);
	virtual bool stopServer(void);

	//!	read one connection from client socks
	virtual bool readClient(iosock_base_t &client);

	//!	read all connections from client socks
	virtual bool readClients(void) = 0;

	//! Start a new worker thread to handle client replies
	virtual bool startClientService(const iosock_base_t &client);

	//!	The request handler
	virtual request_handler_t &getRequestHandler(const iosock_base_t &client) const = 0;

	//!	This method handles new connections.
	virtual bool newClient(void) = 0;

	//!	This method handles connection failures, remove and delete client connection.
	virtual bool clientFailure(const iosock_base_t &client) = 0;


protected:
	server_base_t();
	virtual ~server_base_t();

private:
	server_base_attrs_t	*m_pAttrs;
};



//!
//! Server implementation:
//!	specialiazed with a server socket interface and
//! with advanced connected clients management.
//!
template <class ServerSocket_T,class ClientSocket_T> 
class CServer  : public server_base_t
{
public:
	CServer();
	virtual ~CServer(void);

	virtual unsigned short  getPort() const {return m_socket.getPort();};
	virtual unsigned int  getAddr() const {return m_socket.getIP();};

	size_t getNumClients(void) const { return m_clients.GetSize(); };

    virtual bool startServer(const std::string& address,unsigned short port);
	virtual bool stopServer(void);

	virtual void getClient(size_t numClient,ClientSocket_T*& r_iosock);
	virtual void removeClient(size_t numClient);

	//	Callbacks
	virtual int onNewClient(const ClientSocket_T &client);
	virtual bool onClientClose(const ClientSocket_T &client) { return true; };


protected:
	friend unsigned long waitConnectionEvent(void* pParam);

	virtual bool newClient(void);
	virtual bool readClients(void);
	virtual bool clientFailure(const iosock_base_t &client);

	//	server connection point
	ServerSocket_T	m_socket;

	//	clients management
    vector<ClientSocket_T*>	m_clients;
	vector<ClientSocket_T*>	m_failedClients;
};


//	
//	CServer Implementation
//
template <class ServerSocket_T,class ClientSocket_T> 
CServer<ServerSocket_T,ClientSocket_T>::CServer()
{
}

template <class ServerSocket_T,class ClientSocket_T> 
CServer<ServerSocket_T,ClientSocket_T>::~CServer(void)
{
	m_socket.close();

	stopServer();

	while(m_clients.size()>0)
		removeClient(0);

	m_clients.clear();
}

template <class ServerSocket_T,class ClientSocket_T>
bool CServer<ServerSocket_T,ClientSocket_T>::startServer(const std::string& address,unsigned short port)
{
	m_socket.shutdown();
	m_socket.close();

    m_socket.setServer(this);
	
	//	Try to connect to the given port ...
	if (m_socket.connect(address,port))
	{
		return server_base_t::startServer(address,port);
	}
	else
		return false;
}

template <class ServerSocket_T,class ClientSocket_T>
bool CServer<ServerSocket_T,ClientSocket_T>::stopServer(void)
{
	m_socket.clearCollection();
	m_socket.shutdown();

	return server_base_t::stopServer();
}

template <class ServerSocket_T,class ClientSocket_T> 
bool CServer<ServerSocket_T,ClientSocket_T>::readClients(void)
{
	if (m_clients.size() > 0)
	{
		RAPTOR_TYPENAME vector<ClientSocket_T*>::const_iterator iter = m_clients.begin();
		while (iter != m_clients.end())
		{
			ClientSocket_T *pClient = *iter++;

			if (pClient->isReadable())
				readClient(*pClient);
		}
		return true;
	}
	else
		return false;
}

template <class ServerSocket_T,class ClientSocket_T> 
bool CServer<ServerSocket_T,ClientSocket_T>::newClient(void)
{
	ClientSocket_T client;
	if (client.connect(m_socket))
	{
		ClientSocket_T *pClient = new ClientSocket_T(client);
		m_socket.addToCollection(pClient);

		pClient->setServer(this);
		m_clients.push_back(pClient);

		onNewClient(*pClient);

		return startClientService(*pClient);
	}
	else
		return false;
}

template <class ServerSocket_T,class ClientSocket_T> 
int CServer<ServerSocket_T,ClientSocket_T>::onNewClient(const ClientSocket_T &client)
{
	return m_clients.size();
}

template <class ServerSocket_T,class ClientSocket_T>
bool CServer<ServerSocket_T,ClientSocket_T>::clientFailure(const iosock_base_t &client)
{
	size_t pos=0;

	while ((pos < m_clients.size()) && !(*m_clients[pos] == client))
		pos++;

	if (pos != m_clients.size())
	{
		ClientSocket_T *pClient = m_clients[pos];
		Network::userOutput(INetworkLogger::NETWORK_ERROR,
							Network::networkErrors("IOSock client failure: "));
		pClient->shutdown();
		pClient->close();

		m_socket.removeFromCollection(&client);
		m_failedClients.push_back(pClient);
		m_clients.erase(m_clients.begin() + pos);

		onClientClose(*pClient);

		return true;
	}
	else
		return false;
}


template <class ServerSocket_T,class ClientSocket_T> 
__inline void CServer<ServerSocket_T,ClientSocket_T>::getClient(size_t numClient,ClientSocket_T*& r_iosock)
{
	if (numClient < m_clients.size())
		r_iosock = m_clients.at(numClient);
	else
		r_iosock = NULL;
}


template <class ServerSocket_T,class ClientSocket_T> 
__inline void CServer<ServerSocket_T,ClientSocket_T>::removeClient(size_t numClient)
{
	if (numClient < m_clients.size())
	{
		ClientSocket_T *iosock = m_clients.at(numClient);

		m_clients.erase(m_clients.begin()+numClient);

		delete iosock;
	}
}


#endif // !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
