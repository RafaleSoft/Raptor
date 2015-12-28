// NetworkLogger.h: interface for the INetworkLogger interface.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKLOGGER_H__04F6649D_5560_45A7_8ED5_B5FC9354256C__INCLUDED_)
#define AFX_NETWORKLOGGER_H__04F6649D_5560_45A7_8ED5_B5FC9354256C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_RAPTORNETWORK_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
	#include "RaptorNetwork.h"
#endif

#include <string>

class RAPTORNETWORK_API INetworkLogger
{
public:
	typedef enum
	{
		NETWORK_DEBUG,
		NETWORK_INFO,
		NETWORK_WARNING,
		NETWORK_ERROR,
		NETWORK_FATAL
	} SEVERITY;

	virtual void Log(SEVERITY level,const std::string &msg) const = 0;

protected:
	INetworkLogger() {};
	virtual ~INetworkLogger() {};
};


#endif	// AFX_NETWORKLOGGER_H__04F6649D_5560_45A7_8ED5_B5FC9354256C__INCLUDED_
