// RaptorServer.h: interface for the CRaptorServer class.
//
//////////////////////////////////////////////////////////////////////

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
	bool run(unsigned int width, unsigned int height);

	//! Status
	bool isRunning(void) const { return m_bIsRunning; };

	//!	Returns the current working image (intialised at start)
	CServerImageRenderer& getImage(void) const;

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
