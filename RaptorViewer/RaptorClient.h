/***************************************************************************/
/*                                                                         */
/*  RaptorClient.h                                                         */
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


#if !defined(AFX_RAPTORCLIENT_H__EBD00DF9_AE71_483D_BB72_67717C6FF413__INCLUDED_)
#define AFX_RAPTORCLIENT_H__EBD00DF9_AE71_483D_BB72_67717C6FF413__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Client.h"
#endif
#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/ClientSocket.h"
#endif
#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
	#include "System/RenderEntryPoint.h"
#endif

#include "Raptordll.h"
RAPTOR_NAMESPACE

class CServerImageRenderer;
class CCmdLineParser;

class CRaptorClient : public CRenderEntryPoint
{
public:
	CRaptorClient(void);
	virtual ~CRaptorClient(void);

	//!	Server connection
    bool start(const CCmdLineParser &cmdLine);
	
    bool stop(void);

	bool load(const std::string& fname);

	//!	Rendering
	bool run(	uint32_t width, uint32_t height,
				uint32_t r_width, uint32_t r_height);

	//! Status
	bool isRunning(void) const { return m_bIsRunning; };

	//!	Returns the current working image (intialised at start)
	CServerImageRenderer* getImage(void) const;

	//!	Perform a query to the server and refresh the current image
	void queryServerImage(void);


private:
	//!	Implements CRenderentrypoint
	virtual void glRender(void);

	bool					m_bIsRunning;
	void					*client_id;
	CClient<CClientSocket>	*m_Client;
	CRaptorDisplay			*m_pDisplay;
	RAPTOR_HANDLE			m_window;
	CServerImageRenderer	*m_pImage;
	void*					m_hThread;
};


#endif // !defined(AFX_RAPTORCLIENT_H__EBD00DF9_AE71_483D_BB72_67717C6FF413__INCLUDED_)
