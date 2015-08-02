// RaysServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RaysServer.h"
#include "RaysServerDlg.h"
#include "MsgManager.h"
#include "FileManager.h"
#include "DeamonManager.h"
#include "DeamonSocket.h"
#include "..\RaysCommandLine.h"

#include "eh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//	TODO
//
//	- envoyer message JOB_STOP au client pour signaler que le JOB est termine
//	et qu'il peut recevoir l'image
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// const and statics
//
static const char * RAYS_SERVER_KEY = "Rafale Soft Inc. (c)";
extern const char * MSG_LOCK = "msgLock";
static const char * REGISTER_MSG_STR = "raysThreadMessage";
static const char * REGISTER_DMN_STR = "raysDeamonMessage";

CRaysServerApp	Server;

UINT			regMsgID = RegisterWindowMessage( REGISTER_MSG_STR );
UINT			regDmnMsgID = RegisterWindowMessage( REGISTER_DMN_STR );
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
// CRaysServerApp message maps

BEGIN_MESSAGE_MAP(CRaysServerApp, CWinApp)
	//{{AFX_MSG_MAP(CRaysServerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_REGISTERED_THREAD_MESSAGE( regMsgID, Process_THREAD_MSG )
	ON_REGISTERED_THREAD_MESSAGE( regDmnMsgID, Process_DEAMON_MSG )
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// CRaysServerApp construction
//
CRaysServerApp::CRaysServerApp():
m_nbProcs(0),m_counter(0),m_nbWUperJOB(1),m_wUnitPriority(NORMAL_PRIORITY_CLASS),
m_deamonDelay(1),m_globalJobDone(0),m_output(NULL),m_msgManager(NULL),m_msgEvent(NULL)
{

}


void fatalError()
{
	AfxMessageBox("Unexpected exception !");
}


/////////////////////////////////////////////////////////////////////////////
// CRaysServerApp initialization
//
BOOL CRaysServerApp::InitInstance()
{
	//	Init application settings in registry
	SetRegistryKey(_T(RAYS_SERVER_KEY));
	LoadStdProfileSettings();  

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//	retrieve system informations
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	m_nbProcs = (unsigned char)si.dwNumberOfProcessors;
	if (!QueryPerformanceFrequency(&m_baseFrequency))
		m_baseFrequency.QuadPart=0;

	m_deamonManager = new CDeamonManager(this);

	//	retrieve registry information
	unsigned int nbWU;
	if (0xFFFFFFFF==(nbWU=GetProfileInt( "Settings", "NbWorkUnits", -1)))
		WriteProfileInt("Settings","NbWorkUnits",0);
	else
	{
		CString section;

		for (unsigned int i=0;i<nbWU;i++)
		{
			section.Format("WorkUnit%d",i);
			CString source = GetProfileString(LPCTSTR(section),"source",NULL);
			CString deamon = GetProfileString(LPCTSTR(section),"deamonIP","127.0.0.1");

			unsigned char nbProcs = (unsigned char)(GetProfileInt(LPCTSTR(section),"nbProcs",1));

			m_deamonManager->CreateRegWorkUnit(source,deamon);
			m_deamonManager->SetRegWUNbCPU(i,nbProcs);
		}
	}

	if (1==(m_nbWUperJOB=(unsigned char)GetProfileInt("Settings","NbWUperJOB",1)))
		WriteProfileInt("Settings","NbWUperJOB",1);
	if (10==(m_deamonDelay=(unsigned int)GetProfileInt("Settings","DeamonDelay",10)))
		WriteProfileInt("Settings","DeamonDelay",10);
	if (NORMAL_PRIORITY_CLASS==(m_wUnitPriority=GetProfileInt("Settings","WUnitPriority",NORMAL_PRIORITY_CLASS)))
		WriteProfileInt("Settings","WUnitPriority",NORMAL_PRIORITY_CLASS);
 
	AfxEnableControlContainer();
	Enable3dControls();		
	CRaysServerDlg dlg;
	m_pMainWnd = &dlg;

	m_Server = new CServer<CSSocket>;
	in_addr addr;
	addr.S_un.S_addr = m_Server->GetHostAddr(0);

	RaysCmdLine cmd;
	ParseCommandLine(cmd);

	if (cmd.addrStr.IsEmpty())
	{
		cmd.addrStr = inet_ntoa(addr);
	}

	if (!m_Server->StartServer(PORTBASE,LPCTSTR(cmd.addrStr)))
	{
		AfxMessageBox("Failed to start Rays Start");
		return FALSE;
	}

	m_msgEvent = new CEvent(FALSE,TRUE,"ThreadEvent",NULL);
	m_msgManager = new CMsgManager(this);
	m_fileManager = new CFileManager();
	m_msgEvent->SetEvent();
	m_msgManager->Run();
	m_deamonManager->Run();

	set_unexpected(fatalError);

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	
	}
	return FALSE;
}


