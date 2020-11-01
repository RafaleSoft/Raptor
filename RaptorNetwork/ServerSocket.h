#if !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
#define AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "Server.h"
#endif

#pragma warning( disable : 4251)    // DLL interface required for STL exports
class RAPTORNETWORK_API iosock_collection_t
{
public:
	iosock_collection_t();
	virtual ~iosock_collection_t() {};

	//! Returns the number of items in the collection
	size_t getCount(void) const
	{ return m_collection.size(); };

	//!	Returns true if io successfully added to collection,
	//! false otherwise (e.g. already in collection)
	bool addToCollection(const iosock_base_t* io);

	//!	Returns true if io is successfully removed from collection,
	//!	false otherwise (e.g. io not in collection)
	bool removeFromCollection(const iosock_base_t* io);

	//!	Removes all entries.
	void clearCollection(void) { m_collection.clear(); };

	//!	Performs a select for reading on the collection
	bool areReadable(void);

	//!	Performs a select for reading on the collection
	bool areWritable(void);

	const iosock_base_t* getCollectionItem(size_t itemPos)
	{ if (itemPos < m_collection.size()) return m_collection[itemPos].iosock; else return NULL;}

	bool isReadable(size_t itemPos) const
	{ if (itemPos < m_collection.size()) return m_collection[itemPos].readable; else return false;}
	
	bool isWritable(size_t itemPos) const
	{ if (itemPos < m_collection.size()) return m_collection[itemPos].writable; else return false;}
	

protected:
	typedef struct iosock_collection_item_t
	{
		const iosock_base_t*	iosock;
		bool					readable;
		bool					writable;
	} iosock_collection_item;
	vector<iosock_collection_item_t> m_collection;

	static	struct timeval timeout;
};


//!	
//!	A ServerSocket specializes a socket collection:
//!	- a priviledged socket for server's listening
//!	- a pool of connected clients
//!
class RAPTORNETWORK_API CServerSocket : public iosock_base_t,
										public iosock_collection_t
{
public:
	CServerSocket();
	virtual ~CServerSocket(){};

	//!	Implements iosock_base_t connection service:
	//!		a thread is started to manage all clients input
	virtual bool connect(const std::string& address,unsigned short port);

	//!	Override @see iosock_base_t termination process.
	//!		the listening thread is halted
	virtual void shutdown();

	//!	Returns true after a successful call to connect.
	bool isConnected(void) const
	{ return m_bConnected; };

	//!	Specifies read & write buffer sizes
	static bool setBuffersSizes(size_t readBuffer,size_t writeBuffer);


private:
	//!	Implements @see iosock_base_t: callback for incoming data
	virtual bool onDataReceived(const void *,size_t );

	//!	Implements @see iosock_base_t: callback when a reply is demanded
	virtual bool onReplyRequest(const void *&,size_t &);

	static	size_t	readBufferSize;
	static	size_t	writeBufferSize;

	bool				m_bConnected;
	unsigned long int	m_threadID;
#ifdef WIN32
	HANDLE		m_thread;
#endif
};

#endif // !defined(AFX_SERVERSOCKET_H__A2920B8D_12E4_11D3_9142_D3B83905F198__INCLUDED_)
