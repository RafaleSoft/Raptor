// ServerCmdLine.cpp: implementation of the ServerCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerCmdLine.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ServerCmdLine::ServerCmdLine()
{
    addrStr = "127.0.0.1";
	port = 2048;
	width = 256;
	height = 256;
    parseIP = false;
    parsePort = false;
	parseWidth = false;
	parseHeight = false;
}

ServerCmdLine::~ServerCmdLine()
{

}

void ServerCmdLine::Parse(int argc, char *argv[])
{
	int i = 0;
	while (i < argc)
	{
		if ((!strcmp("-port",argv[i])) && (i+1 < argc))
		{
			i++;
			port = (unsigned short)(0xffff & atoi(argv[i++]));
		}
		else if ((!strcmp("-host",argv[i])) && (i+1 < argc))
		{
			i++;
			addrStr = argv[i++];
		}
		else if ((!strcmp("-width",argv[i])) && (i+1 < argc))
		{
			i++;
			width = atoi(argv[i++]);
		}
		else if ((!strcmp("-height",argv[i])) && (i+1 < argc))
		{
			i++;
			height = atoi(argv[i++]);
		}
		else
			i++;
	}
}

