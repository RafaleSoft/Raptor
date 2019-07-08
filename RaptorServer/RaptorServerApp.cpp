/***************************************************************************/
/*                                                                         */
/*  RaptorServerApp.cpp                                                           */
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
	parser.addOption("host_addr","a",std::string("127.0.0.1"));
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
	int res = main(__argc,__argv);
    return res;
}
#endif

