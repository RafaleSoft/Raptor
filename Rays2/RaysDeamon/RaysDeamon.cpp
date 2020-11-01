/***************************************************************************/
/*                                                                         */
/*  RaysDeamon.cpp                                                         */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "RaptorNetwork/Network.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif

#include "RaysDeamon.h"


/////////////////////////////////////////////////////////////////////////////
// Const and globals
static CRaysDeamon	*p_Server = NULL;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch(fdwCtrlType)
	{ 
		// Handle the CTRL-C signal.
		case CTRL_C_EVENT:
		{
			std::cout << "Deamon user exit requested by Ctrl-C. Exiting, bye!" << std::endl;
			p_Server->requestExit();
			return(TRUE);
			break;
		}
		// CTRL-CLOSE: confirm that the user wants to exit.
		case CTRL_CLOSE_EVENT:
		{
			std::cout << "Deamon user exit requested by Ctrl-close. Exiting, bye!" << std::endl;
			p_Server->requestExit();
			return(TRUE);
			break;
		}
		// Pass other signals to the next handler.
		case CTRL_BREAK_EVENT: 
			return FALSE; 
		case CTRL_LOGOFF_EVENT: 
			return FALSE; 
		case CTRL_SHUTDOWN_EVENT: 
			return FALSE; 
		default: 
			return FALSE; 
	} 
} 

