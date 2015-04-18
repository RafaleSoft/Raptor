// Network.h: interface for the Network class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
#define AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORINSTANCE_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
	#include "RaptorNetwork.h"
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
	
	//!
	//!	Services
	//!
	static int getSocketSize(void);
	static int getHostnameSize(void);
	static int getNbConnectAttempts(void);
	static int getSelectTimeout(void);

	static bool initSocketLayer();

    static unsigned int sockNameToAddr(const std::string& address);

	static string networkErrors(const std::string& extmsg);

	static unsigned long getCRC(unsigned char *buffer,size_t size);

	static void initCRC32Table();


private:
    Network();
    virtual ~Network();

	static unsigned long reflect(unsigned long ref, unsigned char ch);

	static unsigned long crc32Table[256];
};

#endif // !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
