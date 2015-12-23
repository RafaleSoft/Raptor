#include "stdafx.h"
#include <vcclr.h>
#include <windows.h>
#include "RaysClientDlg.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif

using namespace RaysClient;

RaysClientDlg::RaysClientDlg()
	:m_started(false)
{
#ifdef WIN32
	Network::setNbConnectAttempts(1);
	if (!Network::initSocketLayer())
		AddLog("Network layer not initialized properly.");
#endif
	//RaysServerUtils::setLog(gcnew RaysServerDlg::RaysLogger(this));
	m_raysClient = new CClient<CClientSocket>();
	AddLog("Rays Client initialized.");
}

RaysClientDlg::~RaysClientDlg()
{
	if (NULL != m_raysClient)
		delete m_raysClient;
}

char* RaysClientDlg::convertSystemString(System::String^ str)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(str);

	size_t convertedChars = 0;
	size_t  sizeInBytes = ((str->Length + 1) * 2);
	char    *ch = (char *)malloc(sizeInBytes);

	wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

	return ch;
}

bool RaysClientDlg::Start(int argc,char *argv[])
{
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
		if (m_raysClient->connectToServer(addrStr,port))
			m_started = !m_started;
		if (m_started)
			AddLog("Client connected to server. Listening ...");
		else
			AddLog("Client unable to connect to server !");
	}
	else
	{
		if (m_raysClient->disconnectServer())
			m_started = !m_started;
		if (!m_started)
			AddLog("Client stopped.");
		else
			AddLog("Client unable to stop !");
	}

	return m_started;
}


bool RaysClientDlg::Quit(void)
{
	return !m_started;
}