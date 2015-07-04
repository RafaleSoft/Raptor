// ClientCmdLine.cpp: implementation of the ServerCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientCmdLine.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ClientCmdLine::ClientCmdLine()
{
    addrStr = "127.0.0.1";
	port = 2048;
	width = 320;
	height = 240;
	r_width = 256;
	r_height = 256;
	file = "Demo.xml";
}

ClientCmdLine::~ClientCmdLine()
{

}

void ClientCmdLine::Parse(int argc, char *argv[])
{
	int i = 0;
	while (i < argc)
	{
		if ((!strcmp("-port",argv[i])) && (i+1 < argc))
		{
			i++;
			port = (unsigned short)atoi(argv[i++]);
		}
		else if ((!strcmp("-host",argv[i])) && (i+1 < argc))
		{
			i++;
			addrStr = argv[i++];
		}
		else if ((!strcmp("-width",argv[i])) && (i+1 < argc))
		{
			i++;
			width = (unsigned short)atoi(argv[i++]);
		}
		else if ((!strcmp("-height",argv[i])) && (i+1 < argc))
		{
			i++;
			height = (unsigned short)atoi(argv[i++]);
		}
		else if ((!strcmp("-r_width",argv[i])) && (i+1 < argc))
		{
			i++;
			r_width = (unsigned short)atoi(argv[i++]);
		}
		else if ((!strcmp("-r_height",argv[i])) && (i+1 < argc))
		{
			i++;
			r_height = (unsigned short)atoi(argv[i++]);
		}
		else if ((!strcmp("-file",argv[i])) && (i+1 < argc))
		{
			i++;
			file = argv[i++];
		}
		else
			i++;
	}
}