int CRaysServerApp::ExitInstance() 
{
	delete m_Server;	

	LPJOBSTRUCT			lpJob;
	
	//	Clear pending jobs
	while (m_Jobs.GetSize()>0)
	{
		lpJob = (LPJOBSTRUCT)m_Jobs.GetAt(0);

		delete [] lpJob->workUnits;
		delete [] lpJob->globalImageBuffer;
		
		delete lpJob;
		m_Jobs.RemoveAt(0);
	}
	m_Jobs.RemoveAll();


	//	Clear finished jobs
	while (m_FinishedJobs.GetSize()>0)
	{
		lpJob = (LPJOBSTRUCT)m_FinishedJobs.GetAt(0);
		delete [] lpJob->workUnits;
		delete [] lpJob->globalImageBuffer;
		
		delete lpJob;
		m_FinishedJobs.RemoveAt(0);
	}
	m_FinishedJobs.RemoveAll();

	delete m_msgManager;
	delete m_fileManager;
	delete m_deamonManager;

	return CWinApp::ExitInstance();
}



void CRaysServerApp::SetRegWUNbCPU(int nbWU,unsigned char nbCPU)
{
	if (!m_deamonManager->SetRegWUNbCPU(nbWU,nbCPU))
			AfxMessageBox("Some CPUs are in use or invalid WorkUnit state!");
	else
	{
	
		// registry
		CString section;
		section.Format("WorkUnit%d",nbWU);
		WriteProfileInt(LPCTSTR(section),"nbProcs",nbCPU);
	}
}


void CRaysServerApp::RegisterWorkUnit(CString wu,CString deamon)
{
	CString section;

	const CDeamonManager::WUREGSTRUCT * const lpRegWU = 
		m_deamonManager->CreateRegWorkUnit(wu,deamon);
	
	if (lpRegWU->nbProcs > 1)
	{
		// registry
		int pos = m_deamonManager->GetNbRegWorkUnits();
		section.Format("WorkUnit%d",pos-1);
		WriteProfileInt(LPCTSTR(section),"nbProcs",lpRegWU->nbProcs);
	}
	else
	{
		// registry
		int nbWU = m_deamonManager->GetNbRegWorkUnits();
		WriteProfileInt("Settings","NbWorkUnits",nbWU);
		section.Format("WorkUnit%d",nbWU-1);
		WriteProfileString(LPCTSTR(section),"source",LPCTSTR(wu));
		WriteProfileString(LPCTSTR(section),"deamonIP",LPCTSTR(deamon));
		WriteProfileInt(LPCTSTR(section),"nbProcs",1);
	}
}

void CRaysServerApp::UpdateWorkUnitDeamon(const CDeamonManager::WUREGSTRUCT * const wu)
{
	CString section;
	int pos = 0;

	int max = m_deamonManager->GetNbRegWorkUnits();

	while (pos<max)
	{
		const CDeamonManager::WUREGSTRUCT * const lpRegWU = 
			m_deamonManager->GetRegWorkUnit(pos++);

		if (lpRegWU == wu)
		{
			// registry
			section.Format("WorkUnit%d",pos-1);
			WriteProfileString(LPCTSTR(section),"deamonIP",LPCTSTR(wu->deamonIP));

			break;
		}
	}
}


