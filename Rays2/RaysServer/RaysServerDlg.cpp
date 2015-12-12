#include "stdafx.h"
#include <windows.h>
#include <vcclr.h>
#include "RaysServerDlg.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "RaptorNetwork/Network.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif

#include "ServerTransport.h"
#include "DeamonManager.h"

using namespace RaysServer;

RaysServerDlg::RaysServerDlg()
	:m_started(false),m_pTransport(NULL),m_pDeamonManager(NULL)
{
	m_pTransport = new CServerTransport();
	m_pDeamonManager = new CDeamonManager();
}

RaysServerDlg::~RaysServerDlg()
{
	if (NULL != m_pTransport)
		delete m_pTransport;
	if (NULL != m_pDeamonManager)
		delete m_pDeamonManager;
}

char* RaysServerDlg::convertSystemString(System::String^ str)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(str);

	size_t convertedChars = 0;
	size_t  sizeInBytes = ((str->Length + 1) * 2);
	char    *ch = (char *)malloc(sizeInBytes);

	wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

	return ch;
}

bool RaysServerDlg::Start(int argc,char *argv[])
{
#ifdef WIN32
	if (!Network::initSocketLayer())
		return 1;
#endif

	if (!m_started)
	{
		char* addrStr = "127.0.0.1";
		unsigned short port = 2048;

		CCmdLineParser parser;
		parser.addOption("port","p",port);
		parser.addOption("host_addr","a",addrStr);
		parser.parse(argc,argv);

		parser.getValue("port",port);
		parser.getValue("host_addr",addrStr);
		if (m_pTransport->startServer(addrStr,port))
			m_started = !m_started;
	}
	else
	{
		if (m_pTransport->stopServer())
			m_started = !m_started;
	}

	return m_started;
}


bool RaysServerDlg::Quit(void)
{
	return !m_started;
}