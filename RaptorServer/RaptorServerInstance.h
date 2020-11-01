/***************************************************************************/
/*                                                                         */
/*  RaptorInstance.h                                                       */
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


#if !defined(AFX_RAPTORSERVERINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
#define AFX_RAPTORSERVERINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/Server.h"
#endif

#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
	#include "System/RenderEntryPoint.h"
#endif


#include "Raptordll.h"
RAPTOR_NAMESPACE

class CServerSession;
class CRaptorServerCompressor;

class CRaptorServerInstance :	public server_base_t::request_handler_t,
								public CRenderEntryPoint
{
public:
	static CRaptorServerInstance* GetInstance(void);
	virtual ~CRaptorServerInstance();

	//!	Starts a Raptor instance, performing all inits
    bool start(unsigned int width,unsigned int height);

	//!	Run the instance, handling rendering events
	bool run(void);

	//!	To quit application from outside main thread, call CloseWindow 
	bool closeWindow(void);

	//!	Stop server server requests
    bool stop(void);

	//! Handle a closing connection from a client
	bool closeSession(request_handler_t::request_id id);


private:
	CRaptorServerInstance();

	//!	Implements CRenderentrypoint
	virtual void glRender();

	//!	Implements Server request callback
	virtual bool handleRequest(request_handler_t::request_id id,const void *data,size_t size);

	//! Implements Server reply request
	virtual bool handleReply(request_handler_t::request_id id, const void *&data,size_t &size);

	typedef struct request_t
	{
		request_handler_t::request_id id;
		bool			reply;
		bool			render;
		size_t			size;
		CRaptorDisplay	*display;
		void			*data;
	} request;

	//!	Parse received request.
	bool executeRequest(request &r);

	//!	Generate the final frame to be decompressed by client.
	void processOutputFrame(request &r);

	//!	Helper method to load a Data Package received from clients
	bool loadPackage(const CRaptorNetwork::DATA_COMMAND& cmd3, request_handler_t::request_id id);


	static CRaptorServerInstance* m_pInstance;

	//! Status
	bool				m_bStarted;

	CRaptorApplication  *m_pApplication;
    CRaptorDisplay		*m_pDisplay;
    RAPTOR_HANDLE		m_pWindow;
	CRaptorServerCompressor	*m_pCompressor;
	CServerSession		*m_pSessionManager;

	//!	Semaphore to monitor a producer/consumer model
	//!	to render (procuder) an send back results (consumer).
	CRaptorSemaphore	m_prodCons;

	//!	Mutex to protect request queue
	CRaptorMutex		m_mutex;
	//!	Mutex to protect replies queue
	CRaptorMutex		m_rmutex;
	//!	Requests queue
	std::vector<request>		m_requests;
	//!	Replies queue
	std::vector<request>		m_replies;
	//! Displays to recycle
	std::vector<CRaptorDisplay*> m_recycledDisplays;
};

#endif // !defined(AFX_RAPTORINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