void CRaysServerApp::UnregisterWorkUnit(int nbWU)
{
	int max = m_deamonManager->GetNbRegWorkUnits();

	if ((nbWU<max) && (nbWU>=0))
	{
		if (m_deamonManager->DestroyRegWorkUnit(nbWU))
		{
			CString section,section2,source;
			int oldnbWU = GetProfileInt("Settings","NbWorkUnits",0);
			WriteProfileInt("Settings","NbWorkUnits",oldnbWU-1);
			int pos = nbWU+1;

			while (pos<oldnbWU)
			{
				section.Format("WorkUnit%d",pos-1);
				section2.Format("WorkUnit%d",pos++);

				source=GetProfileString(LPCSTR(section2),"source",NULL);
				WriteProfileString(LPCTSTR(section),"source",source);

				source=GetProfileString(LPCSTR(section2),"deamonIP","hostname");
				WriteProfileString(LPCTSTR(section),"deamonIP",source);

				unsigned int nb=GetProfileInt(LPCTSTR(section2),"nbProcs",0);
				WriteProfileInt(LPCTSTR(section),"nbProcs",nb);
			}
		}
		else
			AfxMessageBox("CPU in use, can't unregister Work Unit !");
	}
}


void CRaysServerApp::AddNewConnection(iosock_t<CSSocket> *io)
{
	io->Iofile()->AssertValid();
	ASSERT(CSocket::FromHandle(io->socket->m_hSocket) != NULL);
	m_incomingClients.Add(io);
}


iosock_t<CSSocket> *CRaysServerApp::UseNewConnection(unsigned int addr,unsigned int port,bool capture)
{
	bool found = false;
	int pos = 0;
	iosock_t<CSSocket> *res = NULL;

	//	Search in current unassociated new clients
	while ((!found) && (pos<m_incomingClients.GetSize()))
	{
		iosock_t<CSSocket> *io = (iosock_t<CSSocket> *)m_incomingClients[pos++];
		found = ((io->IP() == addr) && (port == io->Port()));
	}
	if (found)
	{
		res = (iosock_t<CSSocket> *)m_incomingClients[pos-1];
		res->Iofile()->AssertValid();

		ASSERT(CSocket::FromHandle(res->Socket()->m_hSocket) != NULL);

		if (capture)
			m_incomingClients.RemoveAt(pos-1);
	}
	else
	{
		//	Search in finished jobs
		LPJOBSTRUCT lpJob = NULL;
		pos = 0;
		//	Search in current unassociated new clients
		while ((!found) && (pos<m_FinishedJobs.GetSize()))
		{
			lpJob = (LPJOBSTRUCT)m_FinishedJobs[pos++];
			iosock_t<CSSocket> *io = (iosock_t<CSSocket> *)(lpJob->connection);
			found = ((io->IP() == addr) && (port == io->Port()));
		}

		if (found)
			res = (iosock_t<CSSocket> *)(lpJob->connection);
	}

	return res;
}

void CRaysServerApp::CloseConnection(iosock_t<CSSocket> *io)
{
	// Search if the connection is a workunit
	// of a running job
	LPJOBSTRUCT lpJob = NULL;
	int pos = 0;
	bool found = false;

	while (!found && (pos<m_Jobs.GetSize()))
	{
		lpJob = (LPJOBSTRUCT)m_Jobs[pos++];
		
		for (int i=0;i<lpJob->nbWorkUnitsReady;i++)
		{
			const CDeamonManager::WORKUNITSTRUCT *pWu = lpJob->workUnits[i];
			iosock_t<CSSocket> *io2 = (iosock_t<CSSocket> *)(pWu->connection);

			if ((io->IP() == io2->IP()) && (io->Port() == io2->Port()));
			{
				CString msgStr;
				msgStr.Format("Work unit (job: %d) failed. Job will be corrupted",lpJob->jobID);
				WriteMessage(IDS_SERVER_STRING,msgStr);

				lpJob->nbWorkUnitsReady--;
				found = true;

				m_deamonManager->ReleaseWorkUnit(pWu->workUnitID);
			
				if (lpJob->nbWorkUnitsReady == 0)
				{
					time(&lpJob->jobEnd);

					msgStr.Format("Job %d (%d on port %d) is complete!",lpJob->clientID,lpJob->jobID,lpJob->clientPort);
					WriteMessage(IDS_SERVER_STRING,msgStr);

					m_FinishedJobs.Add(lpJob);
					m_Jobs.RemoveAt(pos-1);
					WND->SetNbJobs(m_Jobs.GetSize());
				}
			}
		}
	}
}

