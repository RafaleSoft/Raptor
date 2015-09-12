// RaysDeamon.h : main header file for the RAYSDEAMON application
//

#if !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
#define AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Messages.h"			// io messages IDs and structs

RAPTOR_NAMESPACE


class RaysCmdLine;

class CRaysDeamon
{
public:
	CRaysDeamon();
	virtual ~CRaysDeamon();


	bool Start(	const RaysCmdLine& cmdline );
	
	bool Stop(void);

private:
	//CServer<CDSocket>	*m_Server;
	//iosock_t<CDSocket>	*m_RaysServer;

	void ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[]);
};

#endif // !defined(AFX_RAYSDEAMON_H__1FD417A3_0293_47C1_B3C3_DD773362F2E1__INCLUDED_)