void print_help(void)
{
	std::cout << "Rays Deamon command line help:" << std::endl;

	std::cout << "  --port|-p : defines the deamon listening port, by default 2049" << std::endl;
	std::cout << "  --host_addr|-a : defines the deamon listening IP address, by default 127.0.0.1" << std::endl;
	std::cout << "  --config_file|-f : the path to the deamon configuration file, by default RaysDeamon.config in the current execution folder" << std::endl;
	std::cout << "  --work_unit|-w : the path to a rays workunit executable. Any number of workunits can be provided, but at least one workunit is mandatory." << std::endl;
	std::cout << "  --cpu|-c : the number of CPU cores allocated to each workunit" << std::endl;
	std::cout << "  --help|-h : print this help and quit" << std::endl;
	std::cout << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
//	Entry point
int main(int argc, char* argv[])
{
	std::cout << std::endl;
	std::cout << "        Raptor Rays Deamon.        " << std::endl;
	std::cout << "        version: " << RAPTOR_VERSION_STR << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	std::cout << std::endl;

	
	CCmdLineParser parser;
	std::vector<string> wus;
	std::vector<unsigned int> cpus;
	parser.addOption("port", "p", (uint16_t)2049);
	parser.addOption("host_addr", "a", std::string("127.0.0.1"));
	parser.addOption("config_file", "f", std::string("RaysDeamon.config"));
	parser.addOption("work_unit","w",wus);
	parser.addOption("cpu","c",cpus);
	parser.addOption("help", "h", CCmdLineParser::NO_VALUE_OPTION);
	if (!parser.parse(argc,argv))
	{
		std::cout << "Deamon failed to parse command line. Exiting, bye!" << std::endl;
		return -1;
	}

	CCmdLineParser::NO_VALUE_OPTION_t help = CCmdLineParser::NO_VALUE_UNDEFINED;
	parser.getValue<CCmdLineParser::NO_VALUE_OPTION_t>("h", help);
	if (CCmdLineParser::NO_VALUE_VALUE == help)
	{
		print_help();
		return 0;
	}

	if (!Network::initSocketLayer())
	{
		std::cout << "Network layer not initialized properly.  Exiting, bye!" << std::endl;
		return -1;
	}

	p_Server = new CRaysDeamon;
    if (!p_Server->start(parser))
	{
		std::cout << "Deamon failed to start. Exiting, bye!" << std::endl;
		return -1;
	}
	else
	{
		if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
			std::cout << "Failed to install deamon crtl handler. Exiting, bye!" << std::endl;

		while (!p_Server->doExit())
			Sleep(500);
		int res = (p_Server->stopServer() ? 0 : 1);
		delete p_Server;

		std::cout << "Rays Deamon exiting with code " << res << ". Bye!" << std::endl;
		return res;
	}
}

CRaysDeamon::CRaysDeamon()
	:m_bExit(false)
{
}

CRaysDeamon::~CRaysDeamon()
{
}

server_base_t::request_handler_t& CRaysDeamon::getRequestHandler(const iosock_base_t& ) const
{
	const server_base_t::request_handler_t* rq = this;
	server_base_t::request_handler_t& handler = const_cast<server_base_t::request_handler_t&>(*rq);
	return handler;
}


bool CRaysDeamon::handleReply(request_handler_t::request_id id, const void *&data,size_t &size)
{
	request r;
	r.id = 0;

	CRaptorLock lock(m_mutex);

	if (m_replies.size() > 0)
	{
		vector<request>::const_iterator it = m_replies.begin();
		do
		{
			r = *it++;
		} while ((it != m_replies.end()) && (r.id != id));

		if (r.id == id)
		{
			it--;
			m_replies.erase(it);
		}
		else
			return false;
	}
	else
	{
		//	A small delay to wait for a reply ?
		//	A semaphore would be better
		Sleep(1);
		return false;
	}

	data = r.msg;
	size = r.size;
	return true;
}

bool CRaysDeamon::handleRequest(request_handler_t::request_id id,const void *data,size_t size)
{
	request rq;
	rq.id = id;
	rq.reply = false;
	rq.size = size;
	rq.msg = (MSGSTRUCT*)new unsigned char[size];
	memcpy(rq.msg,data,size);

	if (size > MSGSIZE)
		std::cout << "Extra data on message ignored !" << std::endl;

	switch(rq.msg->msg_id)
	{
		case DMN_STATUS:
		{
			dmnStatus(rq);
			break;
		}
		case DMN_DISPATCHJOB:
		{
			dispatchJob(rq);
			break;
		}
		case JOB_START:
		{
			break;
		}
		case OBJ_PLUGIN:
		{
			objPlugin(rq);
			break;
		}
		default:
		{
			std::cout << "Rays2 Deamon does not handle message " << rq.msg->msg_id << std::endl;
			return false;
			break;
		}
	}

	return true;
}


void CRaysDeamon::dmnStatus(request &rq)
{
	std::cout << "Rays2 Deamon status returned" << std::endl;
	unsigned int nbProcs = 0;
	unsigned int nbProcsAvailable = 0;
	float jobDone = 0;
	for (unsigned int i = 0; i < m_WorkUnits.size(); i++)
	{
		const CRaysDeamon::WORKUNITSTRUCT &w = m_WorkUnits[i];
		nbProcs += w.nbProcs;
		nbProcsAvailable += w.nbProcsAvailable;
		jobDone += w.jobDone;
	}
	rq.msg->msg_id = DMN_STATUS;
	rq.msg->msg_data[0] = nbProcs;
	rq.msg->msg_data[1] = nbProcsAvailable;
	rq.msg->msg_data[2] = (RAYS_MSG_ID)(floor(jobDone));
	rq.msg->msg_data[3] = 0;
	rq.msg->msg_data[4] = this->getAddr();
	rq.reply = true;

	CRaptorLock lock(m_mutex);
	m_replies.push_back(rq);
}

void CRaysDeamon::dispatchJob(request &rq)
{
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	stringstream		WUID;
	char				workUnit[MAX_PATH];

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	si.lpTitle = "Rays 2 Workunit";

	int		workUnitID = rq.msg->msg_data[0];
	int		serverPort = rq.msg->msg_data[3];
	DWORD	nbProcs = rq.msg->msg_data[1];
	DWORD	serverIP = rq.msg->msg_data[2];
	DWORD	priority = rq.msg->msg_data[4];
	DWORD	creationFlag = CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE;

	WUID << "-i " << workUnitID << " ";
	WUID << "-p " << serverPort << " ";
	WUID << "-a " << serverIP << " ";

	// creating work unit
	unsigned char* raw_data = (unsigned char*)(rq.msg) + sizeof(MSGSTRUCT);
	if (0 == CreateProcess((const char*)raw_data,	// pointer to name of executable module
							const_cast<char*>(WUID.str().c_str()),		// pointer to command line string
							NULL,		// process security attributes
							NULL,		// thread security attributes
							TRUE,		// handle inheritance flag
							creationFlag | priority,// creation flags
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

	//rq.msg->msg_header = MSG_START;
	rq.msg->msg_id = DMN_DISPATCHJOB;
	//rq.msg->msg_tail = MSG_END;
	//rq.msg->msg_size = 0;
	rq.msg->msg_data[0] = workUnitID;
	rq.msg->msg_data[1] = (DWORD)(pi.hProcess);	// TODO: will not work in 64 bits
	rq.msg->msg_data[2] = (DWORD)(pi.hThread);
	rq.msg->msg_data[3] = pi.dwProcessId;
	rq.msg->msg_data[4] = pi.dwThreadId;
	rq.reply = true;

	CRaptorLock lock(m_mutex);
	m_replies.push_back(rq);

	// now the job starts
	::ResumeThread(pi.hThread);

	stringstream pid;
	pid << "Creating new WorkUnit: pid: ";
	pid << pi.dwProcessId;
	pid << ", tid: ";
	pid << pi.dwThreadId;
	std::cout << pid.str() << std::endl;
}

void CRaysDeamon::objPlugin(request &rq)
{
	unsigned char* raw_data = (unsigned char*)(rq.msg) + sizeof(MSGSTRUCT);

	//	Install plugin only if not present.
	//	future version will hanle plugin version
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	string pname = (const char*)(&raw_data[0]);
	bool install = false;
	bool lastfile = false;

	hFind = FindFirstFile(pname.c_str(), &ffd);
	lastfile = (INVALID_HANDLE_VALUE == hFind);

	while (!install && !lastfile)
	{
		if (pname.compare(ffd.cFileName) &&
			(rq.msg->msg_data[1] > ffd.ftLastWriteTime.dwLowDateTime))
			install = true;
		else
			lastfile = !FindNextFile(hFind, &ffd);
	}

	if (install)
	{
		std::cout << "Installing plugin : " << pname.c_str();
		HANDLE f = CreateFile(pname.c_str(),
			GENERIC_WRITE,
			0,	// access mode: exclusive
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (NULL != f)
		{
			size_t len = pname.length() + 1;
			DWORD written = 0;
			//BOOL b = WriteFile(f, raw_data + len, rq.msg->msg_size - len, &written, NULL);

			//if (!b || (written != rq.msg->msg_size))
			//	std::cout << "Error installing plugin !" << std::endl;
			//	Delete file ?
			CloseHandle(f);
		}
	}
}

bool CRaysDeamon::onClientClose(const CClientSocket &client)
{
	std::cout << "Rays Server " << &client << " closed connection to RaysDeamon" << std::endl;
	return false;
}

size_t CRaysDeamon::onNewClient(const CClientSocket &client)
{
	std::cout << "Rays Server " << &client << " connected to RaysDeamon" << std::endl;
	return getNumClients();
}

bool CRaysDeamon::start(const CCmdLineParser& cmdline )
{
	vector<string> wus;
	vector<unsigned int> cpus;
	CRaysDeamon::WORKUNITSTRUCT wu;

	cmdline.getValue("w", wus);
	cmdline.getValue("c", cpus);
	if (wus.size() > 0)
	{
		for (unsigned int i = 0; i < wus.size(); i++)
		{
			unsigned int cpu = 4;
			if (i < cpus.size())
				cpu = cpus[i];

			wu.path = wus[i];
			wu.nbProcs = cpu;
			wu.nbProcsAvailable = cpu;
			wu.connection = NULL;
			wu.jobDone = 0;
			wu.active = false;
			m_WorkUnits.push_back(wu);
		}
	}
	else
	{
		std::cout << "Deamon launched with no work units. Exiting, bye!" << std::endl;
		return false;
	}

	std::string addrStr = "127.0.0.1";
	unsigned short port = 2048;
	cmdline.getValue("port",port);
	cmdline.getValue("host_addr",addrStr);
	bool res = startServer(addrStr,port);
    if (res)
    {
		std::cout << "Deamon Server ready on port ";
		std::cout << port;
		std::cout << " at host ";
		std::cout << addrStr;
		std::cout << std::endl;
	
		std::cout << m_WorkUnits.size() << " work units available:" << std::endl;
		for (unsigned int i = 0; i < m_WorkUnits.size(); i++)
		{
			const CRaysDeamon::WORKUNITSTRUCT &w = m_WorkUnits[i];
			std::cout << "  WorkUnit " << i << ": " << w.nbProcs << " processors from " << w.path << std::endl;
		}
		return true;
    }
    else
    {
		std::cout << "Deamon Server couldn't be started on port ";
		std::cout << port;
		std::cout << " at host ";
		std::cout << addrStr;
		std::cout << std::endl;
		return false;
    }
}

	
bool CRaysDeamon::stopServer(void)
{
	bool res = CServer<CServerSocket, CClientSocket>::stopServer();

	if (res)
		std::cout << "Deamon Server stopped successfully" << std::endl;
	else
		std::cout << "Deamon Server failed to stop without errors" << std::endl;

	return res;
}