void CRaysServerApp::ProcessMsg(MSGSTRUCT& msg,unsigned char raw_data[])
{
	CString msgStr;

	//	Two messages cannot be processed at a time:
	//	server private data is shared
	processLock.Lock();

	switch(msg.msg_id)
	{
	case JOB_START:
		Process_JOB_START(msg,raw_data);
		break;
	case JOB_WUNIT:
		Process_JOB_WUNIT(msg,raw_data);
		break;
	case JOB_STATUS:
		Process_JOB_STATUS(msg,raw_data);
		break;
	case JOB_STOP:
		Process_JOB_STOP(msg,raw_data);
		break;
	case JOB_RUN:
		Process_JOB_RUN(msg,raw_data);
		break;
	case OBJ_DATA:
		Process_OBJ_DATA(msg,raw_data);
		break;
	case OBJ_TEXTURE:
		Process_OBJ_TEXTURE(msg,raw_data);
		break;
	case OBJ_PLUGIN:
		Process_OBJ_PLUGIN(msg,raw_data);
		break;
	case IMG_REQUEST:
		Process_IMG_REQUEST(msg,raw_data);
		break;
	case ACK_OBJ:
	case ACK_NONE:
	case ACK_JOB:
		Process_ACK(msg,raw_data);
		break;
	case JOB_PERCENT:
		Process_JOB_PERCENT(msg,raw_data);
		break;
	case JOB_REQUEST:
		Process_JOB_REQUEST(msg,raw_data);
		break;
	case DMN_ACTIVE:
		Process_DMN_ACTIVE(msg,raw_data);
		break;
	case DMN_INACTIVE:
		Process_DMN_INACTIVE(msg,raw_data);
		break;
	case DMN_DISPATCHJOB:
		Process_DMN_DISPATCHJOB(msg,raw_data);
		break;
	default:
		msgStr="Unrecognized client message";
		WriteMessage(IDS_SERVER_STRING,msgStr);
		break;
	}

	if (raw_data!=NULL)
		delete [] raw_data;

	processLock.Unlock();
}


bool CRaysServerApp::MsgProcessorAvailable(bool force)
{
	if (force)
		processLock.Unlock();

	CSingleLock MPAlock(&processLock);

	return (MPAlock.IsLocked() == FALSE);
}


void CRaysServerApp::ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[])
{
	m_msgManager->ManageMsg(msg,raw_data);
}


void CRaysServerApp::WriteMessage(UINT id,CString &msg)
{
	CString msgTmp;

	//	window dlg is temporary and should not be stored
	m_output = (CListBox*)(WNDITEM(IDC_LIST1));
	
	if (m_output != NULL)
	{
		msgTmp.LoadString(id);
		msgTmp+=msg;
		m_output->InsertString(0,msgTmp);
	}
}


BOOL CRaysServerApp::OnIdle(LONG lCount) 
{
	//	In every situation, if the server is Idle
	//	it can notify that the msgProcessor is available
	m_msgEvent->SetEvent();
	CString idle = "Server idle";
	WriteMessage(IDS_SERVER_STRING,idle);	
	return CWinApp::OnIdle(lCount);
}


bool CRaysServerApp::InstallPlugin(CString pname,iosock_t<CSSocket> *connection)
{
	UINT size = 0;
	UINT size2 = 0;
	unsigned char *buffer = NULL;

	CFileManager::LPRAYSFILE pFile = m_fileManager->GetFile(pname);

	if (pFile != NULL)
	{
		if (m_fileManager->GetFile(pname,size,size2,buffer))
		{
			return m_deamonManager->InstallPlugin(	connection->IP(),
													connection->Port(),
													pFile->fileDate,
													pname,
													size,
													buffer);
		}
		else
			return false;
	}
	else
		return false;
}

void CRaysServerApp::SaveFinishedJob(LPJOBSTRUCT lpJob)
{
	if (lpJob != NULL)
		m_FinishedJobs.Add(lpJob);
}