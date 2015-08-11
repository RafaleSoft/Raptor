// RaysDeamon.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RaysDeamon.h"
#include "RaysDeamonDlg.h"
#include "..\RaysCommandLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Const and globals

//#define PROFILE	1

CRaysDeamonApp Deamon;



/////////////////////////////////////////////////////////////////////////////
// CRaysDeamonApp

BEGIN_MESSAGE_MAP(CRaysDeamonApp, CWinApp)
	//{{AFX_MSG_MAP(CRaysDeamonApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRaysDeamonApp construction

CRaysDeamonApp::CRaysDeamonApp()
{

}

/////////////////////////////////////////////////////////////////////////////
// CRaysDeamonApp initialization

BOOL CRaysDeamonApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	Enable3dControls();			// Call this when using MFC in a shared DLL

	m_RaysServer = NULL;
	m_Server = new CServer<CDSocket>;
	
	in_addr addr;
	addr.S_un.S_addr = m_Server->GetHostAddr(0);

	RaysCmdLine cmd;
	ParseCommandLine(cmd);

	if (cmd.addrStr.IsEmpty())
	{
		cmd.addrStr = inet_ntoa(addr);
	}

	if (!m_Server->StartServer(PORTBASE+1,LPCTSTR(cmd.addrStr)))
	{
		AfxMessageBox("Failed to start Rays Deamon");
		return FALSE;
	}

	CRaysDeamonDlg dlg;
	m_pMainWnd = &dlg;
	
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}

	return FALSE;
}


void CRaysDeamonApp::ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[])
{
	switch(msg.msg_id)
	{
		case DMN_ACTIVE:
			((CRaysDeamonDlg*)m_pMainWnd)->SetActive(true);
			msg.msg_data[4] = m_Server->GetAddr();
			m_RaysServer->Write(&msg,MSGSIZE);
			break;
		case DMN_INACTIVE:
			((CRaysDeamonDlg*)m_pMainWnd)->SetActive(false);
			break;
		case DMN_DISPATCHJOB:
			{
				STARTUPINFO			si;
				PROCESS_INFORMATION	pi;
				char				WUID[MAX_PATH];
				char				workUnit[MAX_PATH];

				memset(&si,0,sizeof(STARTUPINFO));
				memset(&pi,0,sizeof(PROCESS_INFORMATION));
				si.cb = sizeof(STARTUPINFO);
				si.lpTitle = "Rays 2 Workunit";

				int		workUnitID = msg.msg_data[0];
				int		serverPort = msg.msg_data[3];
				DWORD	nbProcs = msg.msg_data[1];
				DWORD	serverIP = msg.msg_data[2];
				DWORD	priority = msg.msg_data[4];
				DWORD	creationFlag = CREATE_SUSPENDED|CREATE_NEW_PROCESS_GROUP|CREATE_NEW_CONSOLE;

#ifdef PROFILE
				strcpy(WUID," /E error.txt /A WorkUnit.exe");
				sprintf(workUnit," %d %d %d",workUnitID,serverPort,serverIP);
				strcat(WUID,workUnit);
				strcpy(workUnit,"PROFILE.EXE");
#else
				sprintf(WUID,"%d %d %d",workUnitID,serverPort,serverIP);
				strcpy(workUnit,(const char*)raw_data);
#endif

				// creating work unit
				if (0 == CreateProcess(	workUnit,	// pointer to name of executable module
										WUID,		// pointer to command line string
										NULL,		// process security attributes
										NULL,		// thread security attributes
										TRUE,		// handle inheritance flag
										creationFlag|priority,// creation flags
										NULL,		// pointer to new environment block
										NULL,		// pointer to current directory name
										&si,		// pointer to STARTUPINFO
										&pi))		// pointer to PROCESS_INFORMATION
					AfxMessageBox("Work Unit creation (CreateProcess) Failed !!!\nCheck Work Units registration in option dialog");


				//	Will be used when high performance responsiveness will be required
				/*
				// dispatching work units on available processors
				if (lpWUReg->nbProcs>1)
				{
					CString tmpStr;
					if (FALSE == SetProcessAffinityMask(pi.hProcess,	// handle to process
													(DWORD)(1<<(i%nbProcs)))))			// process affinity mask
					{
						tmpStr = "Failed to set process affinity";
						WriteMessage(IDS_SERVER_STRING,tmpStr);
					}
				}
				*/
				msg.msg_header = MSG_START;
				msg.msg_id = DMN_DISPATCHJOB;
				msg.msg_tail = MSG_END;
				msg.msg_size = 0;
				msg.msg_data[0] = workUnitID;
				msg.msg_data[1] = (DWORD)(pi.hProcess);
				msg.msg_data[2] = (DWORD)(pi.hThread);
				msg.msg_data[3] = pi.dwProcessId;
				msg.msg_data[4] = pi.dwThreadId;
				m_RaysServer->Write(&msg,MSGSIZE);
				
				// now the job starts
				::ResumeThread(pi.hThread);

				CString pid;
				pid.Format("Creating new WorkUnit: pid: %d, tid: %d",pi.dwProcessId,pi.dwThreadId);
				((CRaysDeamonDlg*)m_pMainWnd)->DisplayInfo(pid);
			}
			break;
		case JOB_START:
			break;
		case OBJ_PLUGIN:
			{
				//	Install plugin only if not present.
				//	future version will hanle plugin version
				CFileFind ff;
				CString pname = (const char*)(&raw_data[0]);
				int len = pname.GetLength();
				bool install = false;

				if (!ff.FindFile(LPCTSTR(pname)))
					install = true;
				else
				{
					if (ff.FindNextFile())
					{
						CTime fileTime;
						if (ff.GetLastWriteTime(fileTime))
						{
							install = (msg.msg_data[1] > fileTime.GetTime());
						}
					}
				}

				if (install)
				{
					((CRaysDeamonDlg*)m_pMainWnd)->DisplayInfo("Installing plugin : " + pname);
					CFile plugin(LPCTSTR(pname),CFile::modeCreate|CFile::modeWrite);
					plugin.Write(raw_data + len + 1,msg.msg_size - len - 1);
					plugin.Close();
				}
			}
			break;
		default:
			break;
	}

	if (raw_data!=NULL)
		delete [] raw_data;

}

int CRaysDeamonApp::ExitInstance() 
{
	delete m_Server;
	
	return CWinApp::ExitInstance();
}
