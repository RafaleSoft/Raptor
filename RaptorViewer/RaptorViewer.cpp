// RaptorViewer.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"

#if !defined(AFX_RAPTORSERVER_H__4595FAB9_F62A_4C9C_8356_9E204107AA14__INCLUDED_)
	#include "RaptorClient.h"
#endif
#if !defined(AFX_CLIENTCMDLINE_H__D2547634_67C1_4608_8A82_6360CFBF4A42__INCLUDED_)
	#include "ClientCmdLine.h"
#endif


int main(int argc, char* argv[])
{
#ifdef WIN32
	if (!Network::initSocketLayer())
		return -1;
#endif

	ClientCmdLine commandLine;
	commandLine.Parse(argc,argv);
	
	CRaptorClient	client;
	if (client.start(commandLine))
	{
		if (client.load(commandLine.file))
		{
			if (client.run(commandLine.width,commandLine.height))
			{
				if (!client.stop())
				{
					cout << "Raptor client failed to stop." << endl;
					return -3;
				}
			}
			else
			{
				cout << "Raptor client failed to run." << endl;
				return -2;
			}
		}
	}
	else
	{
		cout << "No Raptor server available." << endl;
	}		

	return 0;
}

#ifdef WIN32
int WINAPI WinMain(	HINSTANCE /*hInstance*/,  // handle to current instance
					HINSTANCE /*hPrevInstance*/,  // handle to previous instance
                    LPSTR /*lpCmdLine*/,      // pointer to command line
                    int /*nCmdShow*/)          // show state of window
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

