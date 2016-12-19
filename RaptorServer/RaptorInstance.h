// RaptorInstance.h: interface for the CRaptorInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAPTORINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
#define AFX_RAPTORINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_

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


class CRaptorServerCompressor;

class CRaptorInstance : public server_base_t::request_handler_t,
						public CRenderEntryPoint
{
public:
    static CRaptorInstance* GetInstance(void);
	virtual ~CRaptorInstance();

	//!	Starts a Raptor instance, performing all inits
    bool start(unsigned int width,unsigned int height);

	//!	Run the instance, handling rendering events
	bool run(void);

	//!	Stop server server requests
    bool stop(void);

	//! Handle a closing connection from a client
	bool closeSession(request_handler_t::request_id id);


private:
    CRaptorInstance();

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

    static CRaptorInstance* m_pInstance;

	//! Status
	bool				m_bStarted;

	CRaptorApplication  *m_pApplication;
    CRaptorDisplay		*m_pDisplay;
    RAPTOR_HANDLE		m_pWindow;
	CRaptorIO			*m_pTranslator;
	CRaptorServerCompressor	*m_pCompressor;

	//!	Semaphore to monitor a producer/consumer model
	//!	to render (procuder) an send back results (consumer).
	CRaptorSemaphore	m_prodCons;

	//!	Mutex to protect request queue
	CRaptorMutex		m_mutex;
	//!	Mutex to protect replies queue
	CRaptorMutex		m_rmutex;
	//!	Requests queue
	vector<request>		m_requests;
	//!	Replies queue
	vector<request>		m_replies;

	typedef struct session_t
	{
		request_handler_t::request_id id;
		CRaptorDisplay* display;
	} session;

	//!	Sessions list
	vector<session>		m_sessions;
	vector<session>		m_sessionsToDestroy;
};

#endif // !defined(AFX_RAPTORINSTANCE_H__602E9801_E82B_41B1_9B90_DD498DDF468F__INCLUDED_)
