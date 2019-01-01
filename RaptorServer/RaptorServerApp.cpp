// RaptorServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORSERVER_H__713A4063_7F42_4900_B42D_6574E4FA796C__INCLUDED_)
    #include "RaptorServer.h"
#endif
#if !defined(AFX_NETWORK_H__AC9D546D_A00A_4BFC_AC0C_288BE137CD20__INCLUDED_)
    #include "RaptorNetwork/Network.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif



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

	CRaptorServer	*p_Server = new CRaptorServer;
    if (p_Server->Start(parser))
		return (p_Server->Stop() ? 1 : 0);
	else
		return -1;
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

