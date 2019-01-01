// RaysServer.h : main header file for the RAYSSERVER application
//

#if !defined(AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_)
#define AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"				// main symbols
#include "../Messages.h"			// io messages IDs and structs
#include "SSocket.h"
#include <afxcoll.h>				// CMapWordToPtr
#include "DeamonManager.h"	// Added by ClassView


class CMsgManager;
class CRaysServerApp;
class CFileManager;
class CPostProcessor;

extern UINT				MsgProcessor( LPVOID pParam );
extern CRaysServerApp	Server;
extern const char *		MSG_LOCK;
extern UINT				regMsgID;
extern UINT				regDmnMsgID;

/////////////////////////////////////////////////////////////////////////////
// CRaysServerApp:
// See RaysServer.cpp for the implementation of this class
//
#define WND ((CRaysServerDlg*)m_pMainWnd)
#define WNDITEM(n) (m_pMainWnd->GetDlgItem(##n))

typedef struct JOBSTRUCT
{
	//	Identification
	unsigned int		jobID;				// ID of the current job
	unsigned int		clientPort;			// port of job ID ( client port )
	unsigned int		clientID;			// client external ID ( for resume identification)

	//	Work units management
	unsigned int		nbWorkUnits;		// nb of WU per job
	unsigned int		nbWorkUnitsReady;	// nb of identified work units
	const CDeamonManager::WORKUNITSTRUCT	
						** workUnits;		// array of pointers to work units

	//	Job stats
	time_t				jobStart;			// duration of job
	time_t				jobEnd;				// duration of job
	unsigned int		processedAcks;		// nb ackowledgements from WU

	//	Job data
	unsigned int		jobWidth;			// width of job ( image )
	unsigned int		jobHeight;			// height of job ( image )
	void				*connection;		// connection to the client ( CServer::iosock_t )
	unsigned char		*globalImageBuffer;	// buffer for computed image
	float				*ZBuffer;

	CPostProcessor		*processor;
} job_struct_t;

typedef JOBSTRUCT* LPJOBSTRUCT;




class CRaysServerApp : public CWinApp
{
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaysServerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL


// Implementation
public:
	void DisconnectDeamon(CString sname,unsigned int port);
	CRaysServerApp();

	void WriteMessage(UINT id,CString &msg);
	void ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[]);

	void UpdateWorkUnitDeamon(const CDeamonManager::WUREGSTRUCT * const wu);

	void RegisterWorkUnit(CString wu,CString deamon);
	void SetRegWUNbCPU(int nbWU,unsigned char nbCPU);
	void UnregisterWorkUnit(int nbWU);


	void SaveFinishedJob(LPJOBSTRUCT lpJob);

	//	Adds a new incoming connection from underlying
	//	server to a pool of available connections 
	//	( i.e. potential client connection requests 
	//	or work unit ready for work )
	//	this connection will be used later for a 
	//	job start request
	void AddNewConnection(iosock_t<CSSocket> *io);

	//	Handles disconnection to abort running jobs
	void CloseConnection(iosock_t<CSSocket> *io);

	bool MsgProcessorAvailable(bool force = false);


public:
	CPtrArray			m_Jobs;			// array of all current jobs
	unsigned char		m_nbWUperJOB;
	int					m_wUnitPriority;
	unsigned int		m_deamonDelay;
	CServer<CSSocket>	*m_Server;
	unsigned char		m_nbProcs;
	LARGE_INTEGER		m_baseFrequency;	

	CCriticalSection	processLock;

	const CDeamonManager * const GetDeamonManager() const { return m_deamonManager; };


//	IHM management variables
private:
	int				m_counter;		// counter for unique job IDs
	CPtrArray		m_FinishedJobs;	// array of all finished jobs for further query	
	float			m_globalJobDone;


//	Client/Server IO variables
private:
	CDeamonManager	*m_deamonManager;
	CMsgManager		*m_msgManager;
	CFileManager	*m_fileManager;
	CEvent			*m_msgEvent;

	CPtrArray		m_incomingClients;
	CListBox		*m_output;

	iosock_t<CSSocket> *UseNewConnection(unsigned int addr,unsigned int port,bool capture = false);
	bool	InstallPlugin(CString pname,iosock_t<CSSocket> *connection);

	friend class CMsgManager;
	friend UINT MsgProcessor( LPVOID pParam );


//	Msg processor functions prototypes
private:
	//	Thread callbacks:
	//	the thread msg is called when a Win32 message ( regMsgID ) is
	//	sent to the thread from the message manager.
	//	The message manager updates the pool of messages directly
	//	received from the socket, and when one message is ready to be
	//	processed, this callback is then called through win32.
	//	this callback fetches the message data and then call ProcessMsg.
	afx_msg void Process_THREAD_MSG(UINT wParam,LONG lParam);
	afx_msg void Process_DEAMON_MSG(UINT wParam,LONG lParam);


	//	The main server processing routine:
	//	every reuest is dispatched from this routine to
	//	one of the Process_XXX routines hereunder
	void ProcessMsg(MSGSTRUCT& msg,unsigned char raw_data[]);

	//	Specific server processing routines.
	void Process_JOB_REQUEST(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_JOB_PERCENT(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_JOB_RUN(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_JOB_START(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_JOB_STOP(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_JOB_WUNIT(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_JOB_STATUS(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_OBJ_DATA(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_OBJ_TEXTURE(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_OBJ_PLUGIN(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_IMG_REQUEST(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_ACK(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_DMN_ACTIVE(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_DMN_INACTIVE(MSGSTRUCT& msg,unsigned char raw_data[]);
	void Process_DMN_DISPATCHJOB(MSGSTRUCT& msg,unsigned char raw_data[]);
	//{{AFX_MSG(CRaysServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYSSERVER_H__08DA65C2_E4BF_4448_A475_CE63C87DC6DA__INCLUDED_)
