// RaysDeamon.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "RaptorNetwork/Network.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif

#include "RaysDeamon.h"


/////////////////////////////////////////////////////////////////////////////
// Const and globals

int main(int argc, char* argv[])
{
#ifdef WIN32
	if (!Network::initSocketLayer())
		return 1;
#endif

	CCmdLineParser parser;
	parser.addOption("port","p",(unsigned short)2048);
	parser.addOption("width","w",(unsigned short)256);
	parser.addOption("height","h",(unsigned short)256);
	parser.addOption("host_addr","a","127.0.0.1");
	parser.parse(argc,argv);

	string addrStr = "127.0.0.1";
	unsigned short port = 2048;
	parser.getValue("port",port);

/*
	CRaptorServer	*p_Server = new CRaptorServer;
    if (p_Server->Start(commandLine))
		return (p_Server->Stop() ? 1 : 0);
	else
		return -1;
*/
/*
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
*/
}

#ifdef WIN32
int WINAPI WinMain(	HINSTANCE hInstance,  // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
                    LPSTR lpCmdLine,      // pointer to command line
                    int nCmdShow          // show state of window
)
{
	/*
	string cmdLine = lpCmdLine;
	vector<string> args;

	size_t pos = cmdLine.find_first_of(' ');
	while (string::npos != pos)
	{
		args.push_back(cmdLine.substr(0,pos));
		cmdLine = cmdLine.substr(pos+1);
		pos = cmdLine.find_first_of(' ');
	}
	args.push_back(cmdLine);

	const char **argv = new const char*[args.size()+1];
	for (pos = 0;pos<args.size();pos++)
		argv[pos] = args[pos].c_str();
	argv[args.size()] = NULL;

	char **c_argv = const_cast<char**>(argv);
	int res = main(args.size(),c_argv);

	delete [] argv;
	*/
	int res = main(__argc,__argv);
    return res;
}
#endif

/*
void CRaysDeamonApp::ManageMsg(MSGSTRUCT& msg,unsigned char raw_data[])
{
	switch(msg.msg_id)
	{
		case DMN_ACTIVE:
			std::cout << "Rays2 Deamon active." << std::endl;
			msg.msg_data[4] = m_Server->GetAddr();
			m_RaysServer->Write(&msg,MSGSIZE);
			break;
		case DMN_INACTIVE:
			std::cout << "Rays2 Deamon inactive." << std::endl;
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
					cout << "Work Unit creation (CreateProcess) Failed !!!\nCheck Work Units registration" << endl;


				//	Will be used when high performance responsiveness will be required
				
				// dispatching work units on available processors
				//if (lpWUReg->nbProcs>1)
				//{
				//	CString tmpStr;
				//	if (FALSE == SetProcessAffinityMask(pi.hProcess,	// handle to process
				//									(DWORD)(1<<(i%nbProcs)))))			// process affinity mask
				//	{
				//		tmpStr = "Failed to set process affinity";
				//		WriteMessage(IDS_SERVER_STRING,tmpStr);
				//	}
				//}
				
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

				stringstream pid;
				pid << "Creating new WorkUnit: pid: ";
				pid << pi.dwProcessId;
				pid << ", tid: ";
				pid << pi.dwThreadId;
				std::cout << pid.str() << std::endl;
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
*/
