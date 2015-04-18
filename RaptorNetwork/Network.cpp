// Network.cpp: implementation of the Network class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <sstream>

#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "Network.h"
#endif


static const int SOCKET_SIZE = 2048;

static const int HOSTNAME_SIZE = 64;

static const int NB_CONNECT_ATTEMPTS = 10;

static const int SELECT_TIMEOUT = 1000;

unsigned long Network::crc32Table[256];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Network::Network()
{

}

Network::~Network()
{

}

int Network::getSocketSize(void)
{
	return SOCKET_SIZE;
}

int Network::getHostnameSize(void)
{
	return HOSTNAME_SIZE;
}

int Network::getNbConnectAttempts(void)
{
	return NB_CONNECT_ATTEMPTS;
}

int Network::getSelectTimeout(void)
{
	return SELECT_TIMEOUT;
}

bool Network::initSocketLayer()
{
	initCRC32Table();

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 2, 2 );
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return false;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return false; 
	}
#endif

	return true;
}

string Network::networkErrors(const std::string& extmsg)
{
	string msg;
	int error = GetLastError();
	switch(error)
	{
		case WSANOTINITIALISED:
			msg = "WSA Sockets not initialised";
			break;
		case WSAENETDOWN:
			msg = "WSA Network Down";
			break;
		case WSAEAFNOSUPPORT:
			msg = "WSA Arguments not large enough";
			break;
		case WSAEINPROGRESS:
			msg = "WSA Blocking socket in progress";
			break;
		case WSAEMFILE:
			msg = "WSA No more file descriptor available";
			break;
		case WSAENOBUFS:
			msg = "WSA No buffer space available";
			break;
		case WSAEPROTONOSUPPORT:
			msg = "WSA The specified port is not supported";
			break;
		case WSAEPROTOTYPE:
			msg = "WSA The specified port is the wrong type for this socket.";
			break;
		case WSAESOCKTNOSUPPORT:
			msg = "WSA The specified socket type is not supported in this address family.";
			break;
		case WSAEFAULT:
			msg = "WSA Arguments not large enough";
			break;	
		case WSAENOTSOCK:
			msg = "WSA Not a socket";
			break;
		case WSAEINVAL:
			msg = "WSA Socket in bound with bind";
			break;
		case WSAEADDRINUSE:
			msg = "WSA Address already in use";
			break;
		case WSAECONNREFUSED:
			msg = "WSA Connection refused";
			break;
		case WSAEDESTADDRREQ:
			msg = "WSA Destination address is required";
			break;
		case WSAEHOSTUNREACH:
			msg = "WSA Host unreacheable";
			break;
		case WSAECONNRESET:
			msg = "WSA existing connection was forcibly closed by the remote host.";
			break;
		default:
        {
			std::stringstream  str;
            str << "Socket error num: " << error;
			msg = str.str();
			break;
        }
	}
	msg+="\n";
    msg+=extmsg;

	return msg;
}

unsigned int Network::sockNameToAddr(const std::string& address)
{
    string::size_type pos = 0;
    string::size_type pos2 = 0;

	pos2 = address.find('.',0);
	unsigned int ip0 = atoi(address.substr(pos,pos2-pos).data());

	pos = pos2 + 1;pos2 = address.find('.',pos);
	unsigned int ip1 = atoi(address.substr(pos,pos2-pos).data());

	pos = pos2 + 1;pos2 = address.find('.',pos);
	unsigned int ip2 = atoi(address.substr(pos,pos2-pos).data());

	pos = pos2 + 1;pos2 = address.size();
	unsigned int ip3 = atoi(address.substr(pos,pos2-pos).data());

	unsigned int ip = (ip0 << 24) + (ip1 << 16) + (ip2 << 8) + ip3;
	return ip;
}

unsigned long Network::reflect(unsigned long ref, unsigned char ch) 
{
	unsigned long value(0); 

	// Swap bit 0 for bit 7, bit 1 for bit 6, etc. 
	for(int i = 1; i < (ch + 1); i++) 
	{
		if(ref & 1) 
			value |= 1 << (ch - i); 
		ref >>= 1; 
	}

	return value; 
} 

void Network::initCRC32Table() 
{
	// This is the official polynomial used by CRC-32 
	// in PKZip, WinZip and Ethernet. 
	unsigned long ulPolynomial = 0x04c11db7; 

	// 256 values representing ASCII character codes. 
	for(int i = 0; i <= 0xFF; i++) 
	{ 
		crc32Table[i] = reflect(i, 8) << 24; 
		for (int j = 0; j < 8; j++) 
			crc32Table[i] = (crc32Table[i] << 1) ^ (crc32Table[i] & (1 << 31) ? ulPolynomial : 0); 
		crc32Table[i] = reflect(crc32Table[i], 32); 
	}
} 

unsigned long Network::getCRC(unsigned char *buffer,size_t size) 
{
	unsigned long  ulCRC(0xffffffff); 
	size_t len = size; 

	while(len--) 
		ulCRC = (ulCRC >> 8) ^ crc32Table[(ulCRC & 0xFF) ^ *buffer++]; 

	// Exclusive OR the result with the beginning value. 
	return ulCRC ^ 0xffffffff; 
} 
