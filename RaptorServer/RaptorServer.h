/***************************************************************************/
/*                                                                         */
/*  RaptorServer.h                                                         */
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


#if !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
#define AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRaptorServerInstance;
class CServerTransport;
class CCmdLineParser;


class CRaptorServer
{
public:
	CRaptorServer();
	virtual ~CRaptorServer();

    bool Start(const CCmdLineParser& cmdline);
	
    bool Stop(void);

private:
	CRaptorServerInstance *m_pInstance;
    CServerTransport *m_pTransport;
};

#endif // !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
