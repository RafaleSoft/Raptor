// Network.h: interface for the Network class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
#define AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORNETWORK_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
	#include "RaptorNetwork.h"
#endif
#if !defined(AFX_NETWORKLOGGER_H__04F6649D_5560_45A7_8ED5_B5FC9354256C__INCLUDED_)
	#include "NetworkLogger.h"
#endif

#include <iostream>
#include <strstream>
using std::string;
using std::ostrstream;


class RAPTORNETWORK_API Network
{
public:
    typedef enum
    {
        READ_CLIENT,
        WRITE_CLIENT,
        FDPLX_CLIENT
    } CLIENT_MODEL;
	
	//!	Initialise the Network layer
	static bool initSocketLayer();

	//!
	//!	Get Services
	//!
	static int getSocketSize(void);
	static int getHostnameSize(void);
	static int getSelectTimeout(void);
	static int getNbConnectAttempts(void);

	//!
	//!	Set Services
	//!	- NbConnectAttempts is limited to 16
	//!
	static bool setNbConnectAttempts(int nbAttemps);

	//!	
	//!	Utilities
	//!
    static unsigned int sockNameToAddr(const std::string& address);
	//!	Retrieve the WinSock2 sublayer error.
	static string networkErrors(const std::string& extmsg);
	//!	Computes a CRC on input buffer.
	static unsigned long getCRC(unsigned char *buffer,size_t size);
	//!	Initialise the CRC generator.
	static void initCRC32Table();
	//!	Generic user output service, on console by default,
	//!	on loggers if any registered.
	static void userOutput(INetworkLogger::SEVERITY level, const std::string &msg);
	//!	Register a logger for userOutput.
	static void addLogger(const INetworkLogger *logger);


private:
    Network();
    virtual ~Network();

	static unsigned long reflect(unsigned long ref, unsigned char ch);

	static unsigned long crc32Table[256];

	static const INetworkLogger * *loggers;
};

#endif // !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
