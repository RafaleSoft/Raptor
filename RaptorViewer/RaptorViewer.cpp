// RaptorViewer.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"

#if !defined(AFX_RAPTORSERVER_H__4595FAB9_F62A_4C9C_8356_9E204107AA14__INCLUDED_)
	#include "RaptorClient.h"
#endif
#if !defined(AFX_CMDLINEPARSER_H__D7D8768A_3D97_491F_8493_588972A3CF62__INCLUDED_)
	#include "ToolBox/CmdLineParser.h"
#endif


int main(int argc, char* argv[])
{
#ifdef WIN32
	if (!Network::initSocketLayer())
		return -1;
#endif

	unsigned short width = 320;
	unsigned short height = 240;
	char* file = (char*)"Demo.xml";
	CCmdLineParser parser;
	parser.addOption("port","p",(unsigned short)2048);
	parser.addOption("width","w",width);
	parser.addOption("height","h",height);
	parser.addOption("r_width","rw",(unsigned short)256);
	parser.addOption("r_height","rh",(unsigned short)256);
	parser.addOption("file","f",file);
	parser.addOption("host_addr","a",std::string("127.0.0.1"));
	parser.parse(argc,argv);

	CRaptorClient	client;
	if (client.start(parser))
	{
		parser.getValue("file",file);
		if (client.load(file))
		{
			parser.getValue("width",width);
			parser.getValue("height",height);
			if (client.run(width,height))
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
	int res = main(__argc,__argv);
    return res;
}
#endif

